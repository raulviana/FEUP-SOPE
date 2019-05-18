
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdio.h>

#include "constants.h"
#include "types.h"

#define MAXLINE 512

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
        char command[1024] = "echo -n";
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




