#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <complex.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITER 1000

int mandelbrot(double real, double imag) {
    int n;
    double complex z = real + imag * I;
    double complex c = z;

    for (n = 0; n < MAX_ITER; ++n) {
        if (cabs(z) > 2.0)
            return n;
        z = z * z + c;
    }

    return n;
}

void savePGM(const char *filename, int image[WIDTH][HEIGHT]) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open the file for writing");
        exit(1);
    }

    fprintf(file, "P5\n%d %d\n%d\n", WIDTH, HEIGHT, MAX_ITER - 1);

    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            fputc((unsigned char)(image[i][j] * 255 / (MAX_ITER - 1)), file);
        }
    }

    fclose(file);
}

int main(int argc, char** argv) {
    int rank, size;
    int i, j;
    int image[WIDTH][HEIGHT];
    double real_min = -2.0, real_max = 1.0, imag_min = -1.0, imag_max = 1.0;
    double real_step = (real_max - real_min) / WIDTH;
    double imag_step = (imag_max - imag_min) / HEIGHT;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t start_time, end_time;
    double total_time;

    if (rank == 0) {
        start_time = clock();
    }

    int chunk_size = HEIGHT / size;
    int start_row = rank * chunk_size;
    int end_row = (rank == size - 1) ? HEIGHT : start_row + chunk_size;

    for (i = 0; i < WIDTH; ++i) {
        for (j = start_row; j < end_row; ++j) {
            double real = real_min + i * real_step;
            double imag = imag_min + j * imag_step;
            image[i][j] = mandelbrot(real, imag);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = clock();
        total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

        printf("Mandelbrot computation took %.4f seconds.\n", total_time);

        // Save the Mandelbrot image as a PGM file
        savePGM("mandelbrot.pgm", image);
        printf("Mandelbrot image saved as 'mandelbrot.pgm'.\n");
    }

    MPI_Finalize();

    return 0;
}
