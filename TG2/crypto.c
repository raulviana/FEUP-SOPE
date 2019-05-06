
#include <stdlib.h>
#include <time.h>

#include "../constants.h"



void createSalt(char* newSalt){
    int i, n;
    time_t t;
   
    //Intializes random number generator
    srand((unsigned) time(&t));

    /* Print 5 random numbers from 0 to 49 */
    for( i = 0 ; i <= HASH_LEN ; i++ ) {
      rand() % 50;
    }
}