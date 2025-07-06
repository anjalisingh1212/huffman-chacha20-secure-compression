//Utils.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Function to handle errors by printing the error message and exiting
void errExit(const char *str){
	perror(str);
	exit(EXIT_FAILURE);
}

const char *compressed_exts[] = {
    ".jpg", ".png", ".mp3", ".zip", ".rar", ".gz", ".mp4", ".avi", ".pdf", NULL
};

int is_compressed_format(const char *filename) {
    const char **ext = compressed_exts;
    while (*ext) {
        if(strstr(filename, *ext)) {
            return 1; // Found a compressed format
        }
        ext++;
    }
    return 0;
}
// Function to print data in hexadecimal format
// This is useful for debugging and visualizing binary data
void printHex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

