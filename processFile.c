
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char* argv[]){
 
    struct stat file_info;


    printf("%s", argv[1]);
    printf(",");
    printf("tipo ficheiro,");
    printf("%ld,", file_info.st_size);
    printf("%s,", ctime(&file_info.st_atime));
    printf("%s,", ctime(&file_info.st_ctime));
    printf("%s,", ctime(&file_info.st_mtime));



    exit(0);
}