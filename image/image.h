#ifndef IMAGE_PIPELINE_IMAGE_H
#define IMAGE_PIPELINE_IMAGE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum Image_type{RGB, GRAY_SCALE};

enum Image_format{JPG, PNG};

typedef struct {
    uint8_t gray;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} Pixel;

typedef struct {
    enum Image_type type;
    enum Image_format format;
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    Pixel** pixel_matrix;
} Image;

void load_image(Image *img, const char *fname);
Image* copy_image(Image* img);
void separate_channels(Image* img);
void join_channels(Image* img);
void save_image(const Image *img, const char *fname);
void free_image(Image *img);
#endif //IMAGE_PIPELINE_IMAGE_H
