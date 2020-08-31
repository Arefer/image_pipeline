#ifndef IMAGE_PIPELINE_IMAGE_PIPELINE_H
#define IMAGE_PIPELINE_IMAGE_PIPELINE_H
#include "image/image_filters.h"
enum Image_classification {NEARLY_BLACK, NOT_NEARLY_BLACK};

typedef struct {
    char* name;
    enum Image_classification classification;
} Result;

void print_usage();
int get_arguments(int argc, char **argv, int* number_of_images, int* binarization_threshold,
        double* nearly_black_threshold, char** mask_path, int* verbose);
int pipeline(int binarization_threshold, double nearly_black_threshold, char* img_path, Mask* mask, char* dest_img_path);
void print_results(Result* results, int res_size);
void print_pipe_results(char* img_path, char* classification);

#endif //IMAGE_PIPELINE_IMAGE_PIPELINE_H
