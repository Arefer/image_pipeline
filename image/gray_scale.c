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

int main(int argc, char *argv[]) {
    int image_type = atoi(argv[1]);
    int image_format = atoi(argv[2]);
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    int channels = atoi(argv[5]);
    int size = atoi(argv[6]);
    char* image_name = argv[7];
    int c = atoi(argv[8]);
    int u = atoi(argv[9]);
    int n = atoi(argv[10]);
    char* mask_path = argv[11];
    int b = atoi(argv[12]);

    int pipe_fds[2];
    pid_t pid;
    if (pipe(pipe_fds) == -1){
        printf("Error en syscall pipe\n");
        exit(-1);
    }

    if ((pid = fork()) == -1){
        printf("Error en syscall fork");
        exit(-1);
    }

    if (pid == 0){ // Hijo
        uint8_t* reading = (uint8_t*)malloc(sizeof(uint8_t)*size);
        //dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[1]);
        int i=0;
        while (i < size){
            read(pipe_fds[0], &reading[i], sizeof(uint8_t));
            //printf("i=%d . Lei '%u' desde mi padre %d\n", i, reading[i], getppid());
            i++;
            //printf("%d\n", size);
        }
        free(reading);
        exit(0);
    }
    else{ // Padre
        //dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[0]);
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
        Image* gray_scaled_image = image_to_gray_scale(img);
        char name[50];
        sprintf(name, "%s_gray_scale.jpg", image_name);
        save_image(gray_scaled_image, name);
        uint8_t* gray_scaled_data = pixels_to_array(gray_scaled_image);
        printf("%d\n", size);
        for (size_t i = 0; i < size; i++){
            write(pipe_fds[1], &gray_scaled_data[i], sizeof(uint8_t));
        }
        close(pipe_fds[1]);
        wait(NULL);
        free_image(img);
        free(img);
        free_image(gray_scaled_image);
        free(gray_scaled_image);
        free(gray_scaled_data);
    }

    exit(0);
}