
#include <stdio.h>
#include "image/image.h"
#include "image/image_filters.h"
#include "utils/utils.h"
#include <unistd.h>
#include "image_pipeline.h"
#include <sys/wait.h>
#include <sys/types.h>


#define READ 0
#define WRITE 1
#define STDOUT 1


int main(int argc, char *argv[]){
    int pid, status;
    int fd[2]; // arreglo para pipe
    if(pipe(fd) < 0)
    {
        printf("Error al crear pipe\n");
        exit(0);
    }
    if((pid = fork()) < 0)
    {
        printf("No se pudo crear hijo");
        exit(1);
    }
    if(pid == 0) 
    {
        close(fd[WRITE]);
        dup2(fd[READ], STDIN_FILENO);
        execl("grayscale","grayscale",NULL);
    }
    else
    {
        close(fd[READ]);
        Image* img = (Image*)malloc(sizeof(Image));
        lecture_stage(img, argv[1]);
        write(fd[WRITE],img,10000);
    }

    waitpid(pid, &status,0);
    return 0;
    


    
    
}