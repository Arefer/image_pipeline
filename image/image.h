#ifndef IMAGE_PIPELINE_IMAGE_H
#define IMAGE_PIPELINE_IMAGE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Pixel;

typedef struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    Pixel** pixel_matrix;
} Image;

void load_image(Image *img, const char *fname);
void separate_channels(Image* img);
void join_channels(Image* img);
void save_image(const Image *img, const char *fname);
void free_image(Image *img);
#endif //IMAGE_PIPELINE_IMAGE_H
