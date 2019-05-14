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
#include "../log.c" 

#define ERR_ARGS 1
#define ERR_FIFO 2

#define MAX_LINE 512
#define MAX_ARGUMENTS 7
#define ADMIN_BALANCE 0

int num_eletronic_counter;
int slog;

sem_t *sem1, *sem2;
pthread_t *threads;
pthread_mutex_t mutexI = PTHREAD_MUTEX_INITIALIZER;
tlv_request_t request;
bank_account_t accounts_array[MAX_BANK_ACCOUNTS];

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./server <Number of Eletronic Bank Officces> <\"Admin Password\">\n");
}
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
    sem_close(sem1);
    sem_close(sem2);
}
void *wRequest(void *n) {
    
    char mess[50];
   
    usleep(request.value.header.op_delay_ms * 1000);

    sprintf(mess, "-OPEN%i\n", *(int *) n);
    write(slog, mess, strlen(mess) );
   
    sem_wait(sem2);
    while(1){
    pthread_mutex_lock(&mutexI);
    if (sem_trywait(sem2) == -1){
        if (errno == EAGAIN){
            pthread_mutex_unlock(&mutexI);
            continue;
        }
    }
    
    pthread_mutex_unlock(&mutexI); 
    
    switch(request.type){

        //Criação de Conta
        case OP_CREATE_ACCOUNT:
            pthread_mutex_lock(&mutexI);
            //
            //logAccountCreation(int fd, int id, const bank_account_t *account);
            //
            pthread_mutex_unlock(&mutexI);
            break;

        //Consulta de Saldo
        case OP_BALANCE:
            pthread_mutex_lock(&mutexI);
            //....
            pthread_mutex_unlock(&mutexI);
            break;
        //Transferência
        case OP_TRANSFER:

            break;

        //Encerramentodo Servidor
        case OP_SHUTDOWN:
            shutdown();
            break;
        default:
            break;

    }

    
    pthread_mutex_unlock(&mutexI); 
    
    //printf("Thread");

   
    }
    sprintf(mess, "CLOSE: %i\n", *(int *) n);
    write(slog, mess, strlen(mess));


    sem_post(sem1);
    //logReply(STDOUT_FILENO, *(int *)n, &request);

    //logBankOfficeClose(STDOUT_FILENO, *(int *) n, pthread_self());
    pthread_exit(NULL);
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
    strcpy(new_account.hash, password);  //TODO depois de fazer a funcao sha256 aqui entra a hash
    accounts_array[ADMIN_ACCOUNT_ID] = new_account;
    //Conta admnistrador criada

    //open_slog_file(); //**Só comentei para poder correr..
    //open_ulog_file();

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

do {
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

request = remakeTLV(opcode, length, str, request);    //re-constroi a mensagem TLV
//TODO colocar mensagem num thread e ir à conta correspondente
if (request.type== OP_TRANSFER) transferAccount( request, &accounts_array[MAIN_THREAD_ID]);
if(request.type == OP_BALANCE) balanceAccount( request, &accounts_array[MAIN_THREAD_ID]);
if(request.type == OP_CREATE_ACCOUNT) createAccount( request, &accounts_array[MAIN_THREAD_ID]);

buf[0] = '\0';
numread = 0;
if(opcode == OP_SHUTDOWN) break; //TODO sai do loop e processa o fecho do servidor
}while (1);



close(fd);
close(fd_dummy);
//Erase FIFO******************************
if (unlink(SERVER_FIFO_PATH) < 0)
    printf("Error when destroying FIFO '/tmp/requests'\n");
else
    printf("Server FIFO has been destroyed\n");
//*************************************** 

    pthread_t temp[num_eletronic_counter];
    threads = temp;
    int threads_num[num_eletronic_counter];

     slog = open(SERVER_LOGFILE, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0664);

    if(slog < 0){
        fprintf(stderr, "Error opening server log file.\n");
        exit(1);
    }
    int ulog = open(USER_LOGFILE, O_WRONLY | O_TRUNC);

    if(ulog < 0){
        fprintf(stderr, "Error opening USER log file.\n");
        exit(1);
    }

    for (int i = 1; i <= num_eletronic_counter; i++) {
        threads_num[i-1] = i;
        pthread_create(&threads[i-1], NULL, wRequest, (void *) &threads_num[i-1]);
    }

    logSyncMech(STDOUT_FILENO, MAIN_THREAD_ID, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, 0);
    //printf(syncM);
    pthread_mutex_lock(&mutexI);
    logDelay(STDOUT_FILENO, MAIN_THREAD_ID, 0);

    pthread_mutex_unlock(&mutexI);

    for(int i = 0; i < num_eletronic_counter; i++){
        pthread_join(temp[i],NULL);
    }


    close(slog);
    close(ulog);

    return 0;

}

