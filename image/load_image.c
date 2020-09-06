//
// Created by kevin on 8/29/20.
//


#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "image.h"

// Etapa de lectura de pipeline

int main(int argc, char *argv[]) {
    char* c = argv[1];
    char* u = argv[2];
    char* n = argv[3];
    char* mask_path = argv[4];
    char* b = argv[5];

    // Se carga la imagen
    char img_path[50];
    scanf("%s", img_path);
    Image* img = (Image*)malloc(sizeof(Image));
    load_image(img, img_path);
    uint8_t* data = pixels_to_array(img);

    int pipe_fds[2];
    pid_t pid;
    if (pipe(pipe_fds) == -1){
        printf("Error en syscall pipe\n");
        exit(-1);
    }
     // Se crea el proceso hijo
    if ((pid = fork()) == -1){
        printf("Error en syscall fork");
        exit(-1);
    }

    if (pid == 0){ // Hijo
        dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[1]);
        char str_image_type[10];
        sprintf(str_image_type, "%d", (int)img->type);
        char str_image_format[10];
        sprintf(str_image_format, "%d", (int)img->format);
        char str_width[10];
        sprintf(str_width, "%d", img->width);
        char str_height[10];
        sprintf(str_height, "%d", img->height);
        char str_channels[10];
        sprintf(str_channels, "%d", img->channels);
        char str_size[10];
        sprintf(str_size, "%zu", img->size);

        // Se le da paso a la siguiente etapa de escala de grises
        execl("bin/gray_scale",
              "bin/gray_scale",
              str_image_type,
              str_image_format,
              str_width,
              str_height,
              str_channels,
              str_size,
              img_path,
              c,
              u,
              n,
              mask_path,
              b,
              (char*)NULL);
    }
    else{ // Padre
        //dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[0]);
        // Para escribir los datos de la imagen en el pipe lo que se hace es ir escribiendo uno a uno ls valores de la data de los pixeles en el pipe, esto es en todas las
        // etapas del pipeline
        for (int i=0; i<img->size; i++){
            write(pipe_fds[1], &data[i], sizeof(uint8_t));
        }
        close(pipe_fds[1]);
        wait(NULL);
    }

    free(data);
    free_image(img);
    free(img);
    return 0;
}