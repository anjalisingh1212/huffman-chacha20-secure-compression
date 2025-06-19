//data_compressor.h
#ifndef COMPRESSOR_H__
#define COMPRESSOR_H__

/*
 * @brief Header for data compression function
 */


typedef struct HuffmanCode{
	char data;
	char *code;
}HuffmanCode;

//Function to comress file
int compress_file(const char *input_file, const char *output_file);
 
#endif  //COMPRESSOR_H__
