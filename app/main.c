#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ztimer.h"
#include "thread.h"
#include "shell.h"
#include "mutex.h"

// Add lps331ap related include here 
#include "lpsxxx.h"
#include "lpsxxx_params.h"

// Declare the lps331ap device variable here 
static lpsxxx_t lpsxxx;
static char test_stack[THREAD_STACKSIZE_MAIN];


//Declare here the struct used to handle the buffer and the mutex 
typedef struct {
    int16_t temperature;
    mutex_t lock;
} data_t;
static data_t data;


static int lpsxxx_handler(void){
    
    uint8_t sum = 0;
    int16_t temp = 0;
    uint8_t x;
    // Reads temperature 10 times
    for(x = 0; x < 10; x++ ){
        lpsxxx_read_temp(&lpsxxx, &temp);
        sum += temp;
    }
    // Calc average
    data.temperature = sum / 10;

    printf("Temperature: %i.%u°C\n", (data.temperature / 100), (data.temperature % 100));
        
    return 0;
}


static void *test_thread(void *arg){

    (void) arg;

    while(1){

        lpsxxx_handler();
        printf("Thread temp: %i.%u°C\n", (data.temperature / 100), (data.temperature % 100));
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }

    return NULL;
}


int main(void){

    // Initialize the lps331ap sensor 
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);

    // Threads init
    thread_create(test_stack, sizeof(test_stack), THREAD_PRIORITY_MAIN - 1,
                  0, test_thread, NULL, "test thread");


    // Start the shell 
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
