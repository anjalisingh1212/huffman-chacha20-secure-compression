// utils.h
#ifndef UTILS_H__
#define UTILS_H__

#include <stdint.h>

#define BUFFER_SIZE 4096    // Size of the buffer used for reading files
#define MAGIC_HEADER "ENC"  // Magic header for identifying compressed files
#define MAGIC_HEADER_LEN 3  // Length of the magic header

/*
* @brief Function to handle errors by printing the error message and exiting
 * 
 * @param str Error message to print
 */
void errExit(const char *str);

/*
 * @brief Function to check if a file is in a compressed format
 * 
 * @param filename Name of the file to check
 * @return 1 if the file is in a compressed format, 0 otherwise
 */
int is_compressed_format(const char *filename);

/*
 * @brief Function to print data in hexadecimal format
 * 
 * @param data Pointer to the data to print
 * @param len Length of the data
 */
void printHex(const uint8_t *data, size_t len);

#endif  //UTILS_H__