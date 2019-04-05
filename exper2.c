#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <wait.h>

#define MAXLINE 512

int main(void){

char check[] = "md5,sh1";
char s[2] = ",";
char temp[3];
char* token;
int count = 0;

token = strtok(check, s);
   
   while( token != NULL ) {
    
    
    
      token = strtok(NULL, s);
      
      count++;
      printf("%d\n", count);
   }

 exit(0);
}

