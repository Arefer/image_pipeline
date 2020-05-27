//
// Created by keviin on 27-05-20.
//

#include "image.h"
#include "image_filters.h"

Image* image_to_gray_scale(Image* img){
    if (img->type == GRAY_SCALE) return copy_image(img);
    Image* new_image = copy_image(img);
    new_image->channels = img->format == JPG ? 1 : 2;
    for (int i=0; i<new_image->height; i++){
        for (int j=0; j<new_image->width; j++){
            Pixel* p = &new_image->pixel_matrix[i][j];
            p->gray = (p->red)*0.3 + (p->green)*0.59 + (p->blue)*0.11;
        }
    }
    new_image->size = new_image->height * new_image->width * new_image->channels;
    new_image->type = GRAY_SCALE;
    join_channels(new_image);
    return new_image;
}