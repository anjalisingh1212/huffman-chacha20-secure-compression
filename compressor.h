//data_compressor.h
#ifndef COMPRESSOR_H__
#define COMPRESSOR_H__

/*
 * @brief Header for data compression function
 */

#define BUFFER_GROWTH_CHUNK 128
typedef struct HuffmanCode{
	char data;
	char *code;
}HuffmanCode;

//Function to comress file
int compress_file(const char *input_file, const char *output_file);
int save_frequency_table(unsigned char **buffer, size_t *bufferSize, int *freq, size_t freqSize, int N);
void encode_and_write_data(FILE *fpI, unsigned char **buffer, int *offset, size_t *buffSize, char *code_table[]);
#endif  //COMPRESSOR_H__
