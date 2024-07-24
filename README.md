![Modchip + Axolotl = Modxo](images/logo.png)
#
Modxo (pronnounced as "Modsho") is a Xbox LPC Port firmware that converts the Raspberry Pi Pico
into an Original Xbox Modchip that allows running a bios.

### Future features
- Implement device communication (specially with LCD)

# How to Install
### 1. Requirements
- Working LPC Port
- Original Raspberry Pi Pico or RP2040 Zero (There are some clone boards that are not compatible)
- 4 100 Ohm resistors (tested with 1/4 W resistors)

### 2. Build Circuit

![Wiring diagram](images/wiring_diagram.png)

* Note: D0 is only needed by versions different to 1.6
* Note: LFrame pin connection is only needed by version 1.6. Also LPC Rebuild is Required

### 3. Flashing firmware

#### Packing Bios
1. Go to https://shalxmva.github.io/modxo/
2. Drag and Drop your bios file
3. UF2 File with bios image will be downloaded

#### Flashing steps
1. Connect Raspberry Pi Pico with BOOTSEL button pressed to a PC and one new drive will appear.
2. Copy Modxo.uf2 into the Raspberry Pi Pico Drive.
3. Reconnect Raspberry Pi Pico with BOOTSEL button pressed, so the previous drive will showup again.
4. Copy your bios UF2 file into the drive

# Docker Build
#### Setup
1 Build your base docker image with `docker build -t modxo-builder .`

#### Firmware Build
1. `docker compose run --rm builder`
2. output will be `out/modxo.uf2`

There are also some extra parameters that can be passed to the build script:

- WS2812: enables support for WS2812 LEDs (typically present in the RP2040 Zero boards). Default is disabled.

- CLEAN: triggers a clean build. Default is disabled.

- BUILD_TYPE: release|debug. Default is debug.

- BIOS2UF2: path (or glob) to the bios file(s) to be converted to UF2. Default is `bios.bin bios/*.bin`.

#### Packing Bios locally
1. Copy bios file to `bios.bin` in this directory or place any bios files in the `bios` directory
2. `docker compose run --rm bios2uf2`
3. output will be `out/[bios].uf2`

#### Development environment using VSCode and devcontainers
1. Just open this project in VSCode and click on the "Reopen in Container" button when prompted
2. Right-click on the CMakelists.txt file and select "Configure All Projects"
3. Build the project by pressing F7
