// main.c
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/stat.h>
#include "compressor.h"
#include "decompressor.h"
#include "utils.h"

int main(){
	// Main function to run the compression and decompression program
	// It provides a menu for the user to choose between compression and decompression

	char input_file[100] = "\0"; // Stores the name of the input file
	char output_file[100] = "\0"; // Stores the name of the output file
	int choice = 10; // Variable to store user choice
	char line[5] = "\0"; // To clear the input buffer

	printf("=====================================================\n");
	printf("   Welcome to the File Compressor + Encryptor Tool\n");
	printf("=====================================================\n");
	printf("Supported File Types   : .txt, .log, .cfg, .csv, .c, .py, etc.\n");
	printf("Not Recommended Types   : .jpg, .png, .zip, .mp4, .exe, etc.\n");
	printf("Why?                    : Already compressed files may grow larger\n");
	printf("Note                    : Encryption adds overhead (salt + nonce + MAC)\n");
	printf("-----------------------------------------------------\n");


	while(choice != 0){
		input_file[0] = '\0'; // Reset input file name
		output_file[0] = '\0'; // Reset output file name
		struct stat input_file_stat;
		struct stat output_file_stat;
		printf("\n-------------------------------\n");
		printf("For compression enter 1:\n");
		printf("For decompression enter 2:\n");
		printf("For exit enter 0:\n");
		printf("Enter your choice:\n");
		printf("-------------------------------\n");

		if (fgets(line, sizeof(line), stdin) == NULL) {
    		printf("Error reading input.\n");
    		continue;
		}

		if (sscanf(line, "%d", &choice) != 1) {
    		printf("Invalid input. Please enter a number.\n");
    		continue;
		}
		switch(choice){
			case 0:
				printf("Exiting program...\n");
				break;
			case 1:
				// Compression option
				printf("Enter the file name for compression:\n");
				if(fgets(input_file, sizeof(input_file), stdin) == NULL) {
					printf("Error reading input file name.\n");
					continue;
				}
				input_file[strcspn(input_file, "\n")] = 0; // Remove newline character
				
				printf("Enter the output file name for storing compressed data:\n");
				if(fgets(output_file, sizeof(output_file), stdin) == NULL) {
					printf("Error reading output file name.\n");
					continue;
				}
				
				output_file[strcspn(output_file, "\n")] = 0; // Remove newline character
				
				if(strlen(input_file) == 0 || strlen(output_file) == 0) {
					printf("Input or output file name cannot be empty.\n");
					continue;
				}
				if(strcmp(input_file, output_file) == 0) {
					printf("Input and output file names cannot be the same.\n");
					continue;
				}
				if(stat(input_file, &input_file_stat) != 0) {
					printf("Error accessing input file. Please check the file name.\n");
					continue;
				}
				if(is_compressed_format(input_file)) {
					printf("Unsupported file type for compression: %s\n", input_file);
					printf("Please choose a different file type.\n");
					continue;
				}
				if (input_file_stat.st_size <= 50) {
    				printf("Warning: Input file is very small (%ld bytes). Compression may be ineffective.\n", input_file_stat.st_size);
				}
				if(stat(output_file, &output_file_stat) == 0) {
					printf("Warning: Output file %s already exists. Please choose a different name.\n", output_file);
					continue;
				}
				if(compress_file(input_file, output_file) == -1) {
					printf("\nCompression failed. Please check the input file.\n");
				} else {
					stat(output_file, &output_file_stat);
					printf("\n===== Compression Summary =====\n");
					printf("Input File Size    : %ld bytes\n", input_file_stat.st_size);
					printf("Output File Size   : %ld bytes\n", output_file_stat.st_size);
					if(input_file_stat.st_size > output_file_stat.st_size) {
						printf("Compression saved %.2f%% of the original file size.\n", 
							((double)(input_file_stat.st_size - output_file_stat.st_size) / input_file_stat.st_size) * 100);	
					} else {
						printf("Compressed file size is increased by %.2f%%. compared to the original.\n", ((double)(output_file_stat.st_size - input_file_stat.st_size) / input_file_stat.st_size) * 100);
						printf("Compression was not effective. The output file is larger than the input file.\n");
						printf("This may happen due to:\n");
    					printf(" - Overhead from metadata or encryption (e.g., salt, nonce, MAC)\n");
    					printf(" - Small input size\n");
    					printf("Compression + Encryption tradeoff: Sometimes encryption adds fixed overhead even if data compresses well.\n");
					}
				}
				break;

			case 2:
				// Decompression option
				printf("Enter the file name for decompression:\n");
				if(fgets(input_file, sizeof(input_file), stdin) == NULL) {
					printf("Error reading input file name.\n");
					continue;
				}
				input_file[strcspn(input_file, "\n")] = 0; // Remove newline character
					
				printf("Enter the output file name for storing decompressed data:\n");
				if(fgets(output_file, sizeof(output_file), stdin) == NULL) {
					printf("Error reading output file name.\n");
					continue;
				}
				output_file[strcspn(output_file, "\n")] = 0; // Remove newline character
				if(strlen(input_file) == 0 || strlen(output_file) == 0) {
					printf("Input or output file name cannot be empty.\n");
					continue;
				}
				if(strcmp(input_file, output_file) == 0) {
					printf("Input and output file names cannot be the same.\n");
					continue;
				}
				
				if(stat(input_file, &input_file_stat) != 0) {
					printf("Error accessing input file. Please check the file name.\n");
					continue;
				}

				if(stat(output_file, &output_file_stat) == 0) {
					printf("Warning: Output file %s already exists. Please choose a different name.\n", output_file);
					continue;
				}
				if(decompress_file(input_file, output_file) == -1) {
					printf("\nDecompression failed. Please check the input file.\n");
				} else {
					printf("\nDecompression successful. Output written to %s\n", output_file);
				}
				break;

			default:
				printf("Invalid value!\n");
				break;
			
		}
	}
	return 0;
}

