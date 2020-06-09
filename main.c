#include <stdio.h>
#include <stdlib.h>
#include "image_pipeline.h"
#include "image/image_filters.h"
#include "utils/utils.h"

int main(int argc, char *argv[]){
    if (argc == 2 && (strcmp(argv[1], "-h") == 0)){
        print_usage();
        return 0;
    }
    int images, binarization_threshold;
    double nearly_black_threshold;
    char* mask_path = NULL;
    int show_result = 0;
    char img_path[100] = "imagen\0";
    if (!get_arguments(argc, argv, &images, &binarization_threshold, &nearly_black_threshold, &mask_path, &show_result)) {
        return 1;
    }
    int mask_load_sucess = 0;
    Mask* mask = read_mask(mask_path, &mask_load_sucess);
    if (!mask_load_sucess){
        printf("Error al leer la mascara\n");
        return 1;
    }
    if (images == 0){
        printf("Nada que procesar\n");
        return 2;
    } else {
        Result* results = (Result*)malloc(sizeof(Result)*images);
        for (int i=0; i<images; i++){
            char img_path_i[200];
            char dest_img_path_i[200];
            sprintf(img_path_i, "%s_%d.jpeg", img_path, i+1);
            img_path_i[strlen(img_path_i)] = '\0';
            results[i].name = (char*)malloc(sizeof(char)*(strlen(img_path_i)+1));
            sprintf(dest_img_path_i, "%s_%d_bin.jpeg", img_path, i+1);
            strcpy(results[i].name, img_path_i);
            int r = pipeline(binarization_threshold, nearly_black_threshold, img_path_i, mask,
                             dest_img_path_i);
            if (r) results[i].classification = NEARLY_BLACK;
            else results[i].classification = NOT_NEARLY_BLACK;
        }
        if (show_result) print_results(results, images);
        for (int i=0; i<images; i++){
            free(results[i].name);
        }
        free(results);
    }
    free_mask(mask);
    free(mask);
    free(mask_path);
    return 0;
}
