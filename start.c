#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(){
    int pid, status;
    char img_path[100] = "imagen\0";
    int fd[2]; // arreglo para pipe
    for (int  i = 0; i < 1; i++)
    {
        char img_path_i[200];
        char dest_img_path_i[200];
        sprintf(img_path_i, "%s_%d.jpg", img_path, i+1);
        sprintf(dest_img_path_i, "%s_%d_bin.jpg", img_path, i+1);
        if((pid = fork()) < 0)
        {
            printf("No se pudo crear hijo\n");
            exit(1);
        }
        if (pid == 0)
        {
            execl("lecture","lecture",img_path_i,dest_img_path_i,NULL);
            exit(0);
        }
        else
        {
            waitpid(pid, &status,0);
            fflush(stdout); // para vaciar los buffer del SO
        }
    }
    
    
}