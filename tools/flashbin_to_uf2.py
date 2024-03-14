#!/usr/bin/env python3
#
#   SPDX short identifier: BSD-2-Clause
#   Copyright 2024 Shalx <alexwinger@gmail.com>
#
#   Utility for generating UF2 flashing files for modxo (xbox pico modchip)
#
#   Usage:
#        usage: flashbin_to_uf2.py [-h] [-b BASE] [-m MASK] [file ...]
#

import sys
import struct
import subprocess
import re
import os
import os.path
import argparse
import json
import pathlib
from pathlib import Path
from time import sleep


def write_file(name, buf):
    with open(name, "wb") as f:
        f.write(buf)
    print("Wrote %d bytes to %s" % (len(buf), name))

###  UF2 Block Magic Numbers
UF2_MAGIC_START0 = 0x0A324655 # "UF2\n"
UF2_MAGIC_START1 = 0x9E5D5157 # 
UF2_MAGIC_END    = 0x0AB16F30 # 

class Block:
    def __init__(self, addr):
        self.block_size=256
        self.addr = addr
        self.bytes = bytearray(self.block_size)
        

    def encode(self, blockno, numblocks):
        familyId = 0xe48bff56
        flags = 0x2000       #withFamilyId flag
        hd = struct.pack("<IIIIIIII", UF2_MAGIC_START0, UF2_MAGIC_START1, flags, self.addr, self.block_size, blockno, numblocks, familyId
                         )
        hd += self.bytes[0:self.block_size]
        while len(hd) < 512 - 4:
            hd += b"\x00"
        hd += struct.pack("<I", UF2_MAGIC_END)
        return hd

def get_flash_address_mask(filename):
    file = os.stat(filename)
    size = file.st_size
    maskbits = 0
    
    if(size < 1):
        return 0

    size -= 1
    while size > 0:
        size >>= 1
        maskbits+=1
      
    mem_capacity = 1<<(maskbits)
    address_mask = mem_capacity - 1
    return address_mask

def get_uf2_blocks_from_file(binfilename, flashrom_address):
    blocks =[]
    block_address = flashrom_address
    with open(binfilename,"rb") as f:
         while True:
            current_block = Block(block_address)
            current_block.bytes = f.read(current_block.block_size)
            if not current_block.bytes:
                break

            blocks.append(current_block)
            block_address += current_block.block_size
    
    return blocks

def get_uf2_romaddr_mask_block( mask_address, romaddr_mask):
    blocks = []
    
    current_block = Block(mask_address)
    current_block.bytes = romaddr_mask.to_bytes(256, byteorder='little')

    #Copy 16 times a 256 block to fill a 4096 byte sector
    for i in range(16):
        blocks.append(current_block)

    return blocks

def write_uf2_file(binfilename, outputfilename, flashrom_address, mask_address):
    uf2blocks = []
    byte_stream = b""
    
    romaddr_mask = get_flash_address_mask(binfilename)
    if mask_address is not None:
        uf2blocks+=get_uf2_romaddr_mask_block(mask_address,romaddr_mask)

    uf2blocks+=get_uf2_blocks_from_file(binfilename, flashrom_address)
    
    total_blocks=len(uf2blocks)
    current_block = 0

    for block in uf2blocks:
        byte_stream += block.encode(current_block, total_blocks)
        current_block+=1

    print("Total blocks written: ",total_blocks)

    write_file(outputfilename, byte_stream)


def error(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description='Generate flashbios .uf2 files from .bin for RP2040 Flash')
    
    parser.add_argument('file', type=argparse.FileType('r'), nargs='+',
                        help='List of input ".bin" files')
    parser.add_argument('-b', '--base', dest='base', type=str,
                        default="0x10040000",
                        help='Sets the base address where the .bin file is going to be flashed')
    parser.add_argument('-m', '--mask-address', dest='mask', type=str,
                        default="0x1003F000",
                        help='Sets the base address where the flashrom address mask will be flashed')
    
    args = parser.parse_args()
    
    if not args.file or len(args.file)<1:
        error("Input Files not specified")

    baseaddress = int(args.base, 0)
    maskaddress = None

    if args.mask:
        maskaddress = int(args.mask, 0)

    for file in args.file:
        filePath = Path(file.name)
        outputfilename = filePath.with_suffix(".uf2")
        write_uf2_file(file.name, outputfilename, baseaddress, maskaddress)
        

if __name__ == "__main__":
    main()
