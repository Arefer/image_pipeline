#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define READ 0
#define WRITE 1


//Funcion para separar un canal de la imgen de los datos generales de la imagen
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




int maxPool(int* matrix, int index, int n, int m, int width)
{
    int max = matrix[index];
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < m; j++)
        {
            if(matrix[index + j] > max)
            {
                max = matrix[index + j];
            }
        }
        index+=width;
    }
    return max;
}

//m: width, n: heigth
int* pooling(int n, int m, int heigth,int width, int* matrix)
{
    int newHeigth = heigth /n;
    int newWidth = width / m;
    int size = newHeigth * newWidth;
    int index = 0;
    int* exitData = (int*)malloc(sizeof(int) * size);
    for(int i = 0;i < width; i = i + n)
    {
        for(int j = 0; j < heigth; j = j + n)
        {
            exitData[index] = maxPool(matrix,index,n,m,width);
            index++;

        }
    }
    return exitData;
    
}

int* poolImage(int* data, int n, int m,int heigth,int width, int channels)
{
    int** channelsInfo = (int**)malloc(sizeof(int*) * channels);
    int** exitData = (int**)malloc(sizeof(int*) * channels);
    int size = width/m*heigth/n;
    for(int i = 0; i < channels; i++)
    {
        channelsInfo[i] = (int*)malloc(sizeof(int) * (heigth * width));
        exitData[i] = (int*)malloc(sizeof(int) * (heigth * width));
        channelsInfo[i] = separateChannels(i + 1,channels,heigth * width *channels,data);
        for(int j = 0; j < width*heigth;j++)
        {
            printf("conv- %d",channelsInfo[i][j]);
        }        
        //free(channelsInfo[i]);   
        printf("\n");     
        exitData[i] = pooling(n,m,heigth,width,channelsInfo[i]);
        /*for(int j = 0; j < width*heigth/(n*m);j++)
        {
            printf("conv %d",exitData[i][j]);
        } */       
        free(channelsInfo[i]);
    }
    /*for(int i = 0; i < size; i++)
    {
        printf("primer[%d] %d\n",i,exitData[0][i]);
    }*/
    int* newData = joinChannels(channels,size,exitData); 
    free(channelsInfo);    
    return newData;    
}

int main (int argc, char* argv[])
{    
    int heigth = atoi(argv[2]);
    int channels = atoi(argv[1]);
    int width = atoi(argv[3]);
    int size = heigth*width * channels;    
    int* img = (int*)malloc(sizeof(int) * (heigth*width * channels));
    read(STDIN_FILENO,img,sizeof(int) * (heigth*width*channels));  
    int* salida = poolImage(img,2,2,heigth,width,channels);
    printf("pool\n");
    stbi_write_png("resources/Salida.png",width/2,heigth/2,channels,salida,width*heigth*channels/4);
    for(int i = 0; i < size/4; i+=3)
    {
        printf("%d [%d]",salida[i],i);
        
    }
    printf("\n");
    free(img);
    return 0;
}