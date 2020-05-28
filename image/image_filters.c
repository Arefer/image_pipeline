//
// Created by keviin on 27-05-20.
//

#include "image.h"
#include "image_filters.h"

/**
 * Crea una copia de img y aplica el filtro escala de grises.
 * Se encarga de ajustar el numero de canales a 1 o 2 (dependiendo si es JPG o PNG).
 * @param img imagen a aplicar el filtro.
 * @return puntero a la imagen filtrada (Image*)
 */
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

/**
 * Dada una mascara y una posicion de un pixel, aplica la mascara alrededor de dicho pixel.
 * Cada canal del pixel es afectado de manera separada por la mascara.
 * @param img imagen que contiene al pixel.
 * @param i posicion de la fila del pixel (eje y).
 * @param j posicion de la columna del pixel (eje x).
 * @param mask mascara de valores.
 * @param success para saber si hubo exito o error. (1 = exito, 0 = error)
 * @return retorna un Pixel en caso exitoso o NULL si los parametros no son validos (i, j o mask fuera de bordes).
 */
Pixel mask_image_pixel(Image* img, int i, int j, Mask* mask, int* success){
    Pixel empty;
    set_pixel_to_zero(&empty);
    // Retornar NULL si los indices estan fuera de la imagen
    if (i < 0 || i >= img->height || j < 0 || j >= img->width) {
        *success = 0;
        return empty;
    }
    // Comprobacion de posiciones de fila y columna
    // Pixeles hacia la izquierda necesarios
    int pixels_needed_left = mask->h_center;
    // Pixeles hacia la derecha necesarios
    int pixels_needed_right = mask->columns - (mask->h_center + 1);
    // Pixeles hacia arriba necesarios
    int pixels_needed_above = mask->rows - (mask->v_center + 1);
    // Pixeles hacia abajo necesarios
    int pixels_needed_below = mask->v_center;

    int h_min, h_max, v_min, v_max;
    h_min = j - pixels_needed_left;
    h_max = j + pixels_needed_right;
    v_min = i - pixels_needed_above;
    v_max = i + pixels_needed_below;
    // No realizar la mascara si se sobrepasa de los limites posibles
    if (v_min < 0 || v_max >= img->height || h_min < 0 || h_max >= img->width) {
        *success = 0;
        return empty;
    }

    // Calcular el nuevo pixel en caso de que la mascara "quepa"
    SignedPixel new_pixel;
    new_pixel.gray = 0;
    new_pixel.red = 0;
    new_pixel.green = 0;
    new_pixel.blue = 0;
    new_pixel.alpha = 0;
    //set_pixel_to_zero(&new_pixel);
    for (int mask_i = 0, img_i = v_min, mask_j = 0, img_j = h_min; mask_i < mask->rows; mask_j++, img_j++){
        // Pasar a una nueva fila una vez se llegue al final de las columnas
        if (mask_j == mask->columns) {
            mask_i++;
            img_i++;
            mask_j = 0;
            img_j = h_min;
        }
        if (mask_i < mask->rows){
            // Multiplicar cada canal del pixel por la cantidad indicada en la mascara y sumarlo al valor actual
            new_pixel.gray = new_pixel.gray + img->pixel_matrix[img_i][img_j].gray * mask->matrix[mask_i][mask_j];
            new_pixel.red = new_pixel.red + img->pixel_matrix[img_i][img_j].red * mask->matrix[mask_i][mask_j];
            new_pixel.green = new_pixel.green + img->pixel_matrix[img_i][img_j].green * mask->matrix[mask_i][mask_j];
            new_pixel.blue = new_pixel.blue + img->pixel_matrix[img_i][img_j].blue * mask->matrix[mask_i][mask_j];
            new_pixel.alpha = new_pixel.alpha + img->pixel_matrix[img_i][img_j].alpha * mask->matrix[mask_i][mask_j];
        }
    }
    *success = 1;
    Pixel unsigned_pixel;
    unsigned_pixel.gray = pixel_to_unsigned(new_pixel.gray);
    unsigned_pixel.red = pixel_to_unsigned(new_pixel.red);
    unsigned_pixel.green = pixel_to_unsigned(new_pixel.green);
    unsigned_pixel.blue = pixel_to_unsigned(new_pixel.blue);
    unsigned_pixel.alpha = pixel_to_unsigned(new_pixel.alpha);
    return unsigned_pixel;
}

/**
 * Crea una copia a img y realiza un filtro laplaciano.
 * Para ello itera sobre todos los pixeles de img y aplica la mascara (mask) alrededor de cada yuno de ellos.
 * @param img imagen a la que se le desea aplicar el filtro laplaciano
 * @param mask mascara de filtro laplaciano.
 * @return Puntero a una nueva imagen filtrada (Image*).
 */
Image* laplacian_filter(Image* img, Mask* mask){
    Image* new_image = copy_image(img);
    int success = 0;
    for (int i = 0; i < img->height; i++){
        for (int j = 0; j < img->width; j++){
            Pixel masked_pixel = mask_image_pixel(img, i, j, mask, &success);
            if (success == 1){
                new_image->pixel_matrix[i][j] = masked_pixel;
            }
        }
    }
    join_channels(new_image);
    return new_image;
}

void free_mask(Mask* mask){
    for (int i = 0; i < mask->rows; i++){
        free(mask->matrix[i]);
    }
    free(mask->matrix);
}