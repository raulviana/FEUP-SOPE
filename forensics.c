

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char* argv[], char* envp[]){
    char *name = "./processFile";
    char *arguments[2];
    arguments[0] = name;
    arguments[1] = argv[1];
    arguments[2] = NULL;
   execvp(name, arguments);

    exit(0);
}