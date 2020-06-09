//
// Created by keviin on 05-06-20.
//

#ifndef IMAGE_PIPELINE_UTILS_H
#define IMAGE_PIPELINE_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../image/image_filters.h"

// Error utility macro
#define ON_ERROR_EXIT(cond, message) \
do { \
    if((cond)) { \
        printf("Error in function: %s at line %d\n", __func__, __LINE__); \
        perror((message)); \
        exit(1); \
    } \
} while(0)

// Check if a string "str" ends with a substring "ends"
static inline bool str_ends_in(const char *str, const char *ends) {
    char *pos = strrchr(str, '.');
    return !strcmp(pos, ends);
}

static inline int contains(char* arr, char c, int len){
    for (int i=0; i<len; i++){
        if (arr[i] == c) return 1;
    }
    return 0;
}

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

static void inline print_n_chars(char c, int n){
    for (int i=0; i<n; i++) printf("%c", c);
}
Mask* read_mask(const char *file_path, int* success);
#endif //IMAGE_PIPELINE_UTILS_H
