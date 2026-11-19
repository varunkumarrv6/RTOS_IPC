///* USER CODE BEGIN Header */
///**
//  ******************************************************************************
//  * File Name          : freertos.c
//  * Description        : Code for freertos applications
//  ******************************************************************************
//  * @attention
//  *
//  * Copyright (c) 2026 STMicroelectronics.
//  * All rights reserved.
//  *
//  * This software is licensed under terms that can be found in the LICENSE file
//  * in the root directory of this software component.
//  * If no LICENSE file comes with this software, it is provided AS-IS.
//  *
//  ******************************************************************************
//  */
///* USER CODE END Header */
//
///* Includes ------------------------------------------------------------------*/
//#include "FreeRTOS.h"
//#include "task.h"
//#include "main.h"
//#include "FreeRTOS.h"
//#include "cmsis_os2.h"
//
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//#include "math.h"
//#include "ipc.h"
//#include "my_printf_ipc.h"
//#include "my_printf.h"
//
//#include "string.h"
///* USER CODE END Includes */
//
///* Private typedef -----------------------------------------------------------*/
///* USER CODE BEGIN PTD */
//
///* USER CODE END PTD */
//
///* Private define ------------------------------------------------------------*/
///* USER CODE BEGIN PD */
//
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
//
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
///* USER CODE BEGIN Variables */
//uint32_t stopped=0;
///* USER CODE END Variables */
///* Definitions for defaultTask */
//osThreadId_t defaultTaskHandle;
//const osThreadAttr_t defaultTask_attributes = {
//  .name = "defaultTask",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};
///* Definitions for ADC_DTCM */
//osThreadId_t ADC_DTCMHandle;
//const osThreadAttr_t ADC_DTCM_attributes = {
//  .name = "ADC_DTCM",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};
//
///* Private function prototypes -----------------------------------------------*/
///* USER CODE BEGIN FunctionPrototypes */
//
///* USER CODE END FunctionPrototypes */
//
//void StartDefaultTask(void *argument);
//void ADC_FFT(void *argument);
//
//void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
//
///**
//  * @brief  FreeRTOS initialization
//  * @param  None
//  * @retval None
//  */
//void MX_FREERTOS_Init(void) {
//  /* USER CODE BEGIN Init */
//
//  /* USER CODE END Init */
//
//  /* USER CODE BEGIN RTOS_MUTEX */
//  /* add mutexes, ... */
//  /* USER CODE END RTOS_MUTEX */
//
//  /* USER CODE BEGIN RTOS_SEMAPHORES */
//  /* add semaphores, ... */
//  /* USER CODE END RTOS_SEMAPHORES */
//
//  /* USER CODE BEGIN RTOS_TIMERS */
//  /* start timers, add new ones, ... */
//  /* USER CODE END RTOS_TIMERS */
//
//  /* USER CODE BEGIN RTOS_QUEUES */
//  /* add queues, ... */
//  /* USER CODE END RTOS_QUEUES */
//
//  /* Create the thread(s) */
//  /* creation of defaultTask */
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
//
//  /* creation of ADC_DTCM */
//  ADC_DTCMHandle = osThreadNew(ADC_FFT, NULL, &ADC_DTCM_attributes);
//
//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
//  /* USER CODE END RTOS_THREADS */
//
//  /* USER CODE BEGIN RTOS_EVENTS */
//  /* add events, ... */
//  /* USER CODE END RTOS_EVENTS */
//
//}
//
///* USER CODE BEGIN Header_StartDefaultTask */
///**
//  * @brief  Function implementing the defaultTask thread.
//  * @param  argument: Not used
//  * @retval None
//  */
///* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void *argument)
//{
//  /* USER CODE BEGIN StartDefaultTask */
//  /* Infinite loop */
//
//	MyPrintf("HELLO FROM CM7");
//	MyPrintf("MESSAGE 2");
//	MyPrintf("MESSAGE 3");
//
//
//
//
//
//
//
////IPC_SetCommand(IPC_CMD_DAC_SINE);
////osDelay(2000);
////IPC_SetCommand(IPC_CMD_DAC_SQUARE);
////	osDelay(1000);
////IPC_SetCommand(IPC_CMD_ADC_START);
////osDelay(2000);
////IPC_SetCommand(IPC_CMD_ADC_STOP);
////osDelay(1000);
//	 IPC_SetCommand(IPC_CMD_DAC_SQUARE);
//	 osDelay(200);
//
//	 IPC_SetCommand(IPC_CMD_ADC_START);
//	 osDelay(200);
//
//
//
//	    for(;;)
//	    {
//	        /* Check whether CM4 released any printf buffers */
//
//	    	 if (!stopped && HAL_GetTick() >= 20000U)
//	    	    {
//	    	        IPC_SetCommand(IPC_CMD_ADC_STOP);
//	    	        osDelay(200);
//	    	        adc_stop = 2U;
//	    	        stopped = 1;
//	    	    }
//	        while (ipc_message.release_queue.read_index !=
//	               ipc_message.release_queue.write_index)
//	        {
//	            uint32_t index =
//	                ipc_message.release_queue.read_index;
//
//	            uintptr_t ptr =
//	                ipc_message.release_queue.queue[index];
//
//	            MyPrintfBlock_t *block =
//	                (MyPrintfBlock_t *)ptr;
//
//	            /* CM7 owns this pool, so CM7 frees it */
//
//	            MyPrintfFree(block);
////MyPrintf("Done releasing\r\n");
//	            ipc_message.release_queue.read_index =
//	                (index + 1U) %
//	                MY_PRINTF_RELEASE_QUEUE_SIZE;
//	        }
//
//
//	        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
//
//	        osDelay(10);
//	    }
////	    osThreadExit();
//  /* USER CODE END StartDefaultTask */
//}
//
///* USER CODE BEGIN Header_ADC_FFT */
///**
//* @brief Function implementing the ADC_DTCM thread.
//* @param argument: Not used
//* @retval None
//*/
///* USER CODE END Header_ADC_FFT */
//void ADC_FFT(void *argument)
//{
//  /* USER CODE BEGIN ADC_FFT */
//  /* Infinite loop */
//  for(;;)
//  {
//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_0);
//	 	 CM7_ProcessADCCommand();
//	 	osDelay(1);
//  }
//  /* USER CODE END ADC_FFT */
//}
//
///* Private application code --------------------------------------------------*/
///* USER CODE BEGIN Application */
//
///* USER CODE END Application */
//
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <string.h>
#include "arm_math.h"
#include "mdma.h"
#include "ipc.h"
#include "my_printf_ipc.h"
#include "my_printf.h"
#include "lab3modify.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FFT_LEN               ADC_BLOCK_SIZE   /* 1024-point real FFT        */
#define FFT_PRINT_BINS        64U              /* bins printed after STOP    */
#define FFT_BINS_PER_LINE     4U
#define PRINT_SPACING_MS      20U              /* let CM4 drain the UART     */
#define CMD_SETTLE_MS         200U             /* time for CM4 to act on cmd */
#define CTRL_PERIOD_MS        10U              /* control/release loop       */
#define MDMA_TIMEOUT_MS       10U
#define NO_BLOCK              0xFFU

