//#ifndef MY_PRINTF_IPC_H
//#define MY_PRINTF_IPC_H
//
//#include <stdint.h>
//
//#define MY_PRINTF_BUFFER_SIZE 224U
//#define MY_PRINTF_POOL_SIZE   40U
//
//typedef struct
//{
//    uint8_t buff[MY_PRINTF_BUFFER_SIZE];
//} MyPrintfBlock_t;
//#define ADC_BLOCK_SIZE 1024U
//
//typedef struct {
//    uint16_t samples[ADC_BLOCK_SIZE];
//} ADCSharedBlock_t;
//typedef struct
//{
//    MyPrintfBlock_t blocks[MY_PRINTF_POOL_SIZE];
//
//    volatile uint8_t used[MY_PRINTF_POOL_SIZE];
//
//} MyPrintfPool_t;
//extern volatile MyPrintfPool_t cm7_myprintf_pool;
//extern volatile MyPrintfPool_t cm4_myprintf_pool;
//extern volatile ADCSharedBlock_t adc_buffer_pool[2];
//void MyPrintfFree(MyPrintfBlock_t *block);
//#endif
/**
 * @file   my_printf_ipc.h
 * @brief  Shared-memory objects used by MyPrintf and the ADC transfer.
 *         All objects live in SRAM4 (D3, 0x38000000) so both CM7 and CM4
 *         can access them. Both cores define ALL of them (my_printf_ipc.c),
 *         so the linker gives every object the same address on both cores.
 *
 *         SRAM4 layout (check both .map files after any change):
 *           0x38000000  ipc_message / ipc_adc_message   (ipc.c)
 *           0x38000180  cm7_myprintf_pool
 *           0x380024C0  cm4_myprintf_pool
 *           0x38004800  adc_buffer_pool[2]
 */
#ifndef MY_PRINTF_IPC_H
#define MY_PRINTF_IPC_H

#include <stdint.h>

/** @brief Maximum length of one printout, including "\r\n\0". */
#define MY_PRINTF_BUFFER_SIZE 224U

/** @brief Number of print blocks in each core's pool. */
#define MY_PRINTF_POOL_SIZE   40U

/** @brief Number of ADC samples in one half of the double buffer (= FFT length). */
#define ADC_BLOCK_SIZE        1024U

/** @brief One print buffer. */
typedef struct
{
    uint8_t buff[MY_PRINTF_BUFFER_SIZE];
} MyPrintfBlock_t;

/**
 * @brief Fixed-size block pool for one core.
 *        used[i] = 1 while blocks[i] is allocated, 0 when free.
 *        The pool is in a NOLOAD section: CM7 main() must clear it before CM4 starts.
 */
typedef struct
{
    MyPrintfBlock_t  blocks[MY_PRINTF_POOL_SIZE];
    volatile uint8_t used[MY_PRINTF_POOL_SIZE];
} MyPrintfPool_t;

/**
 * @brief One ADC half-buffer copied by CM4 for CM7.
 *        Ownership is given by ipc_adc_message.ready[slot]:
 *        0 = CM4 may write, 1 = CM7 owns it until it clears the flag.
 */
typedef struct
{
    uint16_t samples[ADC_BLOCK_SIZE];
} ADCSharedBlock_t;

/* Shared objects (defined in my_printf_ipc.c, placed in SRAM4 by the linker) */
extern volatile MyPrintfPool_t   cm7_myprintf_pool;   /**< CM7 print buffers */
extern volatile MyPrintfPool_t   cm4_myprintf_pool;   /**< CM4 print buffers */
extern volatile ADCSharedBlock_t adc_buffer_pool[2];  /**< [0] = first half, [1] = second half */

/**
 * @brief Return a print block to the calling core's own pool.
 * @param block Pointer previously handed out by MyPrintf().
 */
void MyPrintfFree(MyPrintfBlock_t *block);

#endif /* MY_PRINTF_IPC_H */
