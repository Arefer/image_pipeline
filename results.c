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
    print_pipe_results(argv[0],argv[1]);
    return 0;
}