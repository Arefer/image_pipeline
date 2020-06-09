#include "utils.h"
Mask* read_mask(const char *file_path, int* success) {
    FILE* f = fopen(file_path, "r");
    if (f == NULL) return 0;
    Mask* mask = (Mask*)malloc(sizeof(Mask));
    mask->rows = 3;
    mask->columns = 3;
    mask->h_center = 1;
    mask->v_center = 1;
    mask->matrix = (int**)malloc(sizeof(int*)*3);
    for (int i=0; i<3; i++) mask->matrix[i] = (int*)malloc(sizeof(int)*3);
    int count;
    count = fscanf(f, "%d %d %d\n%d %d %d\n%d %d %d\n",
                   &mask->matrix[0][0], &mask->matrix[0][1], &mask->matrix[0][2],
                   &mask->matrix[1][0], &mask->matrix[1][1], &mask->matrix[1][2],
                   &mask->matrix[2][0], &mask->matrix[2][1], &mask->matrix[2][2]);
    if (count == EOF) {
        if (ferror(f)) {
            perror("fscanf");
            free(mask);
            fclose(f);
            *success = 0;
            return NULL;
        }
        else {
            fprintf(stderr, "Error: fscanf reached end of file, no matching characters, no matching failure\n");
            free(mask);
            fclose(f);
            *success = 0;
            return NULL;
        }
    }
    else if (count != 9) {
        fprintf(stderr, "Error: fscanf successfully matched and assigned %i input items, 6 expected\n", count);
        free(mask);
        fclose(f);
        *success = 0;
        return NULL;
    }
    fclose(f);
    *success = 1;
    return mask;
}