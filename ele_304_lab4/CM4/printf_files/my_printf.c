/**
 * @file   my_printf.c
 * @author Ken Henry Andersen
 * @brief  Configure and handling debug printouts from a single core application.
 *         A thread receives messages with pointer to memory pool buffer containing
 *         ready formatted printouts to be sent to UART.
 * @date   2023-08-08
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h" /* CMSIS RTOSv2 header file */
#include <my_printf.h>

/* Define UART transfer completed thread flag */
#define PS_UART_TX_COMPLETED 0x04

/* For convenience, define a pointer to UART handle */
UART_HandleTypeDef *my_printf_uart_handle_p = NULL;

/* Declare MyPrintf thread function */
void MyPrintfThread(void *arg);

/* Define a memory block format */
typedef struct {
	uint8_t buff[MY_PRINTF_BUFFER_SIZE];
} my_printf_block_format_t;

/* Defined MyPrintf memory pool id */
osMemoryPoolId_t my_printf_pool_id = NULL;

/* Defined MyPrintf message queue id */
osMessageQueueId_t my_printf_msg_qid = NULL;

/* Define MyPrintf thread id */
osThreadId_t my_printf_thread_id = NULL;

bool MyPrintfInit(void *huart)
{
	/* Define MyPrintf thread attributes */
	osThreadAttr_t my_printf_thread_attributes = {
			.name = "MyPrintfThread",
			.priority = osPriorityRealtime};

	/* Define MyPrintf memory pool attributes */
	osMemoryPoolAttr_t my_printf_pool_attributes = {
			.name = "MyPrintfPool"
	};

	/* Define MyPrintf message queue attributes */
	osMessageQueueAttr_t my_printf_message_queue_attributes = {
			.name = "MyPrintfMessageQueue"
	};

	if (huart == NULL) {
		return false;
	}

	/* Store UART handle to use by MyPrintf */
	my_printf_uart_handle_p = (UART_HandleTypeDef *)huart;

	/* Create MyPrintf memory pool */
	my_printf_pool_id = osMemoryPoolNew(
			MY_PRINTF_POOL_AND_MESSAGE_QUEUE_SIZE, sizeof(my_printf_block_format_t),
			&my_printf_pool_attributes);

	if (my_printf_pool_id == NULL) {
		return false;
	}

	/* Create MyPrintf message queue */
	my_printf_msg_qid = osMessageQueueNew(
			MY_PRINTF_POOL_AND_MESSAGE_QUEUE_SIZE, sizeof(uint32_t),
			&my_printf_message_queue_attributes);

	if (my_printf_msg_qid == NULL) {
		return false;
	}

	/* Create MyPrintf thread */
	my_printf_thread_id = osThreadNew(MyPrintfThread, NULL, &my_printf_thread_attributes);

	if (my_printf_thread_id == NULL) {
		return false;
	}

	return true;
}

static void MyPrintfCRLF(char * const buf)
{
	uint32_t len = strlen(buf);

	if ((len > 1) &&
	    (((buf[len - 2] == '\r') && (buf[len - 1] == '\n')) ||
		 ((buf[len - 2] == '\n') && (buf[len - 1] == '\r')))) {
		return;
	}

	if ((buf[len - 1] == '\r') ||  (buf[len - 1] == '\n')) {
		len--;
	}

	if (len == MY_PRINTF_BUFFER_SIZE - 1U) {
		len -= 2U;
	} else if (len == MY_PRINTF_BUFFER_SIZE - 2U) {
		len -= 1U;
	}
	buf[len] = '\r';
	len++;
	buf[len] = '\n';
	len++;
	buf[len] = '\0';
}

void MyPrintf(const char *fmt, ...)
{
	/* va_list is a type to hold information about variable arguments */
	va_list args;

	osStatus_t status = osError;
	
	/* Allocate a memory pool block */
	my_printf_block_format_t *block_p = (my_printf_block_format_t *)osMemoryPoolAlloc(my_printf_pool_id, 0);

	if (block_p == NULL) {
		/* Pool is empty, bail out */
		return;
	}

	/* va_start must be called before accessing variable argument list */
	va_start(args, fmt);

	/* The vsnprintf() function formats and stores a series of characters and
	 * values in the buffer target-string. The vsnprintf() function works just
	 * like the snprintf() function, except that args points to a list of
	 * arguments whose number can vary from call to call in the program. These
	 * arguments should be initialized by the va_start function for each call.
	 * In contrast, the snprintf() function can have a list of arguments, but
	 * the number of arguments in that list is fixed when you compile the program.
	 *
	 * The vsnprintf() function converts each entry in the argument list according
	 * to the corresponding format specifier in format. The format has the same form
	 * and function as the format string for the printf() function. */
	vsnprintf((char *)block_p->buff, MY_PRINTF_BUFFER_SIZE, fmt, args);

	/* va_end should be executed before the function returns whenever
	 * va_start has been previously used in that function */
	va_end(args);

	/* Make sure formatted printout ends with \r\n */
	MyPrintfCRLF((char *)block_p->buff);

	/* Put the formatted printout on to the message queue */
	status = osMessageQueuePut(my_printf_msg_qid, &block_p, 0, 0);

	if (status != osOK) {
		/* Free pool buffer and bail out */
		osMemoryPoolFree(my_printf_pool_id, block_p);
	}
}

void MyPrintfThread(void *arg)
{
	while (1)
	{
		my_printf_block_format_t *block_p;

		osStatus_t status = osMessageQueueGet(my_printf_msg_qid, (void *)&block_p, NULL, osWaitForever);

		if (status == osOK) {
			if (HAL_UART_Transmit_DMA(my_printf_uart_handle_p, block_p->buff, strlen((char *)block_p->buff)) == HAL_OK) {
				uint32_t flag = osThreadFlagsWait(PS_UART_TX_COMPLETED, osFlagsWaitAny, osWaitForever);

				if ((flag & osFlagsError) || !(flag & PS_UART_TX_COMPLETED)) {
					MyPrintf("Error: Received unexpected flag 0x%x", flag);
				}
			}
			osMemoryPoolFree(my_printf_pool_id, block_p);
		}
	}
}

/* Take over the weak defined HAL_UART_TxCpltCallback() */
#define MyPrintfTxCompletedCB HAL_UART_TxCpltCallback

void MyPrintfTxCompletedCB(UART_HandleTypeDef *huart)
{
	if (huart == my_printf_uart_handle_p) {
		/* Set MyPrintf thread flag */
		osThreadFlagsSet(my_printf_thread_id, PS_UART_TX_COMPLETED);
	}
}

/* End of file my_printf.c */