/* Thread flags for the FFT thread */
#define FFT_ADC_READY_FLAG    (1U << 0)        /* HSEM mode: CM4 marked a block ready */
#define FFT_PRINT_FLAG        (1U << 1)        /* control thread: ADC stopped, print  */

#define HSEM_IRQ_PRIORITY     6U               /* >= configLIBRARY_MAX_SYSCALL (5)    */
#define HSEM_ADC_MASK         __HAL_HSEM_SEMID_TO_MASK(HSEM_ID_ADC_READY)

/* Logic analyser pins (CM7) */
#define DBG_COPY_PORT         GPIOC            /* SRAM4 -> DTCM transfer   */
#define DBG_COPY_PIN          GPIO_PIN_0
#define DBG_FFT_PORT          GPIOD            /* arm_rfft_fast_f32        */
#define DBG_FFT_PIN           GPIO_PIN_0
#define DBG_CONVERT_PORT      GPIOE            /* uint16 -> float (MDMA mode only) */
#define DBG_CONVERT_PIN       GPIO_PIN_0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* FFT working buffers in DTCM (never cached, 0-wait-state for the CM7) */
__attribute__((section(".dtcm_fft"), aligned(16)))
static float32_t fft_input[2][FFT_LEN];
__attribute__((section(".dtcm_fft"), aligned(16)))
static float32_t fft_output[2][FFT_LEN];
#if (LAB3_COPY_MODE == COPY_MDMA)
__attribute__((section(".dtcm_fft"), aligned(16)))
static uint16_t raw_in[2][FFT_LEN];            /* MDMA destination */
#endif

