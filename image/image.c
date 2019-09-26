// https://github.com/sol-prog/tutorial_stb-image_library_examples

#include "image.h"
#include "../utils.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h"

void Image_load(Image *img, const char *fname) {
    if((img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL) {
        img->size = img->width * img->height * img->channels;
        img->allocation_ = STB_ALLOCATED;
    }
}

/**
 * Separa los canales de una imagen. Almacena cada canal en channels_data como una matriz.
 * @param img = estructura (Image) que representa a la imagen
 * No posee salidas
 */
void separate_channels(Image* img){
    img->channels_data = (uint8_t***)malloc(sizeof(uint8_t**)*img->channels);
    // Reservar memoria para cada canal
    for (int c=0; c<img->channels; c++){
        img->channels_data[c] = (uint8_t**)malloc(sizeof(uint8_t*)*img->height);
        for (int i=0; i<img->height; i++){
            img->channels_data[c][i] = (uint8_t*)malloc(sizeof(uint8_t)*img->width);
        }
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
        for (int c=0; c<img->channels; c++){
            img->channels_data[c][i][j] = img->data[k];
            k ++;
        }
        j++;
    }
}

void pooling(Image* img, int i, int j){
    // Separar las capas de la imagen si es que no lo estan
    if (img->channels_data == NULL) separate_channels(img);

    // Crear una matriz por cada capa
    uint8_t*** channels = (uint8_t***)malloc(sizeof(uint8_t**)*img->channels);
    for (int c = 0; c < img->channels; c++){
        channels[c] = (uint8_t**)malloc(sizeof(uint8_t*)*img->height);
        for (int row=0; row<img->height; row++){
            img->channels_data[c][row] = (uint8_t*)malloc(sizeof(uint8_t)*img->width);
        }
    }

    int done = 0;
    int row = 0;
    int column = 0;
    while (row+i < img->height && column+j < img->width){
        // Si llegue a la ultima columna posible, pasar a la siguiente fila
        if (column+j >= img->width) break;
    }
}

/**
 * Determina si una imagen es nearly black. Retorna 1 en caso positivo, 0 en caso negativo.
 * @param img
 * @param u
 * @return
 */
int nearlyBlack(Image* img, int u){
    // Separar las capas de la imagen si es que no lo estan
    if (img->channels_data == NULL) separate_channels(img);

    // Contar los pixeles negros (0,0,0) RGB
    int blackPixels = 0;
    int i = 0;
    while (i < img->size/img->channels){
        if (img->channels_data[0] == 0 && img->channels_data[1] == 0 && img->channels_data[2] == 0){
            blackPixels += 1;
        }
        i += 1;
    }

    return blackPixels > u ? 1 : 0;
}

void Image_create(Image *img, int width, int height, int channels, bool zeroed) {
    size_t size = width * height * channels;
    if(zeroed) {
        img->data = calloc(size, 1);
    } else {
        img->data = malloc(size);
    }

    if(img->data != NULL) {
        img->width = width;
        img->height = height;
        img->size = size;
        img->channels = channels;
        img->allocation_ = SELF_ALLOCATED;
    }
}

void Image_save(const Image *img, const char *fname) {
    // Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
    if(str_ends_in(fname, ".jpg") || str_ends_in(fname, ".JPG") || str_ends_in(fname, ".jpeg") || str_ends_in(fname, ".JPEG")) {
        stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
    } else if(str_ends_in(fname, ".png") || str_ends_in(fname, ".PNG")) {
        stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
    } else {
        ON_ERROR_EXIT(false, "");
    }
}

void Image_free(Image *img) {
    if(img->allocation_ != NO_ALLOCATION && img->data != NULL) {
        if(img->allocation_ == STB_ALLOCATED) {
            stbi_image_free(img->data);
        } else {
            free(img->data);
        }
        // Liberar la memoria de los canales
        if (img->channels_data != NULL){
            for (int c=0; c<img->channels; c++){
                for (int row=0; row<img->height; row++){
                    free(img->channels_data[c][row]);
                }
                free(img->channels_data[c]);
            }
        }
        free(img->channels_data);
        img->data = NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_ = NO_ALLOCATION;
    }
}

void Image_to_gray(const Image *orig, Image *gray) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "The input image must have at least 3 channels.");
    int channels = orig->channels == 4 ? 2 : 1;
    Image_create(gray, orig->width, orig->height, channels, false);
    ON_ERROR_EXIT(gray->data == NULL, "Error in creating the image");

    for(unsigned char *p = orig->data, *pg = gray->data; p != orig->data + orig->size; p += orig->channels, pg += gray->channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(orig->channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }
}

void Image_to_sepia(const Image *orig, Image *sepia) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "The input image must have at least 3 channels.");
    Image_create(sepia, orig->width, orig->height, orig->channels, false);
    ON_ERROR_EXIT(sepia->data == NULL, "Error in creating the image");

    // Sepia filter coefficients from https://stackoverflow.com/questions/1061093/how-is-a-sepia-tone-created
    for(unsigned char *p = orig->data, *pg = sepia->data; p != orig->data + orig->size; p += orig->channels, pg += sepia->channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue
        if(orig->channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }
}