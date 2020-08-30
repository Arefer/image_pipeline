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
    int pid, status;
    int mask_load_sucess = 0;
    Mask* mask = read_mask(argv[2], &mask_load_sucess);
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
        img = binarize_stage(img, atoi(argv[1]),argv[1]);
        free(mask);
        write(fd[WRITE],img,sizeof(Image*));
    }
    else if(pid == 0)
    {
        close(fd[WRITE]);
        dup2(fd[READ],STDIN_FILENO);
        execl("clasification","clasification",NULL);
        fprintf(stderr,"Fallo execl\n");
        exit(0);
    }
    waitpid(pid, &status,0);
    return 0;
}