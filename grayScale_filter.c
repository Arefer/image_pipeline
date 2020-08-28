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
    int status;
    read(STDIN_FILENO,img,sizeof(Image*));
    fprintf(stderr,img);
    int fd[2];
    if(pipe(fd) < 0)
    {
        fprintf(stderr,"Error al crear el pipe\n");
        return 0;
    }
    int pid = fork();
    if(pid > 0)
    {
        close(fd[READ]);
        img = gray_scale_stage(img);
        write(fd[WRITE],img,sizeof(Image*));

    }
    else if(pid == 0)
    {
        close(fd[WRITE]);
        dup2(fd[READ],STDIN_FILENO);
        execl("binarize","binarize",NULL);
        fprintf(stderr,"Fallo execl\n");
        exit(0);
    }
    waitpid(pid, &status,0);
    return 0;
}