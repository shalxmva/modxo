package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"log"
	"os"
	"time"
)

const (
	UF2_MAGIC_START0 = 0x0A324655
	UF2_MAGIC_START1 = 0x9E5D5157
	UF2_MAGIC_END    = 0x0AB16F30
)

type Block struct {
	address   uint32
	blockSize uint32
	bytes     []byte
}

func (b *Block) encode(blockNo, numBlocks uint32) []byte {
	familyID := uint32(0xe48bff56)
	flags := uint32(0x2000) // withFamilyId flag
	blockData := make([]byte, 512)
	binary.LittleEndian.PutUint32(blockData[0:], UF2_MAGIC_START0)
	binary.LittleEndian.PutUint32(blockData[4:], UF2_MAGIC_START1)
	binary.LittleEndian.PutUint32(blockData[8:], flags)
	binary.LittleEndian.PutUint32(blockData[12:], b.address)
	binary.LittleEndian.PutUint32(blockData[16:], b.blockSize)
	binary.LittleEndian.PutUint32(blockData[20:], blockNo)
	binary.LittleEndian.PutUint32(blockData[24:], numBlocks)
	binary.LittleEndian.PutUint32(blockData[28:], familyID)
	copy(blockData[32:], b.bytes)
	binary.LittleEndian.PutUint32(blockData[508:], UF2_MAGIC_END)
	return blockData
}

func (b *Block) decode(data []byte) {
	b.address = binary.LittleEndian.Uint32(data[12:])
	b.blockSize = binary.LittleEndian.Uint32(data[16:])
	b.bytes = data[32 : 32+b.blockSize]
}

func getFlashAddressMask(size int) uint32 {
	maskBits := 0
	size -= 1
	for size > 0 {
		size >>= 1
		maskBits++
	}
	memCapacity := 1 << maskBits
	addressMask := memCapacity - 1
	return uint32(addressMask)
}

func getUF2RomAddrMaskBlock(maskAddress, romAddrMask uint32) []Block {
	blocks := []Block{}
	address := maskAddress
	data := make([]byte, 256)
	binary.LittleEndian.PutUint32(data[0:], romAddrMask)
	for i := 0; i < 16; i++ {
		currentBlock := Block{address, 256, data}
		blocks = append(blocks, currentBlock)
		address += currentBlock.blockSize
	}
	return blocks
}

func getUF2BlocksFromFile(fileData []byte, flashROMAddress uint32, spinner chan bool) []Block {
	blocks := []Block{}
	blockAddress := flashROMAddress
	dataSize := len(fileData)
	blockSize := 256

	spinnerRunning := true
	go func() {
		spinnerChars := []rune{'|', '/', '-', '\\'}
		i := 0
		for spinnerRunning {
			fmt.Printf("\rProcessing blocks %c", spinnerChars[i])
			i = (i + 1) % len(spinnerChars)
			time.Sleep(100 * time.Millisecond)
		}
		close(spinner)
	}()

	for i := 0; i < dataSize; i += blockSize {
		time.Sleep(10 * time.Millisecond) // Adding some delay to make the spinner visible
		end := i + blockSize
		if end > dataSize {
			end = dataSize
		}
		currentBlock := Block{blockAddress, 256, fileData[i:end]}
		blocks = append(blocks, currentBlock)
		blockAddress += uint32(blockSize)
	}
	spinnerRunning = false
	<-spinner
	fmt.Println("\rProcessing blocks done!")

	return blocks
}

func writeUF2File(data []byte, outputFilename string, flashROMAddress, maskAddress uint32) {
	romAddrMask := getFlashAddressMask(len(data))
	headerBlocks := getUF2RomAddrMaskBlock(maskAddress, romAddrMask)
	spinner := make(chan bool)
	dataBlocks := getUF2BlocksFromFile(data, flashROMAddress, spinner)
	uf2Blocks := append(headerBlocks, dataBlocks...)

	fmt.Printf("Header blocks written: %d\n", len(headerBlocks))
	fmt.Printf("Data blocks written: %d\n", len(dataBlocks))
	fmt.Printf("Total blocks written: %d\n", len(uf2Blocks))

	bytestream := bytes.Buffer{}
	for i, block := range uf2Blocks {
		blockData := block.encode(uint32(i), uint32(len(uf2Blocks)))
		bytestream.Write(blockData)
	}

	err := os.WriteFile(outputFilename, bytestream.Bytes(), 0644)
	if err != nil {
		log.Fatalf("Failed to write UF2 file: %v", err)
	}
}

