#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "xtimer.h"
#include "thread.h"
#include "shell.h"
#include "mutex.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"
#include "msg.h"
#include "net/gcoap.h"
#include "fmt.h"
#include "gcoap_cli.h"

// The lps331ap device variable and stack
static lpsxxx_t lpsxxx;
static char lpsxxx_read_stack[THREAD_STACKSIZE_MAIN];
static char lpsxxx_sniffer_stack[THREAD_STACKSIZE_MAIN];

// Thread pid variable
static kernel_pid_t main_pid, lpsxxx_read_pid, lpsxxx_sniffer_pid;

//The struct used to handle the temperature and mutex
typedef struct {
    uint16_t temperature;
    int16_t temperature_last_sent;
    mutex_t lock;
} data_t;

static data_t data;

// Variables for thread sleep duration
// When testing the project smaller variable values are recommended for example 30
uint32_t lpsxxx_sleep = 600;
uint32_t lpsxxx_sniffer_sleep = 600;

// Message queue for main thread
#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];


/**
 * @brief Handles temperature readings from the LPSXXX sensor.
 *
 * This function reads the temperature from the LPSXXX sensor 10 times, calculates
 * the average, and stores the result in the global 'data.temperature' variable.
 *
 * @return 0 on success, -1 on failure.
 */
static int lpsxxx_handler(void){
    
    uint32_t sum = 0;
    int16_t temp = 0;
    uint8_t x;
    // Reads temperature 10 times
    for(x = 0; x < 10; x++ ){
        if (lpsxxx_read_temp(&lpsxxx, &temp) != LPSXXX_OK) {
            // Error reading temperature
            return -1;
        }
        sum += temp;
    }
    // Calculate average
    sum = sum / 10;
    data.temperature = sum;
        
    return 0;
}


/**
 * @brief Compare tenths place of current temperature with the last sent temperature.
 *
 * This function extracts the tens place from both the current temperature
 * and the last sent temperature. It then compares the tenths places to determine
 * if there has been a significant change.
 *
 * @return 1 if the tens place has changed, 0 otherwise.
 */
static int compareTo_LastSent(void){

    // Extract the tens place from data.temperature_last_sent
    int16_t last_sent_tens = (data.temperature_last_sent / 10) % 10;

    // Extract the tens place from data.temperature
    int16_t current_tens = (data.temperature / 10) % 10;

    // Check if the tens place has changed
    if (current_tens != last_sent_tens) {
        return 1; // Value has changed
    } else {
        return 0; // Value has not changed
    }
}


/**
 * @brief Thread function for reading data from the LPSXXX sensor.
 *
 * This thread continuously reads data from the LPSXXX sensor, performs
 * some processing (e.g., using lpsxxx_handler()), and then sleeps for a
 * specified duration before repeating the process.
 *
 * @param arg Unused argument (may be NULL).
 * @return NULL (not used in this context).
 */
static void *lpsxxx_read_thread(void *arg){

    // Unused parameter to avoid compiler warnings
    (void) arg;

    // Infinite loop for continuous sensor reading
    while(1){

        // Acquire the data lock to prevent data corruption during access
        mutex_lock(&data.lock);
        // Call the lpsxxx_handler() function to read sensor data
        if(lpsxxx_handler() != 0){
            printf("lpsxxx_handler failed!");
        }
        printf("Temperature: %i.%u °C\n", (data.temperature / 100), (data.temperature % 100));  
        // Release the data lock after processing
        mutex_unlock(&data.lock);

        // Sleep for a specified duration before the next iteration
        xtimer_sleep(lpsxxx_sleep);
    }
    // This return statement is not reached in this context
    return NULL;
}


/**
 * @brief Thread function for monitoring changes in the temperature and detecting updates.
 *
 * This thread monitors changes in the temperature data by periodically comparing
 * the current temperature with the last sent temperature. if a significant change is detected, 
 * sends the updated temperature data to a server.
 *
 * @param arg Unused argument (may be NULL).
 * @return NULL (not used in this context).
 */
static void *lpsxxx_sniffer_thread(void *arg) {

    // Unused parameter to avoid compiler warnings
    (void)arg;

    // Shift sniffer_thread's while loop to start 5s behind read_thread
    xtimer_sleep(5);

    // Infinite loop for continuous monitoring
    while (1) {
        // Sleep for a specified duration before the next iteration
        xtimer_sleep(lpsxxx_sniffer_sleep);

        // Acquire the data lock to prevent data corruption during access
        mutex_lock(&data.lock);

        // Compare the current temperature with the last sent temperature
        if (compareTo_LastSent() == 1) {
            printf("Last sent value has changed from %i.%u °C to %i.%u °C !\n", (data.temperature_last_sent / 100), (data.temperature_last_sent % 100), (data.temperature / 100), (data.temperature % 100));  

            // Sending data to server
            char response[5];
            char resource[] = "/temperature";
            sprintf(response, "%i.%u", (data.temperature / 100), (data.temperature % 100));

            gcoap_cli_send("put", &response[0], &resource[0]);
            data.temperature_last_sent = data.temperature;
        } else {
            printf("Value has not changed!\n");
        }
        // Release the data lock after processing
        mutex_unlock(&data.lock);
    }
    // This return statement is not reached in this context
    return NULL;
}


/**
 * @brief Prints the startup information for the program.
 */
void printStartupInfo(void) {

    printf("==================================================================\n");
    printf("                     Internet of Things 2023\n");                   
    printf("                          Miniproject 1\n");
    printf("------------------------------------------------------------------\n");
    printf("                            Welcome!\n\n");
    printf("This program reads temperature from IoT-Lab M3 MCU and sends the\n");
    printf("data to backend cloud server using CoAP protocol\n");
    printf("------------------------------------------------------------------\n");
    printf("Documentation: https://github.com/matluuk/IoT-miniproject-1\n");
    printf("Developed by: Matti Luukkonen, Touko Kinnunen and Hermanni Hanhela\n");
    printf("==================================================================\n");
}


/**
 * @brief Main function for the application.
 *
 * This function initializes the necessary components, creates threads for
 * lpsxxx sensor reading and monitoring.
 *
 * @return 0 on successful execution (Note: The return statement is not used in this context).
 */
int main(void) {

    printStartupInfo();

    // Initialize the xtimer module
    xtimer_init();

    // Get the main thread pid
    main_pid = thread_getpid();

    // Initialize the lps331ap sensor
    lpsxxx_init(&lpsxxx, &lpsxxx_params[0]);

    // Lpsxxx reader thread initialization
    lpsxxx_read_pid = thread_create(lpsxxx_read_stack,
                    sizeof(lpsxxx_read_stack),
                    THREAD_PRIORITY_MAIN - 1,
                    0,
                    lpsxxx_read_thread,
                    NULL,
                    "lpsxxx reader thread");

    // Lpsxxx sniffer thread initialization
    lpsxxx_sniffer_pid = thread_create(lpsxxx_sniffer_stack,
                    sizeof(lpsxxx_sniffer_stack),
                    THREAD_PRIORITY_MAIN - 2,
                    0,
                    lpsxxx_sniffer_thread,
                    NULL,
                    "lpsxxx sniffer thread");

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    // Start the shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    // The return statement is not used in this context
    return 0;
}