static arm_rfft_fast_instance_f32 rfft;
static volatile uint8_t last_block = NO_BLOCK;
static uint32_t blocks_processed = 0U;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ADC_DTCM */
osThreadId_t ADC_DTCMHandle;
const osThreadAttr_t ADC_DTCM_attributes = {
  .name = "ADC_DTCM",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void PrintConfig(void);
static void ReleasePrintfBuffers(void);
static void FFT_TransferBlock(uint32_t b);
static void FFT_ProcessReadyBlocks(void);
static void FFT_PrintLatest(void);
static int32_t FFT_BinMag(const float32_t *o, uint32_t k);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void ADC_FFT(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ADC_DTCM */
  ADC_DTCMHandle = osThreadNew(ADC_FFT, NULL, &ADC_DTCM_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Control thread: selects DAC waveform, starts/stops the ADC on CM4,
  *         asks the FFT thread to print, and frees printf buffers returned by CM4.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  uint8_t stopped = 0U;

  PrintConfig();
  MyPrintf("HELLO FROM CM7");

  IPC_SetCommand((LAB3_WAVE_SQUARE != 0) ? IPC_CMD_DAC_SQUARE : IPC_CMD_DAC_SINE);
  osDelay(CMD_SETTLE_MS);
  IPC_SetCommand(IPC_CMD_ADC_START);
  osDelay(CMD_SETTLE_MS);

  for (;;)
  {
    if ((stopped == 0U) && (HAL_GetTick() >= LAB3_RUN_TIME_MS))
    {
      stopped = 1U;
      IPC_SetCommand(IPC_CMD_ADC_STOP);
      osDelay(CMD_SETTLE_MS);                       /* CM4 stops ADC, last block done */
      (void)osThreadFlagsSet(ADC_DTCMHandle, FFT_PRINT_FLAG);
    }

    ReleasePrintfBuffers();
    HAL_GPIO_TogglePin(Green_led_cm7_GPIO_Port, Green_led_cm7_Pin);
    osDelay(CTRL_PERIOD_MS);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ADC_FFT */
/**
* @brief FFT thread: moves each ready block SRAM4 -> DTCM and runs the RFFT.
*        NOTIFY_POLL: wakes every LAB3_POLL_PERIOD_MS and checks ready[].
*        NOTIFY_HSEM: sleeps until the HSEM1 interrupt sets a thread flag.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ADC_FFT */
void ADC_FFT(void *argument)
{
  /* USER CODE BEGIN ADC_FFT */
  if (arm_rfft_fast_init_f32(&rfft, FFT_LEN) != ARM_MATH_SUCCESS)
  {
    Error_Handler();
  }

  /* Run above the control thread so a ready block is handled first */
  (void)osThreadSetPriority(osThreadGetId(), osPriorityAboveNormal);

#if (LAB3_NOTIFY_MODE == NOTIFY_HSEM)
  const uint32_t wait_flags   = FFT_ADC_READY_FLAG | FFT_PRINT_FLAG;
  const uint32_t wait_timeout = osWaitForever;

  HAL_HSEM_ActivateNotification(HSEM_ADC_MASK);
  HAL_NVIC_SetPriority(HSEM1_IRQn, HSEM_IRQ_PRIORITY, 0U);
  HAL_NVIC_EnableIRQ(HSEM1_IRQn);
#else
  const uint32_t wait_flags   = FFT_PRINT_FLAG;
  const uint32_t wait_timeout = LAB3_POLL_PERIOD_MS;   /* acts as the poll delay */
#endif

  for (;;)
  {
    uint32_t flags = osThreadFlagsWait(wait_flags, osFlagsWaitAny, wait_timeout);

    if ((flags & osFlagsError) != 0U)
    {
      flags = 0U;                                       /* poll timeout: no flag */
    }

    FFT_ProcessReadyBlocks();

    if ((flags & FFT_PRINT_FLAG) != 0U)
    {
      FFT_PrintLatest();
    }
  }
  /* USER CODE END ADC_FFT */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* ---------------- HSEM notification from CM4 ---------------- */
#if (LAB3_NOTIFY_MODE == NOTIFY_HSEM)
/** @brief HSEM interrupt for CM7 (vector HSEM1_IRQHandler in startup file). */
void HSEM1_IRQHandler(void)
{
  HAL_HSEM_IRQHandler();
}

/**
  * @brief Called by HAL_HSEM_IRQHandler when CM4 releases HSEM_ID_ADC_READY.
  *        The HAL disables the notification in the IRQ, so re-arm it here.
  */
void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
  if ((SemMask & HSEM_ADC_MASK) != 0U)
  {
    HAL_HSEM_ActivateNotification(HSEM_ADC_MASK);
    (void)osThreadFlagsSet(ADC_DTCMHandle, FFT_ADC_READY_FLAG);
  }
}
#endif

/* ---------------- SRAM4 -> DTCM + RFFT ---------------- */

/**
  * @brief Move block b from SRAM4 to DTCM as float32 and free the SRAM4 slot.
  *        DBG_COPY pin is high during the transfer.
  */
static void FFT_TransferBlock(uint32_t b)
{
#if (LAB3_COPY_MODE == COPY_CPU)
  /* CPU: read uint16 from SRAM4, write float to DTCM in one pass */
  HAL_GPIO_WritePin(DBG_COPY_PORT, DBG_COPY_PIN, GPIO_PIN_SET);
  for (uint32_t i = 0U; i < FFT_LEN; i++)
  {
    fft_input[b][i] = (float32_t)adc_buffer_pool[b].samples[i];
  }
  HAL_GPIO_WritePin(DBG_COPY_PORT, DBG_COPY_PIN, GPIO_PIN_RESET);

  __DMB();
  ipc_adc_message.ready[b] = 0U;                       /* slot free for CM4 */

#else
  /* MDMA: raw uint16 block SRAM4 -> DTCM (CPU waits for completion) */
  HAL_GPIO_WritePin(DBG_COPY_PORT, DBG_COPY_PIN, GPIO_PIN_SET);
  if (HAL_MDMA_Start(&hmdma_mdma_channel0_sw_0,
                     (uint32_t)&adc_buffer_pool[b].samples[0],
                     (uint32_t)&raw_in[b][0],
                     FFT_LEN * sizeof(uint16_t), 1U) != HAL_OK)
  {
    Error_Handler();
  }
  (void)HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
  if (HAL_MDMA_PollForTransfer(&hmdma_mdma_channel0_sw_0,
                               HAL_MDMA_FULL_TRANSFER, MDMA_TIMEOUT_MS) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_GPIO_WritePin(DBG_COPY_PORT, DBG_COPY_PIN, GPIO_PIN_RESET);

  __DMB();
  ipc_adc_message.ready[b] = 0U;                       /* slot free for CM4 */

  /* uint16 -> float, DTCM to DTCM */
  HAL_GPIO_WritePin(DBG_CONVERT_PORT, DBG_CONVERT_PIN, GPIO_PIN_SET);
  for (uint32_t i = 0U; i < FFT_LEN; i++)
  {
    fft_input[b][i] = (float32_t)raw_in[b][i];
  }
  HAL_GPIO_WritePin(DBG_CONVERT_PORT, DBG_CONVERT_PIN, GPIO_PIN_RESET);
#endif
}

/** @brief Handle every block CM4 has marked ready: transfer, then RFFT. */
static void FFT_ProcessReadyBlocks(void)
{
  for (uint32_t b = 0U; b < 2U; b++)
  {
    if (ipc_adc_message.ready[b] == 1U)
    {
      __DMB();                                         /* flag before data */
      FFT_TransferBlock(b);

      HAL_GPIO_WritePin(DBG_FFT_PORT, DBG_FFT_PIN, GPIO_PIN_SET);
      arm_rfft_fast_f32(&rfft, fft_input[b], fft_output[b], 0U);
      HAL_GPIO_WritePin(DBG_FFT_PORT, DBG_FFT_PIN, GPIO_PIN_RESET);

      last_block = (uint8_t)b;
      blocks_processed++;
    }
  }
}

/**
  * @brief Magnitude of bin k from CMSIS packed RFFT output.
  *        o[0] = DC (real), o[1] = Nyquist (real), then (re, im) pairs.
  */
static int32_t FFT_BinMag(const float32_t *o, uint32_t k)
{
  if (k == 0U)
  {
    return (int32_t)fabsf(o[0]);
  }
  return (int32_t)hypotf(o[2U * k], o[2U * k + 1U]);
}

/** @brief Print the latest RFFT once, after the ADC is stopped. */
static void FFT_PrintLatest(void)
{
  if (last_block == NO_BLOCK)
  {
    MyPrintf("CM7: no block processed");
    return;
  }

  const float32_t *o = fft_output[last_block];
  uint32_t peak_bin = 1U;
  int32_t  peak_mag = 0;

  for (uint32_t k = 1U; k < (FFT_LEN / 2U); k++)          /* skip DC */
  {
    int32_t m = FFT_BinMag(o, k);
    if (m > peak_mag)
    {
      peak_mag = m;
      peak_bin = k;
    }
  }

  MyPrintf("CM7 blocks processed %lu", (unsigned long)blocks_processed);
  osDelay(PRINT_SPACING_MS);
  MyPrintf("block %u: peak bin %lu (%lu Hz), mag %ld",
           (unsigned)last_block, (unsigned long)peak_bin,
           (unsigned long)((peak_bin * LAB3_FS_HZ) / FFT_LEN), (long)peak_mag);
  osDelay(PRINT_SPACING_MS);

  for (uint32_t k = 0U; k < FFT_PRINT_BINS; k += FFT_BINS_PER_LINE)
  {
    MyPrintf("bins %2lu..%2lu: %ld %ld %ld %ld",
             (unsigned long)k, (unsigned long)(k + FFT_BINS_PER_LINE - 1U),
             (long)FFT_BinMag(o, k),      (long)FFT_BinMag(o, k + 1U),
             (long)FFT_BinMag(o, k + 2U), (long)FFT_BinMag(o, k + 3U));
    osDelay(PRINT_SPACING_MS);
  }
}

/* ---------------- Misc ---------------- */

/** @brief Print the active build configuration (goes at the top of every log). */
static void PrintConfig(void)
{
  MyPrintf("CFG copy=%s notify=%s icache=%d dcache=%d fs=%lu Hz wave=%s",
           (LAB3_COPY_MODE == COPY_MDMA) ? "MDMA" : "CPU",
           (LAB3_NOTIFY_MODE == NOTIFY_HSEM) ? "HSEM" : "POLL",
           LAB3_ENABLE_ICACHE, LAB3_ENABLE_DCACHE,
           (unsigned long)LAB3_FS_HZ,
           (LAB3_WAVE_SQUARE != 0) ? "square" : "sine");
}

/** @brief Free CM7 printf blocks that CM4 has finished printing. */
static void ReleasePrintfBuffers(void)
{
  while (ipc_message.release_queue.read_index != ipc_message.release_queue.write_index)
  {
    uint32_t idx = ipc_message.release_queue.read_index;
    __DMB();                                           /* index before data */
    MyPrintfFree((MyPrintfBlock_t *)ipc_message.release_queue.queue[idx]);
    ipc_message.release_queue.read_index = (idx + 1U) % MY_PRINTF_RELEASE_QUEUE_SIZE;
  }
}

/* USER CODE END Application */
