//Utils file

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void errExit(const char *str){
	perror(str);
	exit(EXIT_FAILURE);
}

void printHex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

