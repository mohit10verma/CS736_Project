#!/bin/bash

pushd .
cd ../intel_aes_lib
./mk_lnx_lib64.sh
popd

out=bin/aes_example64.so
mkdir bin
rm $out

lvl= 
lvl=-O3
yasm=../yasm/yasm

mkdir -p obj/x64

gcc -g -shared -fPIC $lvl $opt -o $out src/main.c src/aes_example.c src/my_getopt.c -Isrc -I../intel_aes_lib/include -I../../../libfiber-master/include ../intel_aes_lib/lib/x64/intel_aes64.a

echo created $out
