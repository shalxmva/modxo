![Modchip + Axolotl = Modxo](images/logo.png)
# Modxo
Modxo (pronnounced as "Modsho") is a Xbox LPC Port firmware that converts the Raspberry Pi Pico
into an Original Xbox Modchip that allows running a bios.

## Future features
- Implement device communication (specially with LCD)

# Installation
## Requirements
- Working LPC Port
- Original Raspberry Pi Pico or RP2040 Zero (There are some clone boards that are not compatible)
- 4 100 Ohm resistors (tested with 1/4 W resistors)

## Build

![Wiring diagram](images/wiring_diagram.png)

Note: D0 is only needed by versions different to 1.6
Note: LFrame pin connection is only needed by version 1.6. Also LPC Rebuild is Required

## Flashing firmware

### Packing Bios
- Go to https://shalxmva.github.io/modxo/
- Drag and Drop your bios file
- UF2 File with bios image will be downloaded

### Flashing steps
1. Connect Raspberry Pi Pico with BOOTSEL button pressed to a PC and one new drive will appear.
2. Copy Modxo.uf2 into the Raspberry Pi Pico Drive.
3. Reconnect Raspberry Pi Pico with BOOTSEL button pressed, so the previous drive will showup again.
4. Copy your bios UF2 file into the drive

## Build Instructions
Todo 
### Windows
Todo
