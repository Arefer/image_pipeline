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
    // Retornar pixel negro si los indices estan fuera de la imagen. Indicar error en success
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

    int i_min, i_max, j_min, j_max;
    i_min = i - pixels_needed_above;
    i_max = i + pixels_needed_below;
    j_min = j - pixels_needed_left;
    j_max = j + pixels_needed_right;

    // Nuevo pixel para ir almacenando el resultado de las sumas
    SignedPixel new_pixel;
    new_pixel.gray = 0;
    new_pixel.red = 0;
    new_pixel.green = 0;
    new_pixel.blue = 0;
    new_pixel.alpha = 0;

    int mask_i = 0, img_i = i_min;
    int mask_j = 0, img_j = j_min;
    do {
        Pixel img_pixel;
        // Considerar el pixel desbordado como 0
        if (img_i < 0 || img_i >= img->height || img_j < 0 || img_j >= img->width) set_pixel_to_zero(&img_pixel);
        else img_pixel = img->pixel_matrix[img_i][img_j];

        // Multiplicar cada canal del pixel por la cantidad indicada en la mascara y sumarlo al valor actual
        new_pixel.gray = new_pixel.gray + img_pixel.gray * mask->matrix[mask_i][mask_j];
        new_pixel.red = new_pixel.red + img_pixel.red * mask->matrix[mask_i][mask_j];
        new_pixel.green = new_pixel.green + img_pixel.green * mask->matrix[mask_i][mask_j];
        new_pixel.blue = new_pixel.blue + img_pixel.blue * mask->matrix[mask_i][mask_j];

        mask_j++;
        img_j++;

        // Si se llega al final de una fila, pasar a la siguiente
        if (img_j == j_max + 1) {
            mask_i++;
            img_i++;
            mask_j = 0;
            img_j = j_min;
        }
    } while (img_i <= i_max);

    Pixel unsigned_pixel;
    unsigned_pixel.gray = pixel_to_unsigned(new_pixel.gray);
    unsigned_pixel.red = pixel_to_unsigned(new_pixel.red);
    unsigned_pixel.green = pixel_to_unsigned(new_pixel.green);
    unsigned_pixel.blue = pixel_to_unsigned(new_pixel.blue);
    unsigned_pixel.alpha = img->pixel_matrix[i][j].alpha;
    *success = 1;
    return unsigned_pixel;
}

/**
 * Crea una copia a img y realiza un filtro laplaciano.
 * Para ello itera sobre todos los pixeles de img y aplica la mascara (mask) alrededor de cada yuno de ellos.
 * @param img imagen a la que se le desea aplicar el filtro laplaciano
 * @param mask mascara de filtro laplaciano.
 * @return Puntero a una nueva imagen filtrada (Image*).
 */
Image* laplace_filter(Image* img, Mask* mask){
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
    return new_image;
}

/**
 * Crea una copia de img y aplica el filtro de binarizacion.
 * Esto es, para cada pixel de img: si pixel > threshold -> pixel = 255. En caso contrario, pixel = 0.
 * Acepta solamente imagenes en gray scale.
 * @param img imagen (Image*) a la cual se desea aplicar el filtro de binarizacion.
 * @param threshold Umbral de binarizacion.
 * @return Image* con la copia de img binarizada.
 */
Image* binarize_image(Image* img, uint8_t threshold){
    if (img->type != GRAY_SCALE) return NULL;
    Image* new_img = copy_image(img);
    for (int i=0; i<new_img->height; i++){
        for (int j=0; j<new_img->width; j++){
            uint8_t* pixel = &new_img->pixel_matrix[i][j].gray;
            *pixel = *pixel > threshold ? 255 : 0;
        }
    }
    return new_img;
}

/**
 * Libera la memoria de una mascara.
 * @param mask : mascara a liberar.
 */
void free_mask(Mask* mask){
    for (int i = 0; i < mask->rows; i++){
        free(mask->matrix[i]);
    }
    free(mask->matrix);
}