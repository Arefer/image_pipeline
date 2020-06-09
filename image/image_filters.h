#ifndef IMAGE_PIPELINE_IMAGE_FILTERS_H
#define IMAGE_PIPELINE_IMAGE_FILTERS_H
#include "image.h"
typedef struct {
    int rows;
    int columns;
    int h_center;
    int v_center;
    int** matrix;
} Mask;
Image* image_to_gray_scale(Image* img);
Pixel mask_image_pixel(Image* img, int i, int j, Mask* mask, int* success);
Image* laplace_filter(Image* img, Mask* mask);
Image* binarize_image(Image* img, int threshold);
void free_mask(Mask* mask);
#endif //IMAGE_PIPELINE_IMAGE_FILTERS_H
