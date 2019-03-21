

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

void print(char* filemame, char* argv[]){
    struct stat file_info;
    char time_buffer[80];

    if(stat(argv[1],&file_info) < 0){
        printf("Unable to get file info");
        exit(1);
    } 

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
    printf("%s\n", time_buffer);
}

int main(int argc, char* argv[], char* envp[]){
  

  

    /*
    -------------------------------------------------
    Process the file and produce the required output
    ------------------------------------------------
                                                    */
    char outFilePath[80] = "./outFile.csv";
    int fd_out; 
    
    if(1){ //argument "-o" present
        fd_out = open(outFilePath, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(fd_out == -1){
            printf("Couln't open file to output\n");
            return 1;
        } 
    dup2(fd_out, STDOUT_FILENO);
    print(outFilePath, argv);
    fflush(stdout);
    close(fd_out);
    dup2(1, STDOUT_FILENO);
    }
    else{
        print(outFilePath, argv);
    }

    exit(0);
}
