#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


int main(void)
{
 char filePath[100];
 char outProcess[100];
 strcpy(outProcess, "file");
 strcpy(filePath, " folder");
 strcat(outProcess, filePath);

 FILE* file = popen(outProcess, "r");

 if(file != NULL){
    char buffer[100];
    fgets(buffer, 100, file);
    pclose(file);
    char *FileType = strstr(buffer, " ");
    printf(FileType);
    
 }
 else printf("Error getting info from file");
 return 0;
}