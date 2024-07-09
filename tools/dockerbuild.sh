#!/bin/sh

mkdir build
cd build
cmake ..
make
cd ..
mkdir out
mv build/modxo.bin out/
mv build/modxo.uf2 out/
