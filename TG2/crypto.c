
#include <stdlib.h>
#include <time.h>

#include "constants.h"



void createSalt(char* newSalt){
    int min_Ascci = 48;
    int i = 0, n;
    time_t t;
   
    //Intializes random number generator
    srand((unsigned) time(&t));

    while(strlen(newSalt) < HASH_LEN){
      n = (rand() % 75) + min_Ascci; //n está entre 48 e 122
      if ((n > 57 && n < 65) || (n > 90 && n < 97)) continue; //  ! [0..9][a..z][A..Z]
      else {
        newSalt[i] = n;
      }
      newSalt[i + 1] = '\0';
      i++;
    }
}

int isPassOk(char passSalted){
    return 0;
}

