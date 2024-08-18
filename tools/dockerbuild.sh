#!/bin/sh

WS2812=${WS2812_LED:-OFF}
CLEAN=${CLEAN:-0}
BUILD_TYPE=${BUILD_TYPE:-"Debug"} # Default build type
BIOS2UF2=${BIOS2UF2:-"bios.bin bios/*.bin"}

WORKING_DIR=$(pwd)
BUILD_DIR=$(pwd)/build
OUT_DIR=$(pwd)/out
BIOS2UF2_TOOL=$(pwd)/tools/flashbin_to_uf2.py

mkdir -p "$BUILD_DIR"
mkdir -p "$OUT_DIR"

cd "$BUILD_DIR"
cmake -DWS2812_LED=$WS2812_LED -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

if [ $CLEAN -eq 1 ]; then
    make clean
fi

make -j

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

mv "${BUILD_DIR}/modxo.bin" "${OUT_DIR}/"
mv "${BUILD_DIR}/modxo.uf2" "${OUT_DIR}/"
mv "${BUILD_DIR}/modxo.elf" "${OUT_DIR}/"

if [ -n "$BIOS2UF2" ]; then
    FULL_PATHS=""

    for file in $BIOS2UF2; do
        FULL_PATHS="$FULL_PATHS $WORKING_DIR/$file"
    done

    # Trim leading space
    FULL_PATHS=$(echo "$FULL_PATHS" | sed 's/^ //')

    python $BIOS2UF2_TOOL --output-dir "${OUT_DIR}" $FULL_PATHS
fi
