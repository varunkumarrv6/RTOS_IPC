#include "ipc.h"

volatile IPC_Message_t ipc_message
    __attribute__((section(".shared_ipc"), aligned(32)));
volatile IPC_ADC_Message_t ipc_adc_message
    __attribute__((section(".shared_ipc"), aligned(32)));
volatile uint32_t adc_stop=0;
void IPC_SetCommand(IPC_Command_t command)
{
    ipc_message.command = command;
}
void ADC_SetCommand(IPC_ADC command)
{
    if (command == IPC_ADC_HALF)
    {
        ipc_adc_message.ready[0] = 1U;
        ipc_adc_message.transfer_count[0]++;
    }
    else if (command == IPC_ADC_FULL)
    {
        ipc_adc_message.ready[1] = 1U;
        ipc_adc_message.transfer_count[1]++;
    }
}
