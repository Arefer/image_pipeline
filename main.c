#include <stdio.h>
#include "image/image.h"

int main() {
    Image* img = (Image*)malloc(sizeof(Image));
    Image_load(img, "Mozart.png");
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    separate_channels(img);
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img->width, img->height, img->channels);
    printf("Pixel 328,229: (%d, %d, %d)", img->channels_data[0][328][229], img->channels_data[1][328][229], img->channels_data[2][328][229]);
    Image_free(img);
    free(img);
    return 0;
}