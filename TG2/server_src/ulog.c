#include <stdio.h>
#include <fcntl.h>
#include "../constants.h"


static int ulog_descriptor;

void open_ulog_file(){
    const char* fileName = USER_LOGFILE;
    ulog_descriptor = open(fileName, O_CREAT | O_WRONLY | O_APPEND, 0660);

    if(ulog_descriptor < 0){
        // Error opening file
        fprintf(stderr, "Error opening server user file.\n");
        exit(1);
    }
}

/**
  * Closes the server ulog log file
  */
void close_ulog_file(){
    close(ulog_descriptor);
}