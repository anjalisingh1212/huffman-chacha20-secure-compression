// main.c

#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include "compressor.h"
#include "decompressor.h"


int main(){
	
	char input_file[100];
	char output_file[100];
	int choice = 10;
	while(choice != 0){
		printf("-------------------------------\n");
		printf("For compression press 1:\n");
		printf("For decompression press 2:\n");
		printf("For exit press 0:\n");
		__fpurge(stdin);
		scanf("%d",&choice);
		switch(choice){
			case 0:
				printf("Exiting program...\n");
				break;
			case 1:
				/*
				printf("Enter the file name for compression:\n");
				scanf("%s",input_file);

				printf("Enter the output file name for storing compressed data:\n");
				scanf("%s",output_file);
				*/
				strcpy(input_file, "file.txt");
				strcpy(output_file, "ot.txt");
				printf("Proceeding for compression...\n");
				compress_file(input_file, output_file);
				break;
			case 2:
				/*printf("Enter the file name for compression:\n");
				scanf("%s",input_file);

				printf("Enter the output file name for storing compressed data:\n");
				scanf("%s",output_file);*/
				strcpy(input_file, "ot.txt");
				strcpy(output_file, "dcp.txt");
				printf("Proceeding for decompression. ..\n");
				decompress_file(input_file, output_file);
				break;
			default:
				printf("Invalid value!\n");
				break;
			}
	}
	return 0;
}

