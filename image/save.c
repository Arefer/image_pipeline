#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include "../utils/utils.h"
#include "image.h"
#include "image_filters.h"
#include "../image_pipeline.h"

#define READ 0
#define WRITE 1
#define STDOUT 1

// Etapa de escritura de resultados

int main(int argc, char *argv[])
{
    int size = atoi(argv[6]);
    int image_type = atoi(argv[1]);
    int image_format = atoi(argv[2]);
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    int channels = atoi(argv[5]);
    char* image_name = argv[7];
    int nb = atoi(argv[8]);
    char* b = argv[9];

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


    char dest_img_path[200];
    sprintf(dest_img_path, "%s_bin.jpg", image_name);



    // Se escriben los resultados del proceso
    save_image(img, dest_img_path);

    // En caso de que se levante la bandera de display de resultados
    if(strcmp("-b",b) == 0){
      print_pipe_results(image_name,nb);  
    } 

    free_image(img);
    free(img);

    return 0;
}