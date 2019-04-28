
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "../constants.h"
#include "../types.h"

#define ERR_ARGS 2
#define MAX_LINE_LENGTH 512

typedef unsigned int u_int;


void print_usage(FILE* stream) {
    fprintf(stream, "Usage: ./client <Account ID> <\"Password\"> <Operation Delay (ms)> <Operation Code> <\"Operation Arguments\">\n");
}



int main(int argc, char*argv[]){

    if (argc != 6) {
        print_usage(stderr);
        exit(ERR_ARGS);
    }

    //************Prepare arguments*******************

    req_header_t header;
    //Account ID
    u_int account_number = atoi(argv[1]);
    if (account_number < 1 || account_number > MAX_BANK_ACCOUNTS){
        printf("Error: Invalid Account Number. Use: 1 < Account Number < 4096\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    header.account_id = account_number;
    
    //Password
    if(strlen(argv[2]) < MIN_PASSWORD_LEN || strlen(argv[2]) > MAX_PASSWORD_LEN){
        printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    strcpy(header.password, argv[2]);

    //Delay
    u_int delay = atoi(argv[3]);
    //não de faz qualqeur tipo de verificacao ao delay?????
    header.op_delay_ms = delay;
    
    //Operation Code
    u_int op_code = atoi(argv[4]);
    if(op_code < 0 || op_code > 3){
        printf("Invalid Operation Code. Use 0, 1, 2 or 3.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }                                                                 //op_code has operation code verified
    //struct header finished


    //Operation Arguments
    char op_arguments[MAX_LINE_LENGTH];
    char argList[3][MAX_LINE_LENGTH];
    memcpy(op_arguments, argv[5], strlen(argv[5]) + 1);
    char *ptr;
    ptr = strtok(op_arguments, " ");
    int count = 0;
    while(ptr != NULL ) {
       strcpy(argList[count], ptr);
       ptr = strtok(NULL, " ");
       count++;
    }

    //operation arguments list ready

    req_create_account_t createAccount;
    if(op_code == 0){
        
        if(atoi(argList[0]) < 1 || atoi(argList[0]) > MAX_BANK_ACCOUNTS){
            printf("Invalid Destination Account ID. Use: 1 <= Account ID <= 4096\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
        else if(atoi(argList[1]) < MIN_BALANCE || atoi(argList[1])> MAX_BALANCE){
            printf("Invalid account balance. Use 1 <= Account Balance <= 1000000000\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
        else if(strlen(argList[2]) < MIN_PASSWORD_LEN || strlen(argList[2])> MAX_PASSWORD_LEN){
            printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
        else{
            createAccount.account_id = atoi(argList[0]);
            createAccount.balance = atoi(argList[1]);
            strcpy(createAccount.password, argList[2]);
        }        
    }
    //createAccount struct ready


    req_transfer_t transfer;
    if(op_code == 2){
        if(atoi(argList[0]) < 1 || atoi(argList[0]) > MAX_BANK_ACCOUNTS){
            printf("Invalid Destination Account ID. Use: 1 <= Account ID <= 4096\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
        else if(atoi(argList[1]) < MIN_BALANCE || atoi(argList[1])> MAX_BALANCE){
            printf("Invalid account balance. Use 1 <= Account Balance <= 1000000000\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
        else{
            transfer.account_id = atoi(argList[0]);
            transfer.amount = atoi(argList[1]);
        }
    }
    // transfer struct ready
    //*************************************************



    
    return 0;
}







