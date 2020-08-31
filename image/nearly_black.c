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
    int c = atoi(argv[8]);
    int u = atoi(argv[9]);
    int n = atoi(argv[10]);
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

    // Se realiza la clasificacion

    int nb = nearly_black(img, (double)n);
    

    pid = fork();
    if(pid > 0) // Padre escribe los resultados de la binarizacion
    {
        close(fd[READ]);
        uint8_t* data = pixels_to_array(img);
        for (size_t i = 0; i < size; i++){
            write(fd[WRITE], &data[i], sizeof(uint8_t));
        }
        close(fd[1]);
        free(data);
    }
    else if(pid == 0) // Hijo se hace el nuevo proceso 
    {
        uint8_t* reading = (uint8_t*)malloc(sizeof(uint8_t)*size);
        close(fd[WRITE]);
        dup2(fd[READ],STDIN_FILENO);

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

        execl("bin/save",
              "bin/save",
              str_image_type,
              str_image_format,
              str_width,
              str_height,
              str_channels,
              str_size,
              image_name,
              nb,
              b,
              (char*)NULL);
    
        fprintf(stderr,"Fallo execl\n");
        exit(0);
    }
    waitpid(pid, &status,0);
    free_image(img);
    free(img);

    return 0;
}