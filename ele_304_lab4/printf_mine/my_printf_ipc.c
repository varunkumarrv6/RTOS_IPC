//#include "ipc.h"
//#include "my_printf_ipc.h"
//
//
//
//
//volatile MyPrintfPool_t cm7_myprintf_pool
//    __attribute__((section(".shared_myprintf_cm7"), aligned(32)));
//
//volatile MyPrintfPool_t cm4_myprintf_pool
//    __attribute__((section(".shared_myprintf_cm4"), aligned(32)));
//
//volatile ADCSharedBlock_t adc_buffer_pool[2]
//    __attribute__((section(".shared_adc_buffer"), aligned(32)));
/**
 * @file   my_printf_ipc.c
 * @brief  Definitions of the shared SRAM4 objects declared in my_printf_ipc.h.
 *
 *         IMPORTANT: no #ifdef CORE_CM7 / CORE_CM4 here.
 *         Both cores must define every shared object, otherwise each linker
 *         drops the other core's section and the objects end up overlapping
 *         at the same SRAM4 address.
 */
#include "ipc.h"
#include "my_printf_ipc.h"

/* CM7 print pool: CM7 allocates here, CM4 reads the text, CM7 frees */
volatile MyPrintfPool_t cm7_myprintf_pool
    __attribute__((section(".shared_myprintf_cm7"), aligned(32)));

/* CM4 print pool: only CM4 allocates and frees here */
volatile MyPrintfPool_t cm4_myprintf_pool
    __attribute__((section(".shared_myprintf_cm4"), aligned(32)));

/* ADC half-buffers: CM4 writes, CM7 reads (MDMA or CPU) into DTCM */
volatile ADCSharedBlock_t adc_buffer_pool[2]
    __attribute__((section(".shared_adc_buffer"), aligned(32)));
