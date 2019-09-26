#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#define READ 0
#define WRITE 1

// Funcion para aplicar la rectificacion a la imagen, colocando en 0 los valores negativos
//  matrix: arreglo de enteros con los datos de la imagen
//  size: entero con el tamaño de la imagen (ancho* alto * numero de canales)
// Salida: arreglo de enteros con los valores de la imagen rectificados
int* reLu(int* matrix, int size)
{
    for(int i = 0; i < size; i++)
    {        
        if(matrix[i] < 0)
        {
            matrix[i] = 0;
        }

    }
    return matrix;

}
// Proceso para llevar a cabo la rectificacion de los datos de una imagen 
int main (int argc, char* argv[])
{    
    printf("reLu\n");
    int piped[2];
    //int pipeI[2];        
    int i = pipe(piped);
    //int j = pipe(pipeI);
    //printf("%d,%d\n",pipeI[READ],pipeI[WRITE]);
    pid_t pid = fork();
    if(pid == 0)
    {
        sleep(1);
        close(piped[WRITE]);        
        //close(pipeI[WRITE]);    
        dup2(piped[READ],STDIN_FILENO);
        char* buf[7] = {"./pool",argv[1] ,argv[2],argv[3],argv[4],argv[5],NULL}; 
        execv("./pool",buf);
        perror("fallo execv\n");
        exit(-1);                       


    }     
    int heigth = atoi(argv[2]);
    int channels = atoi(argv[1]);
    int width = atoi(argv[3]);
    int size = heigth*width * channels;    
    int* img = (int*)malloc(sizeof(int) * (heigth*width * channels));
    read(STDIN_FILENO,img,sizeof(int) * (heigth*width*channels));  
    img = reLu(img,size);
    write(piped[WRITE],img,sizeof(int) * size);
    wait(&pid);    
    /*for(int i = 0; i < size; i++)
    {
        printf("asas %d\n",img[i]);
        
    }*/
    free(img);
    return 0;
}