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
    int gray;
    int red;
    int green;
    int blue;
    int alpha;
} SignedPixel;

typedef struct {
    enum Image_type type;
    enum Image_format format;
    int width;
    int height;
    int channels;
    size_t size;
    char* name;
    Pixel** pixel_matrix;
} Image;

void load_image(Image *img, const char* file_path);
void set_pixel_to_zero(Pixel* p);
uint8_t pixel_to_unsigned(int p);
Image* copy_image(Image* img);
void data_to_pixels(Image* img, const uint8_t* data);
uint8_t * pixels_to_array(Image* img);
int nearly_black(Image* img, double threshold);
void save_image(Image *img, const char* file_path);
void free_image(Image *img);
#endif //IMAGE_PIPELINE_IMAGE_H
