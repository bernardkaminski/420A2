#include <mpi.h>
#include <stdio.h>
#define TAG 0


void process(char* input_filename, char* output_filename)
{
	unsigned error;
	unsigned char *image, *new_image,*block;
	unsigned width, height;

	error = lodepng_decode32_file(&image, &width, &height, input_filename);
	if (error)
		printf("error %u: %s\n", error, lodepng_error_text(error));
	new_image = malloc(width * height * 4 * sizeof(unsigned char));

	// process image
	unsigned char value;
	int xyz = 123; // more computations
	MPI_Init(Null, Null);
	int size;

    // Get the number of processesu
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	int set_size = height/size;
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int position_in_global = rank * set_size;
	block = malloc(width * set_size * 4 * sizeof(unsigned char));

    
	for (int i = 0; i < set_size ; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// printf("thread num: %d\n", omp_get_num_threads());

			// printf("thread num: %d\n", omp_get_thread_num());
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
			//xyz = pow(sin(i + xyz), cos(xyz + j));
		}
	}
	
	//recieve all the results
	 int val = 0;
	 if (rank == 0) {
        // receive value of x
		int blk_size=(set_size*width*4 * sizeof(unsigned char));
	    memcpy(new_image,block,blk_size);
		for (int var = 1; var < size; ++var) {
			char* blk;
			MPI_Recv(&blk, 1, MPI_INT, var, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			memcpy(new_image+(blksize*var),blk,blk_size);
		}

        new
        /*
        MPI_Recv(
            void* data,
            int count,
            MPI_Datatype datatype,
            int source,
            int tag,
            MPI_Comm communicator,
            MPI_Status* status)
        */
    }
	else {
		 MPI_ISend(&block, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();

	
	
	lodepng_encode32_file(output_filename, new_image, width, height);

	free(image);
	free(new_image);
}

int main(int argc, char *argv[])
{
	char* input_filename = argv[1];
	char* output_filename = argv[2];

	process(input_filename, output_filename);

	return 0;
}
