#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>

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

int main() {
    int i, j;
    int image[WIDTH][HEIGHT];
    double real_min = -2.0, real_max = 1.0, imag_min = -1.0, imag_max = 1.0;
    double real_step = (real_max - real_min) / WIDTH;
    double imag_step = (imag_max - imag_min) / HEIGHT;

    clock_t start_time, end_time;
    double total_time;

    start_time = clock(); // Start timing

    for (i = 0; i < WIDTH; ++i) {
        for (j = 0; j < HEIGHT; ++j) {
            double real = real_min + i * real_step;
            double imag = imag_min + j * imag_step;
            image[i][j] = mandelbrot(real, imag);
        }
    }

    end_time = clock(); // End timing
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Mandelbrot computation took %.4f seconds.\n", total_time);

    // Save the Mandelbrot image as a PGM file
    savePGM("mandelbrot_sequential.pgm", image);
    printf("Mandelbrot image saved as 'mandelbrot_sequential.pgm'.\n");

    return 0;
}
