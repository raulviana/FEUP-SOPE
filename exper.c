#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>

#define MAXLINE 512


char getFileInfo(char* filename, char filetype[], char* processPath, char* process){

int fd1[2], fd2[2], n;
pid_t pid;
pipe(fd1); pipe(fd2);
char result[MAXLINE];
const char token[2] = ":";
const char* ptr;

pid = fork();
if(pid > 0){
  //parent
  close(fd1[0]); close(fd2[1]);
  write(fd1[1], filename, sizeof(filename));
  n = read(fd2[0], result, MAXLINE);
  result[n] = 0;
	int i = 0;
	while(result[i]!='\0') {
      if(result[i]==',') {
        result[i]='|';
        }
        i++;
  }
ptr = strrchr(result, *token);
if(ptr != NULL) strcpy(filetype, ptr + 1);
else strcpy(filetype, result);
return *filetype;
}
else{
  char in[MAXLINE];
	int n2;
  close(fd1[1]); close(fd2[0]);
  dup2(fd1[0], STDIN_FILENO);
  dup2(fd2[1], STDOUT_FILENO);
  n2 = read(fd1[0], in, sizeof(filename));
  in[n2] = 0;
  close (fd1[0]);
  execl(processPath, process, in, NULL);
  close(fd2[1]);
  exit(0);
}
}

int main(void){
	char filetype[80];
	char file[] = "md5sum";
	char filePath[] = "/usr/bin/md5sum";
  char filename[] = "test";
  getFileInfo(filename, filetype, filePath, file);
  printf("%s", filetype);

}
