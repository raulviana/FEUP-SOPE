
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "../constants.h"
#include "../types.h"

#define ERR_ARGS 2

typedef unsigned int u_int;


void print_usage(FILE* stream) {
    fprintf(stream, "Usage: ./client <Account ID> <\"Password\"> <Operation Delay (ms)> <Operation Code> <\"Operation Arguments\">\n");
}


int main(int argc, char*argv[]){

    if (argc != 6) {
        print_usage(stderr);
        exit(ERR_ARGS);
    }

    //************Verify arguments*******************
    
    //Account ID
    u_int account_number = atoi(argv[1]);
    if (account_number < 1 || account_number > MAX_BANK_ACCOUNTS){
        printf("Error: Invalid Account Number. Use: 1 < Account Number < 4096\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    char account_ID[] = "\0";
    char account_final[WIDTH_ACCOUNT] = "\0";
    strcat(account_ID, argv[1]);
    while((strlen(account_final) + strlen(account_ID)) <= WIDTH_ACCOUNT - 1){
        strcat(account_final, "0");
    }
    strcat(account_final, account_ID);

    //Password
    if(strlen(argv[2]) < MIN_PASSWORD_LEN || strlen(argv[2]) > MAX_PASSWORD_LEN){
        printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    char password[MAX_PASSWORD_LEN] = "\0";
    strcat(password, argv[2]);

    //Delay
    char delay[] = "\0";
    char delay_final[WIDTH_ACCOUNT] = "\0";
    strcat(delay, argv[3]);
    while((strlen(delay_final) + strlen(delay)) <= WIDTH_DELAY - 1){
        strcat(delay_final, "0");
    }
    strcat(delay_final, delay);
    
    //Operation Code
    u_int op_code = atoi(argv[4]);
    if(op_code < 0 || op_code > 3){
        printf("Invalid Operation Code. Use 0, 1, 2 or 3.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }

    //Operation Arguments
    char op_arguments[] = "\0";
    strcpy(op_arguments, argv[5]);
    //funcao para partir string nos diferentes argumentos

    //*************************************************
    return 0;
}







