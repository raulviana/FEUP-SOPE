
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "constants.h"

#define MAXLINE 512
/*
char getSha256(char* nameFilePtr, char sha256result[]){
        char* filePathPtr = "/usr/bin/sha256sum";
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
        if (pid > 0) {
                //parent
                close(fd1[0]); close(fd2[1]);
                write(fd1[1], nameFile, sizeof(nameFile));
                n = read(fd2[0], result, MAXLINE);
                result[n] = 0;
                int i = 0;
                while (result[i] != '\0') {
                        if (result[i] == ',') { //Se result contiver ',' sera substituido por '|'
                                result[i] = '|';
                        }
                        i++;
                }
                ptr = strrchr(result, *token);
                if (ptr != NULL) strcpy(sha256result, ptr + 1);
                else strcpy(sha256result, result);
                wait(&status);
                return *sha256result;
        }
        else {
                char in[MAXLINE];
                int n2;
                close(fd1[1]); close(fd2[0]);
                dup2(fd1[0], STDIN_FILENO);
                dup2(fd2[1], STDOUT_FILENO);
                n2 = read(fd1[0], in, sizeof(nameFile));
                in[n2] = 0;
                close(fd1[0]);
                execl(filePath, file, in, NULL);
                close(fd2[1]);
                exit(0);
        }
}*/

char* runCommand(char* command){

    FILE *fp;

    //printf("\nCommand %s", command);
    /* Open the command for reading. */
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

//    char path[1024];
    char output[1024];

    fscanf(fp,"%[^\n]", output);

    /* close */
    pclose(fp);

    return strdup(&output[0]);
}

char* getSha256(char* password, char* salt){
        //returns the sha256sum of password+salt
        char command[1024] = "echo -n ";
        strcat(command, password);
        strcat(command, salt);
        strcat(command, " | sha256sum");
        return runCommand(command);

}

bool checkPassword(char* password, char* salt, bank_account_t account){
        //check if the sha256sum of password+salt is equal to the hash of the account
        char command[1024] = "echo -n ";
        strcat(command, password);
        strcat(command, salt);
        strcat(command, " | sha256sum");
        if (strcmp(account.hash, runCommand(command))) return true;
        else return false;

}

void createSalt(char* newSalt){
    int min_Ascci = 48;
    int i = 0, n;
    time_t t;
   
    //Intializes random number generator
    srand((unsigned) time(&t));

    while(strlen(newSalt) < HASH_LEN){
      n = (rand() % 75) + min_Ascci; //n está entre 48 e 122
      if ((n > 57 && n < 65) || (n > 90 && n < 97)) continue; //  ! [0..9][a..z][A..Z]
      else {
        newSalt[i] = n;
      }
      newSalt[i + 1] = '\0';
      i++;
    }
}

int isPassOk(char passSalted){
    return 0;
}




