#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <signal.h>

#define MAXLINE 512

int main(void){

int fd1[2], fd2[2], n;
pid_t pid;
pipe(fd1); pipe(fd2);
char line[7] = "test";
char result[MAXLINE];

pid = fork();
if(pid > 0){
  //parent
  close(fd1[0]); close(fd2[1]);
  write(fd1[1], line, sizeof(line));
  n = read(fd2[0], result, MAXLINE);
  result[n] = 0;
  char* delimiter = ":";
  char* ptr;
  ptr = strtok(result, delimiter);
  printf("%s", result);

  exit(0);
}
else{
  char in[MAXLINE];
  int n2;
  close(fd1[1]); close(fd2[0]);
  dup2(fd1[0], STDIN_FILENO);
  dup2(fd2[1], STDOUT_FILENO);
  n2 = read(fd1[0], in, sizeof(line));
  in[n2] = 0;
  close (fd1[0]);
  execl("/usr/bin/file", "file", in, NULL);
  close(fd2[1]);
  exit(0);

}
 
}
