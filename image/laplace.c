//
// Created by kevin on 8/29/20.
//


#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "image.h"
#include "image_filters.h"
#include "../utils/utils.h"

// Etapa en la que se termina de aplicar la convolucion 

int main(int argc, char *argv[]) {
    
    int image_type = atoi(argv[1]);
    int image_format = atoi(argv[2]);
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    int channels = atoi(argv[5]);
    int size = atoi(argv[6]);
    char* image_name = argv[7];
    char* c = argv[8];
    char* u = argv[9];
    char* n = argv[10];
    char* mask_path = argv[11];
    char* b = argv[12];

    int pipe_fds[2];
    pid_t pid;
    if (pipe(pipe_fds) == -1){
        printf("Error en syscall pipe\n");
        exit(-1);
    }

    // Leer desde stdin el arreglo data de la imagen
    uint8_t* data = malloc(sizeof(uint8_t)*size);
    int i = 0;
    while (i < size){
        read(STDIN_FILENO, &data[i], sizeof(uint8_t));
        i++;
    }
    Image* img = (Image*)malloc(sizeof(Image));
    img->type = image_type;
    img->format = image_format;
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->size = size;

    data_to_pixels(img, data);
    free(data);

    
    // Se carga la mascara con la direccion ingresada
    int mask_load_sucess = 0;
    Mask* mask = read_mask(mask_path, &mask_load_sucess);
    if (!mask_load_sucess){
        printf("Error al leer la mascara\n");
        return 1;
    }

    // Se aplica el filtro de laplace
    Image* laplace_image = laplace_filter(img,mask);

    if ((pid = fork()) == -1){
        printf("Error en syscall fork");
        exit(-1);
    }

    if (pid == 0){ // Hijo

        dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[1]);
        char str_image_type[10];
        sprintf(str_image_type, "%d", (int)laplace_image->type);
        char str_image_format[10];
        sprintf(str_image_format, "%d", (int)laplace_image->format);
        char str_width[10];
        sprintf(str_width, "%d", laplace_image->width);
        char str_height[10];
        sprintf(str_height, "%d", laplace_image->height);
        char str_channels[10];
        sprintf(str_channels, "%d", laplace_image->channels);
        char str_size[10];
        sprintf(str_size, "%zu", laplace_image->size);

        //Se pasa a la etapa de binarizacion
        execl("bin/binarize",
              "bin/binarize",
              str_image_type,
              str_image_format,
              str_width,
              str_height,
              str_channels,
              str_size,
              image_name,
              c,
              u,
              n,
              b,
              (char*)NULL);
              
        exit(0);
    }
    else{ // Padre
        //dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[0]);
        //char name[50];
        //sprintf(name, "%s_gray_scale.jpg", image_name);
        //save_image(gray_scaled_image, name);
        uint8_t* laplace_data = pixels_to_array(laplace_image);
        //printf("%d\n", size);
        for (size_t i = 0; i < size; i++){
            write(pipe_fds[1], &laplace_data[i], sizeof(uint8_t));
        }
        close(pipe_fds[1]);
        free(laplace_data);
    }
    wait(NULL);
    free_image(img);
    free(img);
    free_image(laplace_image);
    free(laplace_image);

    exit(0);
}