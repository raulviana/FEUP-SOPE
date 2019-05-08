#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include "../constants.h"
#include "../types.h"
#include "../crypto.c"

#include "communication.c" 

#define ERR_ARGS 1
#define ERR_FIFO 2

#define MAX_LINE 512

typedef unsigned int u_int;

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./server <Number of Eletronic Bank Officces> <\"Admin Password\">\n");
}

int main(int argc, char*argv[]){

    //******************Set Up Server******************************
  /*  struct account {
        u_int ID;
        int balance;
        char shar256[HASH_LEN];
    };
    struct account accounts_array[MAX_BANK_ACCOUNTS]; */

    //Verify arguments**********************************
    if (argc != 3) {
        printf("Wrong number of arguments\n");
            print_usage(stderr);
        exit(ERR_ARGS);
    }
    
    int num_eletronic_counter = atoi(argv[1]);
    if(num_eletronic_counter >  MAX_BANK_OFFICES  || num_eletronic_counter <= 0) {
        fprintf(stderr, "Invalid Number of Eletronic Counters, must be between 1 and 99\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }

    char password[MAX_PASSWORD_LEN];
    strcpy(password, argv[2]);
    if(strlen(password) < MIN_PASSWORD_LEN || strlen(password) > MAX_PASSWORD_LEN){
        printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    password[strlen(password)] = '\0'; // retira espaço vazio

    //******************************************

   //TODO abrir conta de admimnostrador

    //Server fifo **********************
    int fd, fd_dummy;
   // char line[MAX_LINE];
   // line[0] = '\0';

    if (mkfifo(SERVER_FIFO_PATH,0660)<0)
        if (errno==EEXIST) printf("Requests FIFO already exists\n");
        else {
            printf("Can't create Requests FIFO\n");
            print_usage(stderr);
            exit(ERR_FIFO);
        }
    else printf("Requests FIFO sucessfully created\n");
    if ((fd=open(SERVER_FIFO_PATH,O_RDONLY)) !=-1)
        printf("Requests FIFO created in READONLY mode\n"); 
    if ((fd_dummy=open(SERVER_FIFO_PATH,O_WRONLY)) !=-1)
        printf("Request FIFO openned in WRITEONLY mode\n");
    //*************************************
/*
    do {
        read(fd,&line, ;
        if (opcode!=0) {
        read(fd,name,MAX_NAME_LEN);
        printf("%s has requested operation %d\n",name,opcode);
        }
    } while (opcode!=0);*/
     
    //Read from FIFO








    close(fd);
    close(fd_dummy);
    //Erase FIFO******************************
    if (unlink(SERVER_FIFO_PATH) < 0)
        printf("Error when destroying FIFO '/tmp/requests'\n");
    else
        printf("FIFO '/tmp/requests' has been destroyed\n");
    //*************************************** 


    return 0;
}