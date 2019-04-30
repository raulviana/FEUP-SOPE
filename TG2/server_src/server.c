#include "../constants.h"
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
#include <sys/stat.h>
#include <sys/file.h>

int main(int argc, char*argv[]){

    req_create_account_t createAccount;

    if (argc != 3) {
        printf("Wrong number of arguments\n");
        exit(-1);
    }
    
    int num_eletronic_counter = atoi(argv[1]);

    if(num_eletronic_counter >  MAX_BANK_OFFICES  || num_eletronic_counter == 0) {
        fprintf(stderr, "Invalid value for num_eletronic_counter, must be non zero positive value\n");
        exit(-2);
    }

    const char password = strlen(argv[2]);
    if(password < MIN_PASSWORD_LEN || password > MAX_PASSWORD_LEN){
        printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
        exit(-3);
       }
    
    createAccount.account_id = 0;
    createAccount.balance = 0;
    strcpy(createAccount.password, argv[2]);
        
    printf("server \n");



    return 0;

}