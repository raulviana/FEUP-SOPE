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

#define READ 0
#define WRITE 1
#define MAXLINE 512


//Chama o processo externo "file" para obter o tipo de ficheiro
char getFileInfo(char* filename, char filetype[], char* filePath, char* file){

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
	
  execl(filePath, file, in, NULL);
  close(fd2[1]);
  exit(0);
}
}

//Imprime caracteristicas do ficheiro
//_____________________________________

void printFileInfo(char* filename, int cripto) {
	struct stat file_info;
	char time_buffer[80];
	char filetype[80];
	char filePath[] = "/usr/bin/file";
	char file[] = "file";
	char md5[] = "md5sum";
	char md5Path[] = "/usr/bin/md5sum";
	char md5result[80];
	char sha1[] = "sha1sum";
	char sha1Path[] = "/usr/bin/sha1sum";
	char sha1result[80];
	char sha256[] = "sha256sum";
	char sha256Path[] = "/usr/bin/sha256sum";
	char sha256result[80];

	getFileInfo(filename, filetype, filePath, file);
	strtok(filetype, "\n");
	
	printf("%s", filename);
	printf(",");
	if (stat(filename, &file_info) < 0) {
		printf("Unable to get file info"); //nao será print. mas terminr com um codigo de erro???
		exit(1);
	}
	printf("%s", filetype);
	printf(", "); 
	printf("%ld,", file_info.st_size);

	struct tm *accessed_stamp = localtime(&file_info.st_mtime);
	strftime(time_buffer, 80, "%FT%T", accessed_stamp);
	printf("%s, ", time_buffer);

	struct tm *created_stamp = localtime(&file_info.st_atime);
	strftime(time_buffer, 80, "%FT%T", created_stamp);
	printf("%s, ", time_buffer);

	struct tm *modified_stamp = localtime(&file_info.st_ctime);
	strftime(time_buffer, 80, "%FT%T", modified_stamp);
	printf("%s", time_buffer);

		if(cripto >= 1){
		printf(", ");
		getFileInfo(filename, md5result, md5Path, md5);
		strtok(md5result, " ");
	  printf("%s", md5result);
		}
    
		if(cripto >= 2){
		printf(", ");
    getFileInfo(filename, sha1result, sha1Path, sha1);
		strtok(sha1result, " ");
	  printf("%s", sha1result);
		}

		if(cripto >= 3){
		printf(", ");
		getFileInfo(filename, sha256result, sha256Path, sha256);
		strtok(sha256result, " ");
		printf("%s", sha256result);
		}

		printf("\n");
	
}
//-----------------------------------------------------------



void recurs_traverse(char* list[]) {

}



int main(int argc, char* argv[], char* envp[]) {


int cripto = 3;
/*char* arguments[argc-1];

//SAVING ARGUMENTS 
	int i;

        printf( "argc:     %d\n", argc );
        printf( "argv[0]:  %s\n", argv[0] );

        if ( argc == 1 ) {
                printf( "No arguments were passed.\n" );
        } else {
                for ( i = 1; i < argc; i++) {
					arguments[i-1] = argv[i];
                }
		}

		printf("Resultant array is\n"); //confirms if arguments array is correct
		for (i = 0; i < argc-1; i++)
			printf("%s\n", arguments[i]);

 //ENVIROMENT VARIABLES
 char** env;
 char* enviroment[0]; // ??size of array 
 int c = 0;
  //	variables to use in pipes
    for(env=envp;*env!=0;env++)
    {
        char* thisEnv = *env;
		enviroment[c]= thisEnv;
       printf("%s\n",thisEnv);
		c++;
    }
	printf("Resultant array is\n"); //confirms if arguments array is correct
		for (i = 0; i < c; i++)
			printf("%s\n", enviroment[i]);
 
*/
//pid_t pid, pidSon;
//int status;
//char folderContent[80];


	//struct stat stat_buf;*/

	/*
	O ciclo while abaixo tem como função abrir diretórios recursivamente.
	Por algum motivo o programa é incapaz de distinguir um diretório de um ficheiro com a função S_ISDIR(stat_buf.st_mode)
	*/
	/*while (Dirent != NULL) {
		printf("\nWhile ");
		name = Dirent->d_name;
		if (name[0] != '.') {
			stat(name, &stat_buf);
			if (stat(name, &stat_buf) == -1) {
				printf("\nError in stat --- %s", name);
				exit(1);
			}
			if (S_ISDIR(stat_buf.st_mode)) {
				pid = fork();
				printf("fork");
				if (pid > 0) {
					//este é o pai
					printf("\nParent %s", name);
					//pidSon = wait(&status);
					if (pidSon != 0) {
						printf("Error parsing file system");
						exit(1);
					}
				}
				else {
					//este é o filho
					printf("\nChild %s", name);
					D = opendir(name);
					Dirent = readdir(D);
					name = Dirent->d_name;

				}
			}
			else printf("%s", name);
		}
		else printf(" . ");
		printf("\nPID = %d", pid);
		Dirent = readdir(D);
	}

	printf("\nEnd of While ");
*/



	/*
	-------------------------------------------------
	Process the file and produce the required output
	------------------------------------------------
	*/
	
	char outFilePath[80] = "./outFile.csv";
	int fd_out;

	if (0) { //argument "-o" present
	fd_out = open(outFilePath, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd_out == -1) {
	printf("Couldn't open file to output\n");
	return 1;
	}
	dup2(fd_out, STDOUT_FILENO);
	printFileInfo(argv[1], cripto);
	fflush(stdout);
	close(fd_out);
	dup2(1, STDOUT_FILENO);
	}
	else {
	printFileInfo(argv[1], cripto);
	}
	
	exit(0);
}

