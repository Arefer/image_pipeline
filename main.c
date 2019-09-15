#include <stdio.h>
#include "image/image.h"

int main() {
    Image* img = (Image*)malloc(sizeof(Image));
    Image_load(img, "resources/Mozart.png");
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img->width, img->height, img->channels);

    Image_free(img);
    free(img);
}