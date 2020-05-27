#include <stdio.h>
#include "image/image.h"
#include "image/image_filters.h"

int main(int argc, char *argv[]) {
    char* path = argv[1];
    Image* img = (Image*)malloc(sizeof(Image));
    load_image(img, path);
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img->width, img->height, img->channels);
    printf("Format: %d\n", img->format);
    printf("Type: %d\n", img->type);
    separate_channels(img);
    Pixel* p1 = &img->pixel_matrix[328][229];
    printf("Pixel 328,229: (%d, %d, %d)", p1->red, p1->green, p1->blue);

    Image* gray_scale_img = image_to_gray_scale(img);
    save_image(gray_scale_img, "gray_scale_wallpaper.jpg");

    for (int row_index = 0 ; row_index < img->height ; row_index++){
        for (int column_index = 0 ; column_index < img->width ; column_index++){
            Pixel* p2 = &img->pixel_matrix[row_index][column_index];
            p2->red = 0;
        }
    }
    join_channels(img);
    save_image(img, "Sin_r.jpg");
    free_image(gray_scale_img);
    free(gray_scale_img);
    free_image(img);
    free(img);
    return 0;
}