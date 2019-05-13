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
#include <ctype.h>
#include <semaphore.h>

#include "../constants.h"
#include "../types.h"
#include "../crypto.c"
#include "slog.h"
#include "ulog.h"

#include "communication.c" 

#define ERR_ARGS 1
#define ERR_FIFO 2

#define MAX_LINE 512

typedef unsigned int u_int;
int num_eletronic_counter;

sem_t *sem1, *sem2;
pthread_t *threads;

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./server <Number of Eletronic Bank Officces> <\"Admin Password\">\n");
}

/*void makeTlv(request){
    
}*/


void shutdown(){
  /*  int t;
    for (t = 0; t < num_eletronic_counter; t++) {
        pthread_join(threads[t], NULL);
    }
    sem_close(sem1);
    sem_close(sem2);*/
    exit(0);
}

int main(int argc, char*argv[]){

    //******************Set Up Server******************************
    struct account {
        u_int ID;
        int balance;
        char shar256[HASH_LEN];
    };
    struct account accounts_array[MAX_BANK_ACCOUNTS];

    //Verify arguments**********************************
    if (argc != 3) {
        printf("Wrong number of arguments\n");
            print_usage(stderr);
        exit(ERR_ARGS);
    }
    
    num_eletronic_counter = atoi(argv[1]);
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
    struct account new_account;
    new_account.ID = ADMIN_ACCOUNT_ID;
    new_account.balance = 0;
    strcpy(new_account.shar256, password);  //TODO depois de fazer a funcao sha256 aqui entra a hash
    accounts_array[0] = new_account;
    printf("ccountIDadmin: %d\n", accounts_array[0].ID);
    //Conta admnistrador criada

    open_slog_file();
    open_ulog_file();

    //Server fifo **********************
    int fd, fd_dummy;

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
//char final [MAX_LINE];
int numread;
char buf[MAX_LINE];
int length;
char str[MAX_LINE];
int  opcode;


  do { 

 numread = read (fd , buf , 1);
buf [ numread ]= '\0';
printf ( " Read from the pipe : %s\n" , buf );
opcode = atoi(buf);
printf("opcode: %d\n", opcode);
buf[0] = 0;

 numread = read (fd , buf , 2);
 buf [ numread ]= '\0';
 printf ( " Read from the pipe : %s\n" , buf );
 length = atoi(buf);
 printf("length: %d\n", length);
 buf[0] = 0;

numread = read (fd , buf , length);
 buf [ numread ]= '\0';
 printf ( " Read from the pipe : %s\n" , buf );
 strcpy(str, buf);
 printf("str: %s\n", str);

buf[0] = '\0';
numread = 0;
 }while (1);







    close(fd);
    close(fd_dummy);
    //Erase FIFO******************************
    if (unlink(SERVER_FIFO_PATH) < 0)
        printf("Error when destroying FIFO '/tmp/requests'\n");
    else
        printf("Server FIFO has been destroyed\n");
    //*************************************** 
/*
    spthread_t request;

    if(pthread_screate(&request, NULL, NULL, NULL) != 0){
        perror("Error creating request threads\n");
        exit(5);
}*/

close_slog_file();
close_ulog_file();
    return 0;
}