func readUF2File(inputFilename string, spinner chan bool) []byte {
	inputData, err := os.ReadFile(inputFilename)
	if err != nil {
		log.Fatalf("Failed to read input file: %v", err)
	}

	if len(inputData)%512 != 0 {
		log.Fatalf("Input file is not a valid UF2 file")
	}

	blocks := map[uint32][]byte{}
	var maxAddr uint32 = 0

	spinnerRunning := true
	go func() {
		spinnerChars := []rune{'|', '/', '-', '\\'}
		i := 0
		for spinnerRunning {
			fmt.Printf("\rProcessing blocks %c", spinnerChars[i])
			i = (i + 1) % len(spinnerChars)
			time.Sleep(100 * time.Millisecond)
		}
		close(spinner)
	}()

	for i := 0; i < len(inputData); i += 512 {
		block := Block{}
		block.decode(inputData[i : i+512])
		blocks[block.address] = block.bytes
		if block.address+block.blockSize > maxAddr {
			maxAddr = block.address + block.blockSize
		}
	}
	spinnerRunning = false
	<-spinner
	fmt.Println("\rProcessing blocks done!")

	outputData := make([]byte, maxAddr)
	for addr, data := range blocks {
		copy(outputData[addr:], data)
	}

	return outputData
}

func convertUF2ToBin(inputFilename, outputFilename string) {
	fmt.Printf("Converting UF2 to BIN: %s -> %s\n", inputFilename, outputFilename)
	spinner := make(chan bool)
	outputData := readUF2File(inputFilename, spinner)
	err := os.WriteFile(outputFilename, outputData, 0644)
	if err != nil {
		log.Fatalf("Failed to write binary file: %v", err)
	}
	fmt.Println("Conversion to BIN completed successfully.")
	fmt.Printf("Total bytes written: %d\n", len(outputData))
}

func packBios(inputBuffer []byte, filename string) {
	writeUF2File(inputBuffer, filename+".uf2", 0x10040000, 0x1003F000)
}

func main() {
	fmt.Println("****************************************")
	fmt.Println("*               ModXo-Go               *")
	fmt.Println("*      by Milenko (@dtoxmilenko)       *")
	fmt.Println("*   UF2 Converter for ModXo Firmware   *")
	fmt.Println("*  https://github.com/shalxmva/modxo/  *")
	fmt.Println("****************************************")

	if len(os.Args) != 4 {
		fmt.Println("Usage: go run modxo_go.go <command> <input_file> <output_file>")
		fmt.Println("Commands: to_uf2, to_bin")
		os.Exit(1)
	}

	command := os.Args[1]
	inputFile := os.Args[2]
	outputFile := os.Args[3]

	switch command {
	case "to_uf2":
		fmt.Printf("Converting BIN to UF2: %s -> %s.uf2\n", inputFile, outputFile)
		inputBuffer, err := os.ReadFile(inputFile)
		if err != nil {
			log.Fatalf("Failed to read input file: %v", err)
		}
		packBios(inputBuffer, outputFile)
		fmt.Println("Conversion to UF2 completed successfully.")
	case "to_bin":
		convertUF2ToBin(inputFile, outputFile)
	default:
		fmt.Println("Unknown command:", command)
		fmt.Println("Usage: go run modxo_go.go <command> <input_file> <output_file>")
		fmt.Println("Commands: to_uf2, to_bin")
		os.Exit(1)
	}

	fmt.Println("****************************************")
	fmt.Println("*        Conversion Finished           *")
	fmt.Println("****************************************")
}
