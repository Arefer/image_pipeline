#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "image.h"
#include "image_filters.h"

#define READ 0
#define WRITE 1
#define STDOUT 1


// Etapa de binarizacion de la imagen

int main(int argc, char *argv[])
{
    int size = atoi(argv[6]);
    int image_type = atoi(argv[1]);
    int image_format = atoi(argv[2]);
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    int channels = atoi(argv[5]);
    char* image_name = argv[7];
    char* c = argv[8];
    char* u = argv[9];
    char* n = argv[10];
    //char* mask_path = argv[11];
    char* b = argv[11];

    pid_t pid;
    int status;
    int fd[2];
    if(pipe(fd) < 0)
    {
        fprintf(stderr,"Error en syscall pipe\n");
        exit(-1);
    }

    // Se leen los datos de la imagen desde STDIN

    uint8_t* data = malloc(sizeof(uint8_t)*size);
    int i = 0;
    while (i < size){
        read(STDIN_FILENO, &data[i], sizeof(uint8_t));
        i++;
    }

    // Se formatea la imagen

    Image* img = (Image*)malloc(sizeof(Image));
    img->type = image_type;
    img->format = image_format;
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->size = size;

    data_to_pixels(img, data);
    free(data);

    // Se realiza la binarizacion

    Image* binarized_img = binarize_image(img, atoi(u));
    if (binarized_img == NULL) {
    printf("Error al binarizar imagen '%s'. Posiblemente el umbral sea incorrecto\n", image_name);
    exit(-1);
    }


    pid = fork();
    if(pid > 0) // Padre escribe los resultados de la binarizacion
    {
        close(fd[READ]);
        uint8_t* binarized_data = pixels_to_array(binarized_img);
        for (size_t i = 0; i < size; i++){
            write(fd[WRITE], &binarized_data[i], sizeof(uint8_t));
        }
        close(fd[1]);
        free(binarized_data);
    }
    else if(pid == 0) // Hijo se hace el nuevo proceso 
    {
        uint8_t* reading = (uint8_t*)malloc(sizeof(uint8_t)*size);
        close(fd[WRITE]);
        dup2(fd[READ],STDIN_FILENO);

        char str_image_type[10];
        sprintf(str_image_type, "%d", (int)binarized_img->type);
        char str_image_format[10];
        sprintf(str_image_format, "%d", (int)binarized_img->format);
        char str_width[10];
        sprintf(str_width, "%d", binarized_img->width);
        char str_height[10];
        sprintf(str_height, "%d", binarized_img->height);
        char str_channels[10];
        sprintf(str_channels, "%d", binarized_img->channels);
        char str_size[10];
        sprintf(str_size, "%zu", binarized_img->size);

        execl("bin/nearly_black",
              "bin/nearly_black",
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

        char name[50];
        sprintf(name, "%s_binarized.jpg", image_name);
        save_image(binarized_img, name);
        fprintf(stderr,"Fallo execl\n");
        exit(0);
    }
    waitpid(pid, &status,0);
    free_image(img);
    free(img);
    free_image(binarized_img);
    free(binarized_img);

    return 0;
}