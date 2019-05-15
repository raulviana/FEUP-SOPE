
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <signal.h>
#include "../constants.h"
#include "../types.h"
#include "../log.c"


#define ERR_ARGS 2
#define ERR_FIFO 3
#define SERVER_FIFO_ERROR 4
#define MESSAGE_SENT_ERROR 5

#define MAX_LINE_LENGTH 512
#define WIDTH_FIFO_NAME 16

typedef unsigned int u_int;

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: ./client <Account ID> <\"Password\"> <Operation Delay (ms)> <Operation Code> <\"Operation Arguments\">\n");
}

void *send_message (tlv_request_t message_send){
   
   char tempstr[MAX_LINE_LENGTH];
   message_send.length = sizeof(message_send);
   size_t len;

   if(message_send.type == OP_BALANCE || message_send.type == OP_SHUTDOWN){
       len = snprintf(tempstr, MAX_LINE_LENGTH, "%d%d%d|%d|%s|%d|",
                        message_send.type, message_send.length, message_send.value.header.pid,
                        message_send.value.header.account_id,message_send.value.header.password,
                        message_send.value.header.op_delay_ms);
    }
    else{
        if(message_send.type == OP_CREATE_ACCOUNT){
            len = snprintf(tempstr, MAX_LINE_LENGTH, "%d%d%d|%d|%s|%d|%d|%d|%s",
                        message_send.type, message_send.length, message_send.value.header.pid,
                        message_send.value.header.account_id,message_send.value.header.password,
                        message_send.value.header.op_delay_ms, message_send.value.create.account_id,
                        message_send.value.create.balance, message_send.value.create.password);
        }
        else{
            len = snprintf(tempstr, MAX_LINE_LENGTH, "%d%d%d|%d|%s|%d|%d|%d",
                        message_send.type, message_send.length, message_send.value.header.pid,
                        message_send.value.header.account_id,message_send.value.header.password,
                        message_send.value.header.op_delay_ms, message_send.value.transfer.account_id,
                        message_send.value.transfer.amount);
        }
    }
    
    message_send.length = len;

    tempstr[len] = '\0';
    char* str = calloc(1, sizeof *str * len +1);
    strcpy(str, tempstr);
    int fd;
    if ((fd = open(SERVER_FIFO_PATH, O_WRONLY | O_CREAT | O_APPEND, 0660)) < 0){
        printf("Failed to open server requests FIFO\n");
        exit(SERVER_FIFO_ERROR);
    } 
    else {
        printf("Open Successfully");
    }

    int write_result = write(fd, str, message_send.length);
    if (write_result < 0){
        printf("Error: Failed to send message to server FIFO\n");
        exit(MESSAGE_SENT_ERROR);
    }
    close(fd);
}

void alarm_hanlder(){
    printf(" timeout, no response from server ");
    //TODO write in log
    exit(0);
}
void setup_alarm(){
    signal(SIGALRM, alarm_hanlder);
}

void receive_message(char *strPid){
int fd, fd_dummy;

    if (mkfifo(SERVER_FIFO_PATH,0660)<0)
        if (errno==EEXIST) printf("Inclient Reply FIFO already exists\n");
        else {
            printf("Can't create Reply FIFO\n");
            print_usage(stderr);
            exit(ERR_FIFO);
        }
    else printf("In client Reply FIFO sucessfully created\n");
     if ((fd=open(SERVER_FIFO_PATH,O_RDONLY)) !=-1)
        printf("Reply FIFO created in READONLY mode\n"); 
    if ((fd_dummy=open(SERVER_FIFO_PATH,O_WRONLY)) !=-1)
      printf("Reply FIFO openned in WRITEONLY mode\n"); 

    char buffer[MAX_LINE_LENGTH];
    int read_number;
    read_number = read(fd, buffer, MAX_LINE_LENGTH);
    buffer[read_number] = '\0';
    printf("buf: %s\n", buffer);

    close(fd);
    close(fd_dummy);



}


