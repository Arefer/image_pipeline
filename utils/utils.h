#ifndef IMAGE_PIPELINE_UTILS_H
#define IMAGE_PIPELINE_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../image/image_filters.h"

/**
 * Comprueba si str termina con ends.
 * @param str : string a comprobar.
 * @param ends : sufijo a comprobar.
 * @return : bool.
 */
static inline bool str_ends_in(const char *str, const char *ends) {
    char *pos = strrchr(str, '.');
    return !strcmp(pos, ends);
}

/**
 * COmprueba si el arreglo arr contiene el caracter c.
 * @param arr : Arreglo a comprobar.
 * @param c : Caracter a comprobar si es contenido por arr.
 * @param len : Largo del arreglo.
 * @return 1 en caso de extio. 0 en caso contrario.
 */
static inline int contains(char* arr, char c, int len){
    for (int i=0; i<len; i++){
        if (arr[i] == c) return 1;
    }
    return 0;
}

/**
 * Provee los margenes necesarios para que un string quede centrado en un espacio de size caracteres.
 * Por ejemplo si tenemos el string "test" y queremos centrarla en un espacio de 6 caracteres, entonces
 * left_margin = right_margin = 1. De este modo aseguramos que "test" esta centrada -> " test ".
 * @param s : String a centrar.
 * @param size : Espacio de caracteres donde centrar s.
 * @param left_margin : Cantidad de espacios que quedan a la izquierda del string s centrado.
 * @param right_margin : Cantidad de espacios que quedan a la derecha del string s centrado.
 * @return 1 en caso de exito. 0 en caso contrario.
 */
static inline int string_center(char* s, int size, int* left_margin, int* right_margin){
    if (size < strlen(s)) return -1;
    if (size == strlen(s)) {
        *left_margin = 0;
        *right_margin = 0;
    };
    int string_size = strlen(s);
    *left_margin = 0;
    *right_margin = size - string_size;
    for (int i=0; i+string_size <= size; i++){
        if ((*right_margin) - (*left_margin) <= 1){
            return 1;
        }
        (*left_margin)++;
        (*right_margin)--;
    }
    return -1;
}

/**
 * Imprime n caracteres c.
 * @param c : caracter a imprimir n veces.
 * @param n : veces que se desea imprimir c.
 * Sin salida.
 */
static void inline print_n_chars(char c, int n){
    for (int i=0; i<n; i++) printf("%c", c);
}
Mask* read_mask(const char *file_path, int* success);
#endif //IMAGE_PIPELINE_UTILS_H
