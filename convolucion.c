#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include "image/image.h"

#define READ 0
#define WRITE 1


// Funcion para separar un canal de la imgen de los datos generales de la imagen
//Entradas:
//  channel: entero con el numero de canal que se desea separar(parte de 1)
//  numberChannels: numero de canales que posee la imagen
//  size: tamaño de la imagen(ancho * largo * numero de canales)
//  data: datos de la imagen
//Salida: arreglo con los valores del canal seleccionado
int* separateChannels(int channel, int numberChannels, int size , int *data)
{
    int sizeArray = size/numberChannels;
    int *cape = (int*)malloc(sizeof(int) * sizeArray);    
    if(channel > numberChannels)
    {
        return NULL;
    }
    else
    {            
        int j = 0;
        for(int i = channel - 1; i < size; i = i + numberChannels)
        {
            cape[j] = data[i];
            j++;
            //printf("%d\n",cape[j]);
        }
        printf("%d\n", cape[0]);
        return cape;

    }
}
// Funcion para unir los datos procesados en un solo arreglo de enteros
// Entradas:
//  numberChannels: numero de canales de la imagen
//  size: entero del tamaño de la imagen (altura * ancho * numero de canales)
//  data: doble puntero con los datos de la imagen separados por capa
// Salida: arreglo unico con los datos de la imagen
int* joinChannels(int numberChannels, int size , int **data)
{
    int* exitData = (int*)malloc(sizeof(int)* (size*numberChannels));
    int index = 0;
    for(int i = 0; i < numberChannels; i++)
    {
        index = i;
        for (int j = 0; j < size; j++)
        {
            exitData[index] = data[i][j];
            index+=numberChannels;
        }
        index = 0;
    }
    return exitData;
}

// Funcion para leer el archivo de texto del filtro para realizar la convolucion
// Entrada: 
//  file: arreglo de caracteres con el nombre de el documento del filtro
// Salida: arreglo de enteros con los valores del filtro
int* readFilter(char* file)
{
        
    FILE* fp = fopen(file,"r");    
    int* filter = (int *)malloc(sizeof(int) * 9);    
    int i = 0, j = 1, k = 2;    
    while(!feof(fp))
    {  
        fscanf(fp,"%d %d %d\n",&filter[i],&filter[j], &filter[k]);
        i = i + 3;
        j = j + 3;
        k = k + 3;        
    }
    fclose(fp);
    /*for(int i = 0; i < 9; i++)
    {
        printf("%d\n",filter[i]);
    }*/
    return filter;


}

// funcion para aplicar la convolucion a una capa que corresponde a un canal de la imagen, se le aplica el filtro 
// y segun al pixel al que se le quiera aplicar la convolucion existen las restricciones de borde correspondientes
// Entradas:
//  data: arreglo de enteros con la informacion de la imagen
//  filter: arreglo de enteros con los valores del filtro
//  width: ancho de la imagen que se usa para desplazarse en el arreglo de los datos
//  heigth: alto de la imagen que se usa para desplazarse en el arreglo de datos
// Salida: capa de datos con la convolucion realizada
int* convolution(int *data, int *filter, int width, int heigth)
{
    int index = 0;
    int size = width*heigth;
    int *newData = (int*)malloc(sizeof(int) * size);
    for(int i = 0; i < heigth; i++)
    {
        for(int j = 0; j < width; j++)
        {
            //printf("indice %d \n",index);
            if(j == 0 && i == 0)
            {
                newData[index] = data[0] * (filter[3]) + data[1]*filter[4] + data[width]*filter[7] + data[width + 1] * filter[8];                                

            }
            else if(j == 0 && i > 0 && i < heigth - 1)
            {
                newData[index] = filter[1] * data[index - width] + filter[2] * data[index - width + 1] + filter[4] * data[index] + filter[5] * data[index + 1] + filter[7] * data[index + width] + filter[8] * data[index + width + 1];                
            }
            else if(j == 0 && i ==  heigth - 1)
            {
                newData[index] = filter[1] * data[index - width] + filter[2] * data[index - width + 1] + filter[4] * data[index] + filter[5] * data[index + 1];                
            }
            else if(i == heigth - 1 && j > 0 && j < width -1)
            {
                newData[index] =filter[0] * data[index - width - 1]+  filter[1] * data[index - width] + filter[2] * data[index - width + 1] +filter[3] * data[index - 1]+ filter[4] * data[index] + filter[5] * data[index + 1];                
            }
            else if(i == heigth - 1 && j == width - 1)
            {
                newData[index] = filter[0] * data[index - width - 1]+  filter[1] * data[index - width] +filter[3] * data[index - 1]+ filter[4] * data[index];
                //printf("valores imagen %d %d %d %d\n",data[index - width - 1], data[index - width], data[index - 1], data[index]);            
                //printf("valores imagen %d\n",newData[index]);                

            }
            else if(i < heigth && i > 0 && j == width - 1)
            {
                newData[index] =filter[0] * data[index - width - 1]+  filter[1] * data[index - width] +filter[3] * data[index - 1]+ filter[4] * data[index] + filter[6] * data[index + width - 1]+ filter[7] * data[index + width];
              
            }
            else if(i == 0 && j == width - 1)
            {
                newData[index] = filter[3] * data[index - 1]+ filter[4] * data[index] + filter[6] * data[index + width - 1]+ filter[7] * data[index + width];                
            }
            else if(i == 0 && j > 0 && j < width - 1)
            {
                newData[index] = filter[3] * data[index - 1]+ filter[4] * data[index] + filter[5] * data[index + 1] + filter[6] * data[index + width - 1]+ filter[7] * data[index + width] + filter[8] * data[index + width + 1];
            }
            else
            {
                newData[index] =filter[0] * data[index - width - 1]+  filter[1] * data[index - width] + filter[2] * data[index - width + 1] +filter[3] * data[index - 1]+ filter[4] * data[index] + filter[5] * data[index + 1] + filter[6] * data[index + width - 1]+ filter[7] * data[index + width] + filter[8] * data[index + width + 1];
            }
            newData[index] = newData[index]/9; 
            index++;        

        }        
    }
    return newData;

}