int main(int argc, char *argv[]) {

    if (argc != 6)
    {
        print_usage(stderr);
        exit(ERR_ARGS);
    }

    //************Prepare arguments*******************

    req_header_t header;
    req_value_t value;
    req_create_account_t createAccount;
    req_transfer_t transfer;
    tlv_request_t message_send;

    //PID
    header.pid = getpid();
    char strPid[WIDTH_ID];
    sprintf(strPid, "%d", getpid());

    //Account ID
    u_int account_number = atoi(argv[1]);
    if (account_number < 0 || account_number > MAX_BANK_ACCOUNTS)
    {
        printf("Error: Invalid Account Number. Use: 0 < Account Number < 4096\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    header.account_id = account_number;

    //Password
    if (strlen(argv[2]) < MIN_PASSWORD_LEN || strlen(argv[2]) > MAX_PASSWORD_LEN)
    {
        printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    strcpy(header.password, argv[2]);

    //Delay
    u_int delay = atoi(argv[3]);
    if (delay > MAX_OP_DELAY_MS || delay <= 0)
    {
        printf("Error: Invalid delay. Use delay between 0 and 99999.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    header.op_delay_ms = delay;

    /*Operation Code
    * 0 - Account creation: "AccountID balance "password"" 
    * 1 - Balance consultation: no extra arguments
    * 2 - Between Accounts Transfer: "destinationAccountID value"
    * 3 - Server ShutDown: no extra arguments
    */
    int op_code = atoi(argv[4]);
    if (op_code < 0 || op_code >= __OP_MAX_NUMBER)
    {
        printf("Invalid Operation Code. Use 0, 1, 2 or 3.\n");
        print_usage(stderr);
        exit(ERR_ARGS);
    }
    //struct header is finished

    //Operation Arguments
    char op_arguments[MAX_LINE_LENGTH];
    char argList[3][MAX_LINE_LENGTH];
    memcpy(op_arguments, argv[5], strlen(argv[5]) + 1);
    char *ptr;
    ptr = strtok(op_arguments, " ");
    int count = 0;
    while (ptr != NULL)
    {
        strcpy(argList[count], ptr);
        ptr = strtok(NULL, " ");
        count++;
    }
    //operation arguments list is ready


    switch(op_code){
        case(OP_BALANCE):
            value.header = header;
            break;
        case(OP_SHUTDOWN):
            value.header = header;
            break;
        case(OP_CREATE_ACCOUNT):
          if (atoi(argList[0]) < 1 || atoi(argList[0]) > MAX_BANK_ACCOUNTS){
            printf("Invalid Destination Account ID. Use: 1 <= Account ID <= 4096\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
          else if (atoi(argList[1]) < MIN_BALANCE || atoi(argList[1]) > MAX_BALANCE){
            printf("Invalid account balance. Use 1 <= Account Balance <= 1000000000\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
          else if (strlen(argList[2]) < MIN_PASSWORD_LEN || strlen(argList[2]) > MAX_PASSWORD_LEN){
            printf("Error: Invalid Password. Must have more than 8 and less than 20 characters.\n");
            print_usage(stderr);
            exit(ERR_ARGS);
        }
          else{
            value.header = header;
            createAccount.account_id = atoi(argList[0]);
            createAccount.balance = atoi(argList[1]);
            strcpy(createAccount.password, argList[2]);
            value.create = createAccount;
        }//createAccount header struct is ready
        case(OP_TRANSFER):
          if (atoi(argList[0]) < 1 || atoi(argList[0]) > MAX_BANK_ACCOUNTS){
            printf("Invalid Destination Account ID. Use: 1 <= Account ID <= 4096\n");
            print_usage(stderr);
            exit(ERR_ARGS);
          }
          else if (atoi(argList[1]) < MIN_BALANCE || atoi(argList[1]) > MAX_BALANCE){
            printf("Invalid account balance. Use 1 <= Account Balance <= 1000000000\n");
            print_usage(stderr);
            exit(ERR_ARGS);
          }
        else{
            value.header = header;
            transfer.account_id = atoi(argList[0]);
            transfer.amount = atoi(argList[1]);
            value.transfer = transfer;
        }// transfer header struct ready
    }

    message_send.value = value;
    message_send.type = op_code;
    message_send.length = sizeof(message_send);
    //message TLV is ready to be sent!

    //*************************************************


    send_message(message_send);

    receive_message(strPid);

return 0;
    
}
