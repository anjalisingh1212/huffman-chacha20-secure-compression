// utils.h

#ifndef UTILS_H__
#define UTILS_H__

/*
 * @brief Functions for utility
 */

#include <stdint.h>
#include <stddef.h>

void errExit(const char *str);

void printHex(const uint8_t *data, size_t len);

#define BUFFER_SIZE 4096
#define MAGIC_HEADER "ENC"
#define MAGIC_HEADER_LEN 3


#endif  //UTILS_H__
