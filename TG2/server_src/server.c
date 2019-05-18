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
#include "../operations.c"

#define ERR_ARGS 1
#define ERR_FIFO 2

#define MAX_LINE 512
#define MAX_ARGUMENTS 7
#define ADMIN_BALANCE 0

int num_eletronic_counter;
int slog;

sem_t s1, s2;
int semV1, semV2;
pthread_t *threads;
pthread_mutex_t mutexI = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;
tlv_request_t request;
bank_account_t accounts_array[MAX_BANK_ACCOUNTS];

tlv_request_t buffer[MAX_BANK_ACCOUNTS];

bool run = 1;

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./server <Number of Eletronic Bank Officces> <\"Admin Password\">\n");
}

tlv_request_t remakeTLV(int opcode, int length, char str[], tlv_request_t request){
    char list[MAX_ARGUMENTS][MAX_LINE];
    int count = 0;
    char delim[] = "|";
	char *ptr = strtok(str, delim);

	while(ptr != NULL){
		strcpy(list[count], ptr);
		ptr = strtok(NULL, delim);
        count++;
	}

    request.type = opcode;
    request.length = length;
    request.value.header.pid = atoi(list[0]);
    request.value.header.account_id = atoi(list[1]);
    strcpy(request.value.header.password, list[2]);
    request.value.header.op_delay_ms = atoi(list[3]);

    if(request.type == OP_CREATE_ACCOUNT){
        request.value.create.account_id = atoi(list[4]);
        request.value.create.balance = atoi(list[5]);
        strcpy(request.value.create.password, list[6]);
    }
    if(request.type == OP_TRANSFER) {
        request.value.transfer.account_id = atoi(list[4]);
        request.value.transfer.amount = atoi(list[5]);
    } 
    return request;
}

void shutdown(){

    chmod(SERVER_FIFO_PATH, S_IRUSR|S_IRGRP|S_IROTH);

    pthread_mutex_destroy(&mutexI);
<<<<<<< HEAD
    //sem_close(&sem1);
    //sem_close(&sem2);
=======
    sem_close(&s1);
    sem_close(&s2);
>>>>>>> 918a4216b62160851af323ea5c4fed86b684bd0c
    run = 0;
}

void *wRequest(void *n) {

    logBankOfficeOpen(STDOUT_FILENO, *(int *) n, pthread_self());

    tlv_reply_t tlv_reply;
    sem_getvalue(&s2, &semV1);
    logSyncMechSem(STDOUT_FILENO, *(int *) n, SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, 0, semV1);
 //   sem_wait(&s2);
 
 
 //   pthread_mutex_lock(&mutexI);

    
 //   pthread_mutex_unlock(&mutexI); 
    
    switch(request.type){

        //Criação de Conta
        case OP_CREATE_ACCOUNT:
        printf("CREATE\n");
            pthread_mutex_lock(&mutexI);
            createAccount(request, accounts_array);        
            pthread_mutex_unlock(&mutexI);
            logAccountCreation(STDOUT_FILENO, *(int *) n, accounts_array);

            break;

        //Consulta de Saldo
        case OP_BALANCE:
        printf("BALANCE\n");
        tlv_reply=balanceAccount(request, accounts_array);
        
            break;
        //Transferência
        case OP_TRANSFER:
        printf("TRANSFER\n");
        tlv_reply=transferAccount(request, accounts_array);
            break;

        //Encerramentodo Servidor
        case OP_SHUTDOWN:
        printf("SHUTDOWN\n");
            shutdown();
            break;
        default:
            break;

    }


    logReply(STDOUT_FILENO, *(int *)n, &tlv_reply);
 
        sem_post(&s1);
        sem_getvalue(&s1, &semV2);
        logSyncMechSem(STDOUT_FILENO, *(int *) n, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, request.value.header.pid, semV2);
    

    logBankOfficeClose(STDOUT_FILENO, *(int *) n, pthread_self());
printf("RUN: %d\n",run);
    return NULL;
}



