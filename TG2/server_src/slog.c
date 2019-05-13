#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../constants.h"


static int slog_descriptor;

void open_slog_file(){
    const char* fileName = SERVER_LOGFILE;
    slog_descriptor = open(fileName, O_CREAT | O_WRONLY | O_APPEND, 0660);

    if(slog_descriptor < 0){
        // Error opening file
        fprintf(stderr, "Error opening server server file.\n");
        exit(1);
    }
}

/**
  * Closes the server slog log file
  */
void close_slog_file(){
    close(slog_descriptor);
}