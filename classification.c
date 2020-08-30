#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "image/image.h"
#include "image/image_filters.h"
#include "utils/utils.h"
#include "image_pipeline.h"

#define READ 0
#define WRITE 1
#define STDOUT 1


int main(int argc, char *argv[])
{
    Image* img;
    int pid, status, nb;
    char* classification;
    read(STDIN_FILENO,img,sizeof(Image*));
    int fd[2];
    if(pipe(fd) < 0)
    {
        fprintf(stderr,"Error al crear el pipe\n");
        return 0;
    }
    pid = fork();
    if(pid > 0)
    {
        close(fd[READ]);
        nb = classification_stage(img,argv[1],(double)atoi(argv[2]));
        if (nb == 1) classification = "NEARLY_BLACK";
            else if (nb == 0) classification = "NOT_NEARLY_BLACK";
            else { printf("Error al clasificar imagen '%s'. Posiblemente umbral incorrecto\n", argv[1]); return 1;}
        write(fd[WRITE],classification,100);

    }
    else if(pid == 0)
    {
        close(fd[WRITE]);
        dup2(fd[READ],STDIN_FILENO);
        execl("write","write",NULL);
        fprintf(stderr,"Fallo execl\n");
        exit(0);
    }
    waitpid(pid, &status,0);
    return 0;
}