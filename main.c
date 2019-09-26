#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <wait.h>
#include "image/image.h"

#define READ 0
#define WRITE 1

// Proceso Main en el cual se leen las imagenes y se comienza a despachar cada una de las imagenes que se leeran en el
// pipeline




int main(int argc,char* argv[]) {
    int cantidadImagenes;
    char* nombreFiltro;
    char* umbral;
    char resultados[2] = "no";
    extern char *optarg;
    int c;
    while((c = getopt(argc,argv,"c:m:n:b")) != -1)
    {
        switch (c)
        {
        case 'c' /* constant-expression */:
            /* code */
            cantidadImagenes = atoi(optarg);
            break;
        case 'm':
            nombreFiltro = optarg;
            break;
        case 'n':
            umbral = optarg;
        case 'b':
            resultados[0] = 's';
            resultados[1] = 'i';
            break;                    
        default:
            abort();
            break;
        }
    }
    printf(" cantidad %d\n",cantidadImagenes);
    int piped[2], pipeI[2];        
    int i = pipe(piped);
    int j = pipe(pipeI);    
    pid_t pid = fork();
    int parentPid = getpid();       
    if(pid == 0)
    {
        // HIJO       
        wait(&parentPid);
        close(piped[WRITE]);        
        close(pipeI[WRITE]);                             
        int* args = (int*)malloc(sizeof(int) * 3);                                       
        dup2(piped[READ],STDIN_FILENO);                                
        read(pipeI[READ],args,sizeof(int)*3); 
        char channelString[2], heigthString[4], widthString[4];
        sprintf(channelString,"%d",args[0]);
        sprintf(heigthString,"%d",args[1]);
        sprintf(widthString,"%d",args[2]);            
        char* buf[7] = {"./convolution",channelString ,heigthString,widthString,umbral,resultados,NULL};                                                           
        execv("./convolution",buf);
        perror("fallo execv\n");
        exit(-1);
    }  
    else
    {                
        Image* img = (Image*)malloc(sizeof(Image));
        close(piped[READ]);
        close(pipeI[READ]);        
        Image_load(img, "resources/Perro-obeso.png");
        printf("Loaded image with a width of %dpx, a height of %dpx and %d channels and %ld size\n", img->width, img->height, img->channels, img->size);        
        if(img == NULL) 
        {
            printf("Error in loading the image\n");
            exit(1);
        }  
        write(piped[WRITE],img->data,sizeof(uint8_t) * img->size);
        printf("lee\n");
        int values[3] = {img->channels,img->height,img->width};                   
        write(pipeI[WRITE],values,sizeof(int)*3);                                     
        Image_free(img);
        free(img);
        return 0;


    }  
}