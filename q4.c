#include <mpi.h>
#include <stdio.h>
#define TAG 0


void process(char* input_filename, char* output_filename)
{
	unsigned error;
	unsigned char *image, *new_image;
	unsigned width, height;

	error = lodepng_decode32_file(&image, &width, &height, input_filename);
	if (error)
		printf("error %u: %s\n", error, lodepng_error_text(error));
	new_image = malloc(width * height * 4 * sizeof(unsigned char));

	// process image
	unsigned char value;
	int xyz = 123; // more computations
	MPI_Init(Null, Null);

    // Get the number of processesu
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	int set_size = height/size;
    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int offset = rank * set_size;

    
	for (int i = 0; i < (offset+set_size) ; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// printf("thread num: %d\n", omp_get_num_threads());

			// printf("thread num: %d\n", omp_get_thread_num());
			int offset = 4 * width * i + 4 * j;
			int r = image[offset + 0]; // R
			rec(r, (offset + 0), new_image);
			int g = image[offset + 1]; // G
			rec(g, offset + 1, new_image);
			int b = image[offset + 2]; // B
			rec(b, offset + 2, new_image);
			int a = image[offset + 3]; // A
			new_image[offset + 3] = a;
			xyz = pow(sin(i + xyz), cos(xyz + j));
		}
	}
	
	//recieve all the results
	 int val = 0;
	 if (rank == 1) {
        // receive value of x
        MPI_Recv(&val, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
		 MPI_Send(&x, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
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