// Funcion para aplicar la convolucion a todas las capas de la imagen
// Entradas:
//  filter: arreglo de caracteres con el nombre de el documento del filtro
// numberChannels: entero con el numero de canales de la imagen
//  heigth: entero con la altura de la imagen
//  width: entero con el ancho de la imagen
//  data: arreglo con los enteros correspondientes a los datos de la imagen
// Salida: arreglo de enteros resultantes de aplicar la convolucion a todas las capas de la imagen
int* convolutionImage(char* fd, int numberChannels, int heigth, int width, int* data)
{
    int* filter = readFilter(fd);    
    int** channelsInfo = (int**)malloc(sizeof(int*) * numberChannels);
    int** exitData = (int**)malloc(sizeof(int*) * numberChannels);
    int size = width*heigth;
    for(int i = 0; i < numberChannels; i++)
    {
        channelsInfo[i] = (int*)malloc(sizeof(int) * (heigth * width));
        exitData[i] = (int*)malloc(sizeof(int) * (heigth * width));
        channelsInfo[i] = separateChannels(i + 1,numberChannels,heigth * width *numberChannels,data);
        exitData[i] = convolution(channelsInfo[i],filter,width,heigth);
        /*for(int j = 0; j < width*heigth;j++)
        {
            printf("conv %d",exitData[i][j]);
        }*/
        free(channelsInfo[i]);
    }
    /*for(int i = 0; i < size; i++)
    {
        printf("primer[%d] %d\n",i,exitData[0][i]);
    }*/
    int* newData = joinChannels(numberChannels,size,exitData); 
    free(channelsInfo);
    free(filter);    
    return newData;
}

// Funcion para transformar un arreglo de enteros de 8bits sin signo a enteros
// Entradas:
//  data: arreglo de datos del tipo uint8_t
//  size: tamaño del arreglo
// Salida: arreglo de enteros
int* cast(uint8_t* data, int size)
{
    int* newData = (int*) malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++)
    {
        newData[i] = (int)data[i];
    }
    return newData;

}

// Proceso para aplicar la convolucion  a la imagen de turno en la etapa del pipeline

int main(int argc, char* argv[])
{
    printf("convolucion\n");
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
        char* buf[7] = {"./reLu",argv[1] ,argv[2],argv[3],argv[4],argv[5],NULL}; 
        execv("./reLu",buf);
        perror("fallo execv\n");
        exit(-1);                       


    }    
    int heigth = atoi(argv[2]);
    int channels = atoi(argv[1]);
    int width = atoi(argv[3]);
    int size = heigth*width * channels;    
    //uint* data = (uint*)malloc(sizeof(uint) * (heigth*width));
    uint8_t* img = (uint8_t*)malloc(sizeof(uint8_t) * (heigth*width * channels));
    read(STDIN_FILENO,img,sizeof(uint8_t) * (heigth*width*channels));    
    int* newImage = cast(img,size);
    int*salida = convolutionImage("resources/mat.txt",channels,heigth,width,newImage);  
    write(piped[WRITE],salida,sizeof(int) * size);            
    for(int i = 0; i < size; i+=3)
    {
        printf("%d [%d| ",img[i], i);
    }
    printf("\n\n");
    for(int i = 0; i < size; i+=3)
    {
        printf("%d [%d| ",salida[i], i);
    }
    printf("\n\n");
    wait(&pid);
    free(img);
    free(newImage);
    return 0;
}
