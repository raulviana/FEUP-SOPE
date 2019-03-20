

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char* argv[], char* envp[]){
  
    


    /*
    -------------------------------------------------
    Process the file and produce the required output
    ------------------------------------------------
                                                    */
    struct stat file_info;
    char time_buffer[80];
   
    if(stat(argv[1],&file_info) < 0) return 1;

    printf("%s", argv[1]);
    printf(",");
    printf("tipo ficheiro,"); // falta ver como se ve o tipo de ficheiro, no exemplo ASCCI TEXT
    printf("%ld,", file_info.st_size);

    struct tm *accessed_stamp = localtime(&file_info.st_mtime);
    strftime(time_buffer,80,"%FT%T", accessed_stamp);
    printf("%s,", time_buffer);

    struct tm *created_stamp = localtime(&file_info.st_atime);
    strftime(time_buffer,80,"%FT%T", created_stamp);
    printf("%s,", time_buffer);

    struct tm *modified_stamp = localtime(&file_info.st_ctime);
    strftime(time_buffer,80,"%FT%T", modified_stamp);
    printf("%s,", time_buffer);


    exit(0);
}