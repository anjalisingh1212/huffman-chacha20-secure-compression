#!/bin/bash

rm -f run

if gcc main.c compressor.c decompressor.c huffman.c encryptor.c decryptor.c utils.c -o run -lsodium; then
	echo "------------Build successfully. Run using ./run------------"
else
	echo "------------Build failed.-------------"
fi
