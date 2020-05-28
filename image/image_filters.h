//
// Created by keviin on 27-05-20.
//

#ifndef IMAGE_PIPELINE_IMAGE_FILTERS_H
#define IMAGE_PIPELINE_IMAGE_FILTERS_H
typedef struct {
    int rows;
    int columns;
    int h_center;
    int v_center;
    int** matrix;
} Mask;
Image* image_to_gray_scale(Image* img);
Pixel mask_image_pixel(Image* img, int i, int j, Mask* mask, int* success);
Image* laplacian_filter(Image* img, Mask* mask);
void free_mask(Mask* mask);
#endif //IMAGE_PIPELINE_IMAGE_FILTERS_H
