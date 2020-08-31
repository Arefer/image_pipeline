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


    Image* img = (Image*)malloc(sizeof(Image));
    img->type = image_type;
    img->format = image_format;
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->size = size;

    char dest_img_path[200];
    sprintf(dest_img_path, "%s_bin.jpg", image_name);

    save_image(img, dest_img_path);

    if(strcmp("-b",b) == 0){
      print_pipe_results(argv[0],argv[1]);  
    } 
    return 0;
}