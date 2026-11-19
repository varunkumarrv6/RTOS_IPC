#ifndef IPC_H
#define IPC_H

#include <stdint.h>

#define MY_PRINTF_QUEUE_SIZE 40U

typedef enum
{
    IPC_CMD_NONE = 0,
    IPC_CMD_ADC_START,
    IPC_CMD_ADC_STOP,
    IPC_CMD_DAC_SINE,
    IPC_CMD_DAC_SQUARE,
} IPC_Command_t;
typedef enum
{
 IPC_ADC_HALF,
 IPC_ADC_FULL
}IPC_ADC;

/*
 * Shared pointer queue for CM7 -> CM4 MyPrintf messages
 */
typedef struct
{
    uintptr_t queue[MY_PRINTF_QUEUE_SIZE];

    volatile uint32_t write_index;
    volatile uint32_t read_index;

} MyPrintfIPCQueue_t;

#define MY_PRINTF_RELEASE_QUEUE_SIZE 40U

typedef struct
{
    uintptr_t queue[MY_PRINTF_RELEASE_QUEUE_SIZE];
    volatile uint32_t write_index;
    volatile uint32_t read_index;
} MyPrintfReleaseQueue_t;
/*
 * Shared IPC structure
 */
typedef struct
{
    volatile IPC_Command_t command;

    MyPrintfIPCQueue_t printf_queue;
    MyPrintfReleaseQueue_t release_queue;


} IPC_Message_t;
typedef struct
{
    volatile uint32_t ready[2];
    volatile uint32_t transfer_count[2];
} IPC_ADC_Message_t;

extern volatile IPC_ADC_Message_t ipc_adc_message;
extern volatile uint32_t adc_stop;
/*
 * Shared IPC object
 */
extern volatile IPC_Message_t ipc_message;


/*
 * ADC command function
 */
void IPC_SetCommand(IPC_Command_t command);
void ADC_SetCommand(IPC_ADC command);
#endif
