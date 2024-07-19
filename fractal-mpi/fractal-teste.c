#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800

typedef struct {
    unsigned char r, g, b;
} Pixel;

void write_bmp(const char *filename, Pixel *img, int width, int height) {
    FILE *f;

    int filesize = 54 + 3 * width * height;  // w is your image width, h is image height, both int

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
    
    int filesize_offset = filesize;
    bmpfileheader[2] = (unsigned char)(filesize_offset);
    bmpfileheader[3] = (unsigned char)(filesize_offset >> 8);
    bmpfileheader[4] = (unsigned char)(filesize_offset >> 16);
    bmpfileheader[5] = (unsigned char)(filesize_offset >> 24);
    
    int width_offset = width;
    bmpinfoheader[4] = (unsigned char)(width_offset);
    bmpinfoheader[5] = (unsigned char)(width_offset >> 8);
    bmpinfoheader[6] = (unsigned char)(width_offset >> 16);
    bmpinfoheader[7] = (unsigned char)(width_offset >> 24);
    
    int height_offset = height;
    bmpinfoheader[8] = (unsigned char)(height_offset);
    bmpinfoheader[9] = (unsigned char)(height_offset >> 8);
    bmpinfoheader[10] = (unsigned char)(height_offset >> 16);
    bmpinfoheader[11] = (unsigned char)(height_offset >> 24);
    
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

void mandelbrot(Pixel *img, int width, int height, int max_iter) {
    double x0, y0, x, y, xtemp;
    int iteration;

    for (int px = 0; px < width; px++) {
        for (int py = 0; py < height; py++) {
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
    int max_iter = 1000;
    
    Pixel *img = (Pixel *)malloc(width * height * sizeof(Pixel));
    
    if (img == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }
    
    mandelbrot(img, width, height, max_iter);
    write_bmp("mandelbrot.bmp", img, width, height);
    
    free(img);
    return 0;
}