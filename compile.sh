#!/bin/bash

rm -f run

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

if gcc -g -fsanitize=address main.c compressor.c decompressor.c huffman.c encryptor.c decryptor.c utils.c -o run -lsodium; then
	echo -e "${GREEN}------------Build successfully. Run using ./run------------${NC}"
else
	echo -e "${RED}------------Build failed.-------------${NC}"
fi
