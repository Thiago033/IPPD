#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 1000

typedef struct {
    unsigned char r, g, b;
} Pixel;

void write_bmp(const char *filename, Pixel *img, int width, int height) {
    FILE *f;
    int filesize = 54 + 3 * width * height;
    unsigned char bmpfileheader[14] = {
        'B', 'M',  // Magic number
        0, 0, 0, 0,  // File size
        0, 0,  // Reserved
        0, 0,  // Reserved
        54, 0, 0, 0  // Data offset
    };
    unsigned char bmpinfoheader[40] = {
        40, 0, 0, 0,  // Header size
        0, 0, 0, 0,  // Width
        0, 0, 0, 0,  // Height
        1, 0,  // Planes
        24, 0,  // Bits per pixel
        0, 0, 0, 0,  // Compression
        0, 0, 0, 0,  // Image size
        0, 0, 0, 0,  // X pixels per meter
        0, 0, 0, 0,  // Y pixels per meter
        0, 0, 0, 0,  // Total colors
        0, 0, 0, 0   // Important colors
    };
    
    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);
    
    bmpinfoheader[4] = (unsigned char)(width);
    bmpinfoheader[5] = (unsigned char)(width >> 8);
    bmpinfoheader[6] = (unsigned char)(width >> 16);
    bmpinfoheader[7] = (unsigned char)(width >> 24);
    
    bmpinfoheader[8] = (unsigned char)(height);
    bmpinfoheader[9] = (unsigned char)(height >> 8);
    bmpinfoheader[10] = (unsigned char)(height >> 16);
    bmpinfoheader[11] = (unsigned char)(height >> 24);
    
    f = fopen(filename, "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Pixel p = img[(height - i - 1) * width + j];
            unsigned char color[3] = {p.b, p.g, p.r};
            fwrite(color, 1, 3, f);
        }
    }
    
    fclose(f);
}

void mandelbrot(Pixel *img, int width, int height, int max_iter, int start_row, int end_row) {
    double x0, y0, x, y, xtemp;
    int iteration;
    for (int py = start_row; py < end_row; py++) {
        for (int px = 0; px < width; px++) {
            x0 = (double)px / width * 3.5 - 2.5;
            y0 = (double)py / height * 2.0 - 1.0;
            x = 0.0;
            y = 0.0;
            iteration = 0;
            while (x*x + y*y <= 4 && iteration < max_iter) {
                xtemp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtemp;
                iteration++;
            }
            Pixel *p = &img[py * width + px];
            if (iteration == max_iter) {
                p->r = 0;
                p->g = 0;
                p->b = 0;
            } else {
                p->r = (iteration % 256);
                p->g = (iteration % 256);
                p->b = (iteration % 256);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int width = WIDTH;
    int height = HEIGHT;
    int max_iter = MAX_ITER;

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Pixel *img = NULL;
    if (rank == 0) {
        img = (Pixel *)malloc(width * height * sizeof(Pixel));
        if (img == NULL) {
            fprintf(stderr, "Error allocating memory\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    int rows_per_proc = height / size;
    Pixel *local_img = (Pixel *)malloc(width * rows_per_proc * sizeof(Pixel));
    if (local_img == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int start_row = rank * rows_per_proc;
    int end_row = (rank + 1) * rows_per_proc;
    mandelbrot(local_img, width, height, max_iter, start_row, end_row);

    MPI_Gather(local_img, width * rows_per_proc * sizeof(Pixel), MPI_BYTE,
               img, width * rows_per_proc * sizeof(Pixel), MPI_BYTE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        write_bmp("mandelbrot.bmp", img, width, height);
        free(img);
    }

    free(local_img);
    MPI_Finalize();
    return 0;
}
