//data_compressor.h
#ifndef COMPRESSOR_H__
#define COMPRESSOR_H__

#include <stdint.h>
#include <stdio.h>  // Needed for FILE*

#define BUFFER_GROWTH_CHUNK 128

/**
 * @brief Structure representing a Huffman-encoded character.
 */
typedef struct HuffmanCode {
    char data;     // Character to be encoded
    char *code;    // Huffman code as a binary string
} HuffmanCode;

/**
 * @brief Compresses an input file using Huffman encoding and writes to output file.
 *
 * @param input_file Path to the file to compress.
 * @param output_file Path to the compressed output file.
 * @return 0 on success, non-zero on failure.
 */
int compress_file(const char *input_file, const char *output_file);

/**
 * @brief Stores the frequency table into a buffer.
 *
 * @param buffer Pointer to dynamically allocated buffer.
 * @param bufferSize Pointer to size of buffer.
 * @param freq Array of byte frequencies.
 * @param freqSize Size of the freq array (should be 256).
 * @param N Number of unique bytes.
 * @return Offset after inserting the frequency table.
 */
int save_frequency_table(unsigned char **buffer, size_t *bufferSize, uint32_t *freq, size_t freqSize, int N);

/**
 * @brief Encodes the file content into Huffman codes and appends it to the buffer.
 *
 * @param fpI Input file pointer.
 * @param buffer Pointer to data buffer to write into.
 * @param offset Offset in the buffer to start writing.
 * @param buffSize Current size of the buffer.
 * @param code_table Huffman code table for each byte.
 */
void encode_and_write_data(FILE *fpI, unsigned char **buffer, int *offset, size_t *buffSize, char *code_table[]);

#endif  // COMPRESSOR_H__