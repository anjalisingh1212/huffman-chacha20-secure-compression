// decompressor.h

#ifndef Decompressor_H__
#define Decompressor_H__

/** 
 * @brief Header for decompression function
 * 
 * @param input_file Path to the compressed input file.
 * @param output_file Path to the file where decompressed data will be written.
 * @return 0 on success, -1 on failure.
 */

int decompress_file(const char* input_file, const char* output_file);

#endif //Decompressor_H__
