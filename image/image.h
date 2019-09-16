// https://github.com/sol-prog/tutorial_stb-image_library_examples

#ifndef IMAGE_PIPELINE_IMAGE_H
#define IMAGE_PIPELINE_IMAGE_H
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum allocation_type {
    NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

typedef struct {
    int width;
    int height;
    int channels;
    size_t size;
    uint8_t *data;
    uint8_t ***channels_data;
    enum allocation_type allocation_;
} Image;

void Image_load(Image *img, const char *fname);
void Image_create(Image *img, int width, int height, int channels, bool zeroed);
void separate_channels(Image* img);
void Image_save(const Image *img, const char *fname);
void Image_free(Image *img);
void Image_to_gray(const Image *orig, Image *gray);
void Image_to_sepia(const Image *orig, Image *sepia);
#endif //IMAGE_PIPELINE_IMAGE_H
