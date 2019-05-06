#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "../constants.h"
#include "../types.h"
#include <sys/file.h>

#define ERR_ARGS 1
#define ERR_FIFO 2

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./server <Number of Eletronic Bank Officces> <\"Admin Password\">\n");
}

int main(int argc, char*argv[]){

    //***************************Verify arguments**********************************
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
    //*************************************************************************************



    //****************Server fifo **********************
    int fd;
    if (mkfifo(SERVER_FIFO_PATH,0660)<0)
 if (errno==EEXIST) printf("FIFO '/tmp/requests' already exists\n");
 else printf("Can't create FIFO\n");
else printf("FIFO '/tmp/requests' sucessfully created\n");
if ((fd=open(SERVER_FIFO_PATH,O_RDONLY)) !=-1)
 printf("FIFO '/tmp/requests' openned in READONLY mode\n"); 
    //************************************************************









    //*******************Erase FIFO******************************
    if (unlink(SERVER_FIFO_PATH) < 0)
        printf("Error when destroying FIFO '/tmp/requests'\n");
    else
        printf("FIFO '/tmp/requests' has been destroyed\n");
    //********************************************* 


    return 0;

}