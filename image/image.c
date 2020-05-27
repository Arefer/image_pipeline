// https://github.com/sol-prog/tutorial_stb-image_library_examples

#include "image.h"
#include "../utils.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h"

void load_image(Image *img, const char *fname) {
    if((img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL) {
        img->size = img->width * img->height * img->channels;
    }
}

Image* copy_image(Image* img){
    Image* new_image = (Image*)malloc(sizeof(Image));
    new_image->width = img->width;
    new_image->height = img->height;
    new_image->channels = img->channels;
    new_image->size = img->size;
    new_image->data = (uint8_t*)malloc(sizeof(uint8_t)*new_image->size);
    new_image->pixel_matrix = (Pixel**)malloc(sizeof(Pixel*)*new_image->height);
    for (int i=0; i<new_image->height; i++){
        new_image->pixel_matrix[i] = (Pixel*)malloc(sizeof(Pixel)*new_image->width);
        for (int j=0; j<new_image->width; j++){
            new_image->pixel_matrix[i][j] = img->pixel_matrix[i][j];
        }
    }
    return new_image;
}
/**
 * Separa los canales de una imagen. Guarda los datos con una matriz de pixeles.
 * @param img = estructura (Image) que representa a la imagen
 * Sin salida
 */
void separate_channels(Image* img){
    // Reservar memoria para cada pixel
    img->pixel_matrix = (Pixel**)malloc(sizeof(Pixel*)*img->height);
    for (int i=0; i<img->height; i++){
        img->pixel_matrix[i] = (Pixel*)malloc(sizeof(Pixel)*img->width);
    }

    int i = 0;
    int j = 0;
    int k = 0;

    while (k < img->size){
        // Si se recorre la fila completa, pasar a la siguiente
        if (j == img->width){
            j = 0;
            i += 1;
        }
        // Iterar sobre los canales del pixel actual
        Pixel* pixel = &img->pixel_matrix[i][j];
        pixel->r = img->data[k];
        pixel->g = img->data[k+1];
        pixel->b = img->data[k+2];
        k+=3;

        j++;
    }
}
/**
 * Libera el areglo data de la Imagen y reemplaza sus valores por los presentes en pixel_matrix.
 * @param img : imagen a procesar.
 * Sin salida.
 */
void join_channels(Image* img){
    free(img->data);
    img->data = (uint8_t*)malloc(sizeof(uint8_t)*img->size);

    int row_index = 0;
    int column_index = 0;

    int value = 0;
    while (value<img->size){
        if (column_index == img->width){
            column_index = 0;
            row_index++;
        }
        Pixel* pixel = &img->pixel_matrix[row_index][column_index];
        img->data[value] = pixel->r;
        img->data[value+1] = pixel->g;
        img->data[value+2] = pixel->b;
        value += 3;

        column_index++;
    }
}

void save_image(const Image *img, const char *fname) {
    // Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
    if(str_ends_in(fname, ".jpg") || str_ends_in(fname, ".JPG") || str_ends_in(fname, ".jpeg") || str_ends_in(fname, ".JPEG")) {
        stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
    } else if(str_ends_in(fname, ".png") || str_ends_in(fname, ".PNG")) {
        stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
    } else {
        ON_ERROR_EXIT(false, "");
    }
}

void free_image(Image *img) {
    if (img->data != NULL) free(img->data);
    // Liberar la memoria de la matriz de pixeles
    if (img->pixel_matrix != NULL){
        for (int i=0; i<img->height; i++) {
            free(img->pixel_matrix[i]);
        }
        free(img->pixel_matrix);
    }
    img->data = NULL;
    img->width = 0;
    img->height = 0;
    img->size = 0;
}

