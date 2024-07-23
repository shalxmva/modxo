#!/bin/sh

mkdir -p build
cd build
cmake ..
make clean
make -j
cd ..
mkdir -p out
mv build/modxo.bin out/
mv build/modxo.uf2 out/
