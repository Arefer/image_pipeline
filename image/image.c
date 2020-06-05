// https://github.com/sol-prog/tutorial_stb-image_library_examples

#include "image.h"
#include "../utils.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image/stb_image_write.h"

void load_image(Image *img, const char *file_path) {
    uint8_t* data;
    if((data = stbi_load(file_path, &img->width, &img->height, &img->channels, 0)) != NULL) {
        img->size = img->width * img->height * img->channels;
        if(str_ends_in(file_path, ".jpg") || str_ends_in(file_path, ".JPG") || str_ends_in(file_path, ".jpeg") ||
           str_ends_in(file_path, ".JPEG")) {
            img->format = JPG;
            if (img->channels == 1) img->type = GRAY_SCALE;
            else img->type = RGB;
        } else if(str_ends_in(file_path, ".png") || str_ends_in(file_path, ".PNG")) {
            img->format = PNG;
            if (img->channels == 2) img->type = GRAY_SCALE;
            else img->type = RGB;
        }
        data_to_pixels(img, data);
        free(data);
    }
}

void set_pixel_to_zero(Pixel* p){
    p->gray = 0;
    p->red = 0;
    p->green = 0;
    p->blue = 0;
    p->alpha = 0;
}

uint8_t pixel_to_unsigned(int p){
    if (p < 0) return 0;
    if (p > 255) return 255;
    return (uint8_t)p;
}

Image* copy_image(Image* img){
    Image* new_image = (Image*)malloc(sizeof(Image));
    new_image->type = img->type;
    new_image->format = img->format;
    new_image->width = img->width;
    new_image->height = img->height;
    new_image->channels = img->channels;
    new_image->size = img->size;
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
void data_to_pixels(Image* img, const uint8_t* data){
    // Reservar memoria para cada pixel
    img->pixel_matrix = (Pixel**)malloc(sizeof(Pixel*)*img->height);
    for (int i=0; i<img->height; i++){
        img->pixel_matrix[i] = (Pixel*)malloc(sizeof(Pixel)*img->width);
        for (int j=0; j<img->width; j++){
            set_pixel_to_zero(&img->pixel_matrix[i][j]);
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
        Pixel* pixel = &img->pixel_matrix[i][j];
        if (img->type == RGB){
            pixel->red = data[k];
            pixel->green = data[k + 1];
            pixel->blue = data[k + 2];
            if (img->format == JPG){
                pixel->alpha = 0;
                k += 3;
            } else if (img->format == PNG) {
                pixel->alpha = data[k + 3];
                k += 4;
            }
        } else if (img->type == GRAY_SCALE){
            pixel->gray = data[k];
            pixel->red = 0;
            pixel->green = 0;
            pixel->blue = 0;
            if (img->format == JPG){
                pixel->alpha = 0;
                k++;
            } else if (img->format == PNG){
                pixel->alpha = data[k + 1];
                k += 2;
            }
        }
        j++;
    }
}
/**
 * Libera el areglo data de la Imagen y reemplaza sus valores por los presentes en pixel_matrix.
 * @param img : imagen a procesar.
 * Sin salida.
 */
uint8_t* pixels_to_array(Image* img){
    uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t)*img->size);

    int row_index = 0;
    int column_index = 0;

    int k = 0;
    while (k<img->size){
        if (column_index == img->width){
            column_index = 0;
            row_index++;
        }
        Pixel* pixel = &img->pixel_matrix[row_index][column_index];
        if (img->type == RGB){
            data[k] = pixel->red;
            data[k + 1] = pixel->green;
            data[k + 2] = pixel->blue;
            if (img->format == JPG){
                k += 3;
            } else if (img->format == PNG) {
                data[k + 3] = pixel->alpha;
                k += 4;
            }
        } else if (img->type == GRAY_SCALE){
            data[k] = pixel->gray;
            if (img->format == JPG){
                k++;
            } else if (img->format == PNG){
                data[k + 1] = pixel->alpha;
                k += 2;
            }
        }
        column_index++;
    }
    return data;
}

/**
 * Concluye si img es "nearly_black". Esto es: si el porcentaje de pixeles negros es mayor o igual a threshold, entonces
 * se concluye que la imagen es nearly_black, en caso contrario, no lo es.
 * @param img imagen sobre la cual concluir si es nearly_black.
 * @param threshold porcentaje de umbral, numero entre 0 y 100.
 * @return 1 si la img es nearly_black. 0 si no img NO es nearly_black. -1 si img no es del tipo GRAY_SCALE.
 */
int nearly_black(Image* img, double threshold){
    if (threshold < 0 || threshold > 100) return -1;
    if (img->type != GRAY_SCALE) return -1;
    int total = (int)img->size / img->channels;
    int black_pixels = 0;
    for (int i=0; i<img->height; i++){
        for (int j=0; j<img->width; j++){
            uint8_t pixel = img->pixel_matrix[i][j].gray;
            if (pixel == 0) black_pixels++;
        }
    }
    // calculo del porcentaje
    double black_pixels_percentage = ((double)black_pixels / total) * 100;
    return black_pixels_percentage >= threshold ? 1 : 0;
}

void save_image(Image *img, const char *file_path) {
    uint8_t* data = pixels_to_array(img);
    // Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
    if(img->format == JPG) {
        stbi_write_jpg(file_path, img->width, img->height, img->channels, data, 100);
    } else if(img->format == PNG) {
        stbi_write_png(file_path, img->width, img->height, img->channels, data, img->width * img->channels);
    } else {
        free(data);
        ON_ERROR_EXIT(false, "");
    }
    free(data);
}

void free_image(Image *img) {
    // Liberar la memoria de la matriz de pixeles
    if (img->pixel_matrix != NULL){
        for (int i=0; i<img->height; i++) {
            free(img->pixel_matrix[i]);
        }
        free(img->pixel_matrix);
    }
    img->width = 0;
    img->height = 0;
    img->size = 0;
}

