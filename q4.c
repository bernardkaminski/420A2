#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lodepng.h"
#include <math.h>
#define TAG 0

void rec(int pixel, int address, unsigned char * new_image)
{
	if (pixel < 127)
	{
		new_image[address] = 127;
	}
	else
	{
		new_image[address] = pixel;
	}
}

void process(char* input_filename, char* output_filename)
{
	unsigned error;
	unsigned char *image, *block; // *new_image
	unsigned width, height;

	error = lodepng_decode32_file(&image, &width, &height, input_filename);
	if (error)
		printf("error %u: %s\n", error, lodepng_error_text(error));

	// process image
	unsigned char value;
	int xyz = 123; // more computations
	MPI_Init(NULL, NULL);
	int size;

    // Get the number of processesu
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	int set_size = height/size;
	int left_over = height%size;
	int special=0;
	if(left_over > 0)
	{
		special = left_over;

	}
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    	int position_in_global = rank * set_size;

    if (rank == (size-1) && special)
	{
		set_size += left_over;	
	}
	
	// int position_in_global = rank * set_size;
	block = malloc(width * set_size * 4 * sizeof(unsigned char));

	for (int i = 0; i < set_size ; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int offset_in_image = 4 * width * (position_in_global+i) + 4 * j;
			int offset= 4 * width * i + 4 * j;
			int r = image[offset_in_image]; // R
			rec(r, (offset), block);
			int g = image[offset_in_image + 1]; // G

			rec(g, offset + 1, block);
			int b = image[offset_in_image + 2]; // B
			rec(b, offset + 2, block);
			int a = image[offset_in_image + 3]; // A
			block[offset + 3] = a;
			xyz *= pow(sin(i + xyz), cos(xyz + j));
						xyz *= pow(sin(i + xyz), cos(xyz + j));

			xyz *= pow(sin(i + xyz), cos(xyz + j));

			xyz *= pow(sin(i + xyz), cos(xyz + j));

			xyz *= pow(sin(i + xyz), cos(xyz + j));

		}

	}
	
	//recieve all the results
	 int val = 0;
	 if (rank == 0) {
		char *new_image = malloc(width * height * 4 * sizeof(unsigned char));
        // receive value of x
		int blk_size=(set_size*width*4 * sizeof(unsigned char));
	    memcpy(new_image,block,blk_size);
		for (int var = 1; var < size; ++var) {
			int start = (blk_size*var);
			    if (var == (size-1) && special)
				{
					set_size += left_over;	
					blk_size=(set_size*width*4 * sizeof(unsigned char));
				}
			char* blk = malloc(blk_size);
			MPI_Recv(blk, blk_size, MPI_CHAR, var, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			memcpy(new_image+start,blk,blk_size);
			free(blk);
		}
		lodepng_encode32_file(output_filename, new_image, width, height);
		free(new_image);
    }
	else {
		MPI_Request request;
		int blk_size=(set_size*width*4 * sizeof(unsigned char));
		MPI_Isend(block, blk_size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &request);
	}
	
	MPI_Finalize();
	free(image);
}

int main(int argc, char *argv[])
{
	char* input_filename = argv[1];
	char* output_filename = argv[2];

	process(input_filename, output_filename);

	return 0;
}
