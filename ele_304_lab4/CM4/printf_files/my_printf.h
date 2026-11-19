/**
 * @file   my_printf.c
 * @author Ken Henry Andersen
 * @brief  Header file of MyPrintf module.
 * @date   2023-08-08
 */

#ifndef MY_PRINTF_H_
#define MY_PRINTF_H_

#include <stdbool.h>
#include "cmsis_os.h" /* CMSIS RTOS header file */
#include "main.h"

/**
 * @brief Define print buffer size, i.e. maximum length of a printout
 */
#ifndef MY_PRINTF_BUFFER_SIZE
#define MY_PRINTF_BUFFER_SIZE 224
#endif

/**
 * @brief Define pool and message queue size, i.e. maximum number of unsent printouts
 */
#ifndef MY_PRINTF_POOL_AND_MESSAGE_QUEUE_SIZE
#define MY_PRINTF_POOL_AND_MESSAGE_QUEUE_SIZE 40
#endif

/**
 * @brief Make the UART handle used by MyPrintf available globally.
 */
extern UART_HandleTypeDef *my_printf_uart_handle_p;

/**
 * @brief Function for initializing MyPrintf.
 *
 * This function configures a memory pool for printouts, a message queue for sending
 * printouts to MyPrintf thread.
 *
 * @param[in] huart Pointer to a handle for the UART to be used for printout.
 *
 * @retval true  If initialization was successful.
 * @retval false If initialization failed.
 */
bool MyPrintfInit(void *huart);

/**
 * @brief Function for sending formatted print output to the UART.
 *
 * This function takes a variable number of arguments and formats the string that
 * is to be sent to UART.
 *
 * @param[in] fmt Pointer to a string that contains formatting options.
 * @param[in] ... Variable number of arguments to enter the formatted string.
 */
void MyPrintf(const char *fmt, ...);

/**
 * @brief PRINTF macro for simplicity.
 */
#if 1
#define PRINTF(...) MyPrintf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while(0)
#endif

#endif /* MY_PRINTF_H_ */
