#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include "image_pipeline.h"
#include "image/image_filters.h"
#include "utils/utils.h"

// Inicio del pipeline donde se toman los argumentos, se inician variables
// y ademas se crea el primer hijo que leera la imagen

int main(int argc, char *argv[]){
    if (argc == 2 && (strcmp(argv[1], "-h") == 0)){
        print_usage();
        return 0;
    }
    int images, binarization_threshold;
    char str_images[10], str_binarization_threshold[10];
    double nearly_black_threshold;
    char str_nearly_black_threshold[10];

    char* mask_path = NULL;
    int show_result = 0;
    char str_show_result[10] = "";
    char img_path[100] = "imagen\0";
    if (!get_arguments(argc, argv, &images, &binarization_threshold, &nearly_black_threshold, &mask_path, &show_result)) {
        return 1;
    }
    sprintf(str_images, "%d", images);
    sprintf(str_binarization_threshold, "%d", binarization_threshold);
    sprintf(str_nearly_black_threshold, "%f", nearly_black_threshold);
    if (show_result) strcpy(str_show_result, "-b\0");
    if (images == 0){
        printf("Nada que procesar\n");
        return 2;
    }
    for (int i = 0; i < images; i++) {
        char img_path_i[200];
        sprintf(img_path_i, "%s_%d.jpg", img_path, i + 1);
        img_path_i[strlen(img_path_i)] = '\0';

        int pipe_fds[2];
        pid_t pid;
        if (pipe(pipe_fds) == -1) {
            printf("Error en syscall pipe\n");
            exit(-1);
        }

        if ((pid = fork()) == -1) {
            printf("Error en syscall fork");
            exit(-1);
        }
        if (pid == 0) { // Hijo
            dup2(pipe_fds[0], STDIN_FILENO);
            close(pipe_fds[1]);
            execl("bin/load_image",
                  "bin/load_image",
                  str_images,
                  str_binarization_threshold,
                  str_nearly_black_threshold,
                  mask_path,
                  str_show_result,
                  (char *) NULL);
        }
        write(pipe_fds[1], img_path_i, sizeof(char) * strlen(img_path_i));
        close(pipe_fds[1]);
        wait(NULL);

    }
    free(mask_path);
    return 0;
}