int main(int argc, char*argv[]){

    //******************Set Up Server******************************
    
 
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
    bank_account_t new_account;
    new_account.account_id = ADMIN_ACCOUNT_ID;
    new_account.balance = ADMIN_BALANCE;
    char newSalt[SALT_LEN + 1];
    createSalt(newSalt);
    strcpy(new_account.salt, newSalt);
    strcpy(new_account.hash, getSha256(password,newSalt));  //depois de fazer a funcao sha256 aqui entra a hash
    new_account.in_use = IN_USE;
    
    //Conta admnistrador criada

    slog = open(SERVER_LOGFILE, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0664);

    if(slog < 0){
        fprintf(stderr, "Error opening server log file.\n");
        exit(1);
    }

    pthread_t threads[num_eletronic_counter];
    int threads_num[num_eletronic_counter];

    sem_init(&s1, 0, num_eletronic_counter);
    sem_getvalue(&s1, &semV1);
    logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, 0, semV2);

   // for (int i = 1; i <= num_eletronic_counter; i++) {
    //    printf("HI");
    //    threads_num[i-1] = i;
      //  pthread_create(&threads[i-1], NULL, wRequest, (void *) &threads_num[i-1]);
//}
    

    logSyncMech(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
    pthread_mutex_lock(&mutexI);
    logDelay(STDOUT_FILENO, MAIN_THREAD_ID, 0);

    accounts_array[ADMIN_ACCOUNT_ID] = new_account;

    logAccountCreation(STDOUT_FILENO, MAIN_THREAD_ID, &accounts_array[ADMIN_ACCOUNT_ID]);

    pthread_mutex_unlock(&mutexI);
    logSyncMech(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, 0);
   

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

int numread;
char buf[MAX_LINE];
int length;
char str[MAX_LINE];
int  opcode;


while(run) {
sem_getvalue(&s1, &semV1);
logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request.value.header.pid, semV1);
sem_wait(&s1);
logRequest(STDOUT_FILENO, request.value.header.pid, &request);

numread = read (fd , buf , 1);
buf [ numread ]= '\0';
opcode = atoi(buf);               //preenche opcode
buf[0] = 0;

numread = read (fd , buf , 2);
buf [ numread ]= '\0';
length = atoi(buf);             //preenche length, para saber quanto ler no proximo read
buf[0] = 0;

numread = read (fd , buf , length);
buf [ numread ]= '\0';
strcpy(str, buf);               //str contém o resto da mensagem
 //re-constroi a mensagem TLV
request = remakeTLV(opcode, length, str, request);

   sem_post(&s2);
   sem_getvalue(&s2,&semV2);
   logSyncMechSem(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request.value.header.pid, semV2);

//pthread_create(&threads[numread], NULL, wRequest, (void *) &threads_num[numread]);

//TODO colocar mensagem num thread e ir à conta correspondente


buf[0] = '\0';
numread = 0;

pthread_create(&threads[numread], NULL, wRequest, (void *) &threads_num[numread]);   
printf("OIasssssss\n");

if(opcode == OP_SHUTDOWN) break; //TODO sai do loop e processa o fecho do servidor
}
    printf("OI\n");


close(fd);
close(fd_dummy);
//Erase FIFO******************************
if (unlink(SERVER_FIFO_PATH) < 0)
    printf("Error when destroying FIFO '/tmp/requests'\n");
else
    printf("Server FIFO has been destroyed\n");
//*************************************** 

    logSyncMech(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
    //printf(syncM);
 //   pthread_mutex_lock(&mutexI);
    logDelay(STDOUT_FILENO, MAIN_THREAD_ID, 0);

  //  pthread_mutex_unlock(&mutexI);


   for(int i = 0; i < num_eletronic_counter; i++){
        pthread_join(threads[i],NULL);
    }

    close(slog);

    return 0;

}

