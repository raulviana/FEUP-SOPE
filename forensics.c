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

//Chama o processo externo "file" para obter o tipo de ficheiro
char getFileInfo(char* nameFilePtr, char filetype[], char* filePathPtr, char* filePtr){

int fd1[2], fd2[2], n;
pid_t pid;
pipe(fd1); pipe(fd2);
char result[MAXLINE];
const char token[2] = ":";
const char* ptr;
char nameFile[512];     //File to be procesed by the co-process
nameFile[0] = '\0';
strcpy(nameFile, nameFilePtr);
char filePath[512];    //Co-Proces path to be called 
strcpy(filePath, filePathPtr);
char file[512];       //Co-Process name to be called
strcpy(file, filePath);   

int status;
pid = fork();
if(pid > 0){
  //parent
  close(fd1[0]); close(fd2[1]);
  write(fd1[1], nameFile, sizeof(nameFile));
  n = read(fd2[0], result, MAXLINE);
  result[n] = 0;
	int i = 0;
	while(result[i]!='\0') {
      if(result[i]==',') { //Se result contiver ',' sera substituido por '|'
        result[i]='|';
        }
        i++;
  }
	ptr = strrchr(result, *token);
  if(ptr != NULL) strcpy(filetype, ptr + 1);
	else strcpy(filetype, result);
	wait(&status);
  return *filetype;
}
else{
	
  char in[MAXLINE];
	int n2;
  close(fd1[1]); close(fd2[0]);
  dup2(fd1[0], STDIN_FILENO);
  dup2(fd2[1], STDOUT_FILENO);
  n2 = read(fd1[0], in, sizeof(nameFile));
  in[n2] = 0;
  close (fd1[0]);
	
  execl(filePath, file, in, NULL);
  close(fd2[1]);
  exit(0);
}
}

//Get the file name from file path
//--------------------------------
void cleanName(char name[]){
	char temp[512];
	char* ptr;
	ptr = strtok(name, "/");
	while (ptr != NULL){
		
		strcpy(temp, ptr);
		ptr = strtok(NULL, "/");
		if(ptr == NULL){
			strcpy(name, temp);
		}
	}
}

//Imprime caracteristicas do ficheiro
//_____________________________________
void printFileInfo(char nameFilePtr[], int checksums, char outFilePathPtr[], int toFile) {
  
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
  char nameFile[256];
	int fd_out;
	strcpy(nameFile, nameFilePtr);
	getFileInfo(nameFile, filetype, filePath, file);
	strtok(filetype, "\n");  //retira o newline

	// Changes the output to te specified file
	//------------------------------------------
	if (!toFile) { //argument "-o" present
	
	char outFilePath[256];
	strcpy(outFilePath, outFilePathPtr);
	fd_out = open(outFilePath, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd_out == -1) {
	   printf("Couldn't open file to output\n");
	   exit(1);
	}
	dup2(fd_out, STDOUT_FILENO);
	}
	//-------------------------------------------
  
	if (stat(nameFile, &file_info) < 0) {
		printf("Error in %d Unable to get file info\n", getpid()); //nao será print. mas terminr com um codigo de erro???
		exit(1);
	}
	char tempnameFile[512];
	strcpy(tempnameFile, nameFile); // guarda-se temporariamente o caminho completo, pois nameFile vai ser usado mais à frente novamente 
	cleanName(nameFile);
	printf("%s", nameFile);
	strcpy(nameFile, tempnameFile); //volta-se a repor nameFile com o caminho completo para o ficheiro
	printf(",");
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

		if(checksums >= 1){
		printf(", ");
		getFileInfo(nameFile, md5result, md5Path, md5);
		strtok(md5result, " ");
	  printf("%s", md5result);
		}
    
		if(checksums >= 2){
		printf(", ");
    getFileInfo(nameFile, sha1result, sha1Path, sha1);
		strtok(sha1result, " ");
	  printf("%s", sha1result);
		}

		if(checksums >= 3){
		printf(", ");
		getFileInfo(nameFile, sha256result, sha256Path, sha256);
		strtok(sha256result, " ");
		printf("%s", sha256result);
		}
		
		printf("\n");

	// Replaces the output back to stdout
	//----------------------------------
	if(!toFile){  //argument "-o" present
		fflush(stdout);
	  close(fd_out);
	  dup2(1, STDOUT_FILENO);
	}
}
//-----------------------------------------------------------


int main(int argc, char *argv[], char **envp){

/*char* arguments[argc-1];

//SAVING ARGUMENTS 
	int i;

        if ( argc == 1 ) {
                printf( "No arguments were passed.\n" );
        } else {
                for ( i = 1; i < argc; i++) {
					arguments[i-1] = argv[i];
                }
		}

		for (i = 0; i < argc; i++)
			printf("%s\n", arguments[i]);
		*/
	//Para as variaveis de ambiente acho que devemos fazer um vetor, mas para estas seria
	//mais pratico fazer assim, o que acham? 




char sumsAsked[MAXLINE];						      //
strcpy(sumsAsked, argv[3]);           //
int checksums = 0;                     //
char s[2] = ",";                     //
char* token;                         //Calcula o numero de hashes que serao calculadas,posteriormente poderia-se ver quais sao efectivamente pedidas                    //
token = strtok(sumsAsked, s);       //
while( token != NULL ) {            //
  token = strtok(NULL, s);          //
  checksums++;
}

char outFilePath[MAXLINE];
strcpy(outFilePath, argv[5]);
char entryFile[512];
strcpy(entryFile, argv[7]);
int argumentO;
char toFile[MAXLINE];
strcpy(toFile, argv[4]);
argumentO = strcmp(toFile, "-o");

/*
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

char content[512];
char temp[512];
DIR* Dir;
struct dirent* DirEntry;
      
struct stat filestat;
stat(entryFile, &filestat); 
    
if(S_ISDIR(filestat.st_mode)){ //entryfFile e um diretorio, faz fork e processa o conteudo noutro processo

    Dir = opendir(entryFile);
    if (Dir == NULL) {
    	printf("Error reading directory %s", entryFile); //para log?
    	exit(2);
    }

    while ((DirEntry = readdir(Dir)) != NULL) {
        content[0] = '\0';
        temp[0] = '\0';
        strcpy(temp, DirEntry->d_name); 
        strcat(content, entryFile);
        strcat(content, "/");
        strcat(content, temp);
        stat(content, &filestat);

        if(S_ISDIR(filestat.st_mode)){   //se este conteudo da pasta for diretorio, entra em recursao
          if((temp[0] != '.' && temp[1] != '.') ||   //Filtra diretorios do tipo '.' e '..'
            (temp[0] != '.' && temp[1] != '\0')) {

		              pid_t pid;
		              int status;
		              pid = fork();
              
		              if(pid != 0){ //parent
                      wait(&status);
                     // if (WIFEXITED(status)) 
                      //printf("Exit status: %d\n", WEXITSTATUS(status)); 
		              }
		              else{ //child
							strcat(entryFile, "/");
                            strcat(entryFile, temp);
							Dir = opendir(entryFile);
                            if (Dir == NULL) {
    	                        printf("Error reading directory in child %s", entryFile); //para log?
    	                        exit(2);
                            }
				    	}
          }
        }
        else{   //chama analide do ficheiro
        printFileInfo(content, checksums, outFilePath, argumentO);          
        }
    }
    closedir(Dir);
    }		
else{    // Entryfile e um ficheiro, pede a sua analise
    printFileInfo(content, checksums,outFilePath, argumentO);
 }

 exit(0);
}

