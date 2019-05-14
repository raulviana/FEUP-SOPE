

#include "types.h"




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

printf("replybalance: %d\n", tlv_reply.value.header.account_id);

    ////create and send message
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

printf("balance: %d\n", tlv_reply.value.header.ret_code );

        //create and send message
}

void balanceAccount(tlv_request_t request, bank_account_t accounts_array[MAIN_THREAD_ID]){
    tlv_reply_t tlv_reply;

    tlv_reply.type = request.type;
    tlv_reply.value.header.account_id = request.value.header.account_id;
    tlv_reply.value.header.ret_code = RC_OK;
    tlv_reply.value.balance.balance = accounts_array[request.value.header.account_id].balance;

    printf("replybalance: %d\n", tlv_reply.value.balance.balance );

    //create and send message
}