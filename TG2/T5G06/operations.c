
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "types.h"
#include "constants.h"
#include "log.c"


#define MAX_LEN_PID 5
#define MAX_LINE 512

void sendMessage(int pid, tlv_reply_t tlv_reply){

    char strPid[WIDTH_ID];
    char FIFO_reply_name[USER_FIFO_PATH_LEN];
    sprintf(strPid, "%d", pid);
    sprintf(FIFO_reply_name, "%s%d", USER_FIFO_PATH_PREFIX, pid);
     
    tlv_reply.length = sizeof(tlv_reply);
    size_t len;
    char strReply[MAX_LINE];
        len = sprintf(strReply, "%d|%d|%d|%d", 
            tlv_reply.length,
            tlv_reply.type, 
            tlv_reply.value.header.account_id, 
            tlv_reply.value.header.ret_code);
    
    int code = tlv_reply.type;
    if(tlv_reply.type != 0){
      switch (code){
        case OP_BALANCE:
            len = sprintf(strReply, "%d|%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.balance.balance);
            break;
        case OP_TRANSFER:
            len = sprintf(strReply, "%d|%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.transfer.balance);
            break;
        case OP_SHUTDOWN:
            len = sprintf(strReply, "%d|%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.shutdown.active_offices);
            break;
        }
    }
    tlv_reply.length = len;
 
    strReply[len] = '\0';
    char* reply_str = calloc(1, sizeof *reply_str * len +1);
    strcpy(reply_str, strReply);


int fd;
if ((fd = open(FIFO_reply_name, O_WRONLY | O_CREAT | O_APPEND, 0660)) < 0){
        printf("Failed to open server requests FIFO\n");
        exit(1);
    } 
    else {
        printf(" ");
    }

    int write_result = write(fd, reply_str, tlv_reply.length);
    if (write_result < 0){
        printf("Error: Failed to send message to server FIFO\n");
        exit(2);
    }
    close(fd);



    int slog = open(SERVER_LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0664);

    if(slog < 0){
        fprintf(stderr, "Error opening SERVER log file.\n");
        exit(1);
    }
 
 int saved_stdout = dup(STDOUT_FILENO);
 dup2(slog,STDOUT_FILENO); 

    const tlv_reply_t* tlv_reply_ptr;
    tlv_reply_ptr = &tlv_reply;
    logReply(fd, getpid(), tlv_reply_ptr);

    close(slog);

     dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}


void createAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){
    tlv_reply_t tlv_reply;
    bank_account_t new_account;
    int ok = 0;
    usleep(request.value.header.op_delay_ms*1000);

    if(accounts_array[request.value.create.account_id].in_use == IN_USE){
         tlv_reply.value.header.ret_code =RC_ID_IN_USE;
         ok = -1;
    }
    if(request.value.header.account_id != 0) {
        tlv_reply.value.header.ret_code = RC_OP_NALLOW;
        ok = -1;
    }
    if(checkPassword(request.value.header.password, 
                     accounts_array[ADMIN_ACCOUNT_ID].salt,
                     accounts_array[ADMIN_ACCOUNT_ID]) == false){
                         tlv_reply.value.header.ret_code = RC_LOGIN_FAIL;
                         ok = -1;
    }
    if(ok == 0){
    new_account.account_id = request.value.create.account_id;
    new_account.balance = request.value.create.balance;
    char newSalt[SALT_LEN + 1];
    createSalt(newSalt);
    strcpy(new_account.salt, newSalt);
    strcpy(new_account.hash, getSha256(request.value.header.password, newSalt));
    new_account.in_use = IN_USE;
    accounts_array[request.value.create.account_id] = new_account;
    tlv_reply.value.header.ret_code = RC_OK;
    }

    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;

    sendMessage(request.value.header.pid, tlv_reply);
}

tlv_reply_t transferAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){

    tlv_reply_t tlv_reply;
    int ok = 0;
    usleep(request.value.header.op_delay_ms*1000);

    if(accounts_array[request.value.transfer.account_id].balance > MAX_BALANCE){
        tlv_reply.value.header.ret_code = RC_TOO_HIGH;
        ok = -1;
    }
    if(accounts_array[request.value.header.account_id].balance < MIN_BALANCE){
        tlv_reply.value.header.ret_code = RC_NO_FUNDS;
        ok = -1;
    }
    if (request.value.header.account_id == request.value.transfer.account_id){
        tlv_reply.value.header.ret_code = RC_SAME_ID;
        ok = -1;
    } 
    if(request.value.header.account_id == 0) {
        tlv_reply.value.header.ret_code = RC_OP_NALLOW;
        ok = -1;
    }
    if(checkPassword(request.value.header.password, 
                     accounts_array[request.value.header.account_id].salt,
                     accounts_array[request.value.header.account_id]) == false) {
        tlv_reply.value.header.ret_code = RC_LOGIN_FAIL;
        ok = -1;
    }
 
    if (ok == 0){
        accounts_array[request.value.header.account_id].balance -= 
                        request.value.transfer.amount;
    
    accounts_array[request.value.transfer.account_id].balance +=
                        request.value.transfer.amount;
    
    tlv_reply.value.header.ret_code = RC_OK;
    }

    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    
    tlv_reply.value.transfer.balance = accounts_array[request.value.transfer.account_id].balance;
    
    sendMessage(request.value.header.pid, tlv_reply);

    return tlv_reply;
}

tlv_reply_t balanceAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){
    tlv_reply_t tlv_reply;
    
    usleep(request.value.header.op_delay_ms*1000);

    if(checkPassword(request.value.header.password, 
                     accounts_array[request.value.header.account_id].salt,
                     accounts_array[request.value.header.account_id]) == false){
         tlv_reply.value.header.ret_code = RC_LOGIN_FAIL;
    }
    else{
    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    tlv_reply.value.header.ret_code = RC_OK;
    tlv_reply.value.balance.balance = accounts_array[request.value.header.account_id].balance;
    }

    sendMessage(request.value.header.pid, tlv_reply);

    return tlv_reply;
}
