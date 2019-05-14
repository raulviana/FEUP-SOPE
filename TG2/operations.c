

#include "types.h"

#define MAX_LEN_PID 5
#define MAX_LINE 512

void sendMessage(int pid, tlv_reply_t tlv_reply){

    char strPid[MAX_LEN_PID];
    sprintf(strPid, "%d", pid);
    char FIFO_reply_name[USER_FIFO_PATH_LEN];
    strcpy(FIFO_reply_name, USER_FIFO_PATH_PREFIX);
    strcat(FIFO_reply_name, strPid);
    printf("FIFOname: %s\n", FIFO_reply_name);
    

    tlv_reply.length = sizeof(tlv_reply);
    size_t len;
    char strReply[MAX_LINE];
        len = sprintf(strReply, "%d%d|%d|%d", 
            tlv_reply.length,
            tlv_reply.type, 
            tlv_reply.value.header.account_id, 
            tlv_reply.value.header.ret_code);
    
    int code = tlv_reply.type;
    if(tlv_reply.type != 0){
      switch (code){
        case OP_BALANCE:
            len = sprintf(strReply, "%d%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.balance.balance);
            break;
        case OP_TRANSFER:
            len = sprintf(strReply, "%d%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.transfer.balance);
            break;
        case OP_SHUTDOWN:
            len = sprintf(strReply, "%d%d|%d|%d|%d", 
                tlv_reply.length,
                tlv_reply.type, 
                tlv_reply.value.header.account_id, 
                tlv_reply.value.header.ret_code, 
                tlv_reply.value.shutdown.active_offices);
            break;
      }
    }
 printf("length: %ld\n", len);
    printf("strReply: %s\n", strReply);
    strReply[len] = '\0';
    char* str = calloc(1, sizeof *str * len +1);
    strcpy(str, strReply);
printf("str: %s\n", str);
}


void createAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){
    tlv_reply_t tlv_reply;
    bank_account_t new_account;

    new_account.account_id = request.value.create.account_id;
    new_account.balance = request.value.create.balance;
    char newSalt[SALT_LEN + 1];
    createSalt(newSalt);
    strcpy(new_account.salt, newSalt);
    strcpy(new_account.hash, request.value.header.password);  //TODO depois de fazer a funcao sha256 aqui entra a hash
    accounts_array[request.value.create.account_id] = new_account;
    
    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    tlv_reply.value.header.ret_code = RC_OK;

    if(accounts_array[request.value.create.account_id].account_id != 0)
         tlv_reply.value.header.ret_code =RC_ID_IN_USE;

    sendMessage(request.value.header.pid, tlv_reply);
}

void transferAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){

    tlv_reply_t tlv_reply;
    
    accounts_array[request.value.header.account_id].balance -= 
                        request.value.transfer.amount;

    
    accounts_array[request.value.transfer.account_id].balance +=
                        request.value.transfer.amount;

    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    tlv_reply.value.header.ret_code = RC_OK;
    tlv_reply.value.transfer.balance = accounts_array[request.value.transfer.account_id].balance;
    
    if (request.value.header.account_id == request.value.transfer.account_id) 
        tlv_reply.value.header.ret_code = RC_SAME_ID;
    if(accounts_array[request.value.header.account_id].balance < MIN_BALANCE)
        tlv_reply.value.header.ret_code = RC_NO_FUNDS;
    if(accounts_array[request.value.transfer.account_id].balance > MAX_BALANCE)
        tlv_reply.value.header.ret_code = RC_TOO_HIGH;

    sendMessage(request.value.header.pid, tlv_reply);
}

void balanceAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){
    tlv_reply_t tlv_reply;

    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    tlv_reply.value.header.ret_code = RC_OK;
    tlv_reply.value.balance.balance = accounts_array[request.value.header.account_id].balance;

    sendMessage(request.value.header.pid, tlv_reply);
}