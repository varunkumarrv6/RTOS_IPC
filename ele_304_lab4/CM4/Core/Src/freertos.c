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
//#include "cmsis_os2.h"
//
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//#include <math.h>
//#include <string.h>
//#include "ipc.h"
//#include "my_printf_ipc.h"
//#include "my_printf.h"
//#include "adc.h"
//#include "tim.h"
//#include "usart.h"
//#include "dac.h"
///* USER CODE END Includes */
//
///* Private typedef -----------------------------------------------------------*/
///* USER CODE BEGIN PTD */
//
///* USER CODE END PTD */
//
///* Private define ------------------------------------------------------------*/
///* USER CODE BEGIN PD */
///* ADC double buffer: two halves of ADC_BLOCK_SIZE samples (DMA circular) */
//#define ADC_BUFFER_SIZE         (2U * ADC_BLOCK_SIZE)
//
///* Thread flags for the ADC thread */
//#define ADC_HALF_FLAG           (1U << 0)   /* first half of buffer filled  */
//#define ADC_FULL_FLAG           (1U << 1)   /* second half of buffer filled */
//#define ADC_ERROR_FLAG          (1U << 2)   /* HAL_ADC_ErrorCallback        */
//#define ADC_CMD_START_FLAG      (1U << 3)   /* START command from CM7       */
//#define ADC_CMD_STOP_FLAG       (1U << 4)   /* STOP command from CM7        */
//
///* Thread flags for the DAC thread */
//#define DAC_SINE_FLAG           (1U << 5)
//#define DAC_SQUARE_FLAG         (1U << 6)
//
///* DAC waveform: one period = DAC_LUT_SIZE samples at TIM2 rate (100 kHz -> 1 kHz) */
//#define DAC_LUT_SIZE            100U
//#define DAC_MAX_CODE            4095U       /* 12-bit DAC full scale */
//#define DAC_PI                  3.14159265f
//
///* Command polling period of the CM4 default task */
//#define CMD_POLL_PERIOD_MS      10U
//#define HELLO_PERIOD_TICKS      100U        /* 100 x 10 ms = 1 s */
//
///* Debug printout of samples after STOP */
//#define DBG_SAMPLE_COUNT        100U
//#define DBG_SAMPLE_STEP         10U
//
///* Logic analyser pins */
//#define DBG_HALF_FULL_PORT      GPIOA       /* high = 1st half done, low = 2nd half done */
//#define DBG_HALF_FULL_PIN       GPIO_PIN_3
//#define DBG_CM4_COPY_PORT       GPIOB       /* high while copying to SRAM4 */
//#define DBG_CM4_COPY_PIN        GPIO_PIN_1
//#define LED_YELLOW_PORT         GPIOE
//#define LED_YELLOW_PIN          GPIO_PIN_1
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
//
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
///* USER CODE BEGIN Variables */
//static uint16_t adc_buffer[ADC_BUFFER_SIZE];   /* DMA target, D2 SRAM */
//static uint32_t dac_buffer[DAC_LUT_SIZE];      /* DAC lookup table    */
//static volatile uint8_t adc_running = 0U;
//
//static volatile uint32_t cb_count      = 0U;   /* half/full callbacks seen          */
//static volatile uint32_t copy_count    = 0U;   /* blocks copied to SRAM4            */
//static volatile uint32_t overrun_count = 0U;   /* blocks dropped: CM7 still owns it */
///* USER CODE END Variables */
///* Definitions for defaultTask */
//osThreadId_t defaultTaskHandle;
//const osThreadAttr_t defaultTask_attributes = {
//  .name = "defaultTask",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};
///* Definitions for ADC_THREAD */
//osThreadId_t ADC_THREADHandle;
//const osThreadAttr_t ADC_THREAD_attributes = {
//  .name = "ADC_THREAD",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};
///* Definitions for DAC */
//osThreadId_t DACHandle;
//const osThreadAttr_t DAC_attributes = {
//  .name = "DAC",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};
//
///* Private function prototypes -----------------------------------------------*/
///* USER CODE BEGIN FunctionPrototypes */
//static void DAC_FillSine(void);
//static void DAC_FillSquare(void);
//static void DAC_Restart(void (*fill)(void), const char *name);
//static void ADC_CopyBlockToShared(uint32_t slot);
//static void IPC_HandlePrintfQueue(void);
//static void IPC_HandleCommand(void);
///* USER CODE END FunctionPrototypes */
//
//void StartDefaultTask(void *argument);
//void StartTask02(void *argument);
//void StartTask03(void *argument);
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
//  (void)MyPrintfInit(&huart3);
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
//  /* creation of ADC_THREAD */
//  ADC_THREADHandle = osThreadNew(StartTask02, NULL, &ADC_THREAD_attributes);
//
//  /* creation of DAC */
//  DACHandle = osThreadNew(StartTask03, NULL, &DAC_attributes);
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
//  * @brief  Default task: prints CM7 messages, forwards CM7 commands, blinks yellow LED.
//  * @param  argument: Not used
//  * @retval None
//  */
///* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void *argument)
//{
//  /* USER CODE BEGIN StartDefaultTask */
//  uint32_t hello_ticks = 0U;
//
//  for (;;)
//  {
//    if (++hello_ticks >= HELLO_PERIOD_TICKS)
//    {
//      hello_ticks = 0U;
//      MyPrintf("hello started cm4");
//    }
//
//    IPC_HandlePrintfQueue();
//    IPC_HandleCommand();
//
//    HAL_GPIO_TogglePin(LED_YELLOW_PORT, LED_YELLOW_PIN);
//    osDelay(CMD_POLL_PERIOD_MS);
//  }
//  /* USER CODE END StartDefaultTask */
//}
//
///* USER CODE BEGIN Header_StartTask02 */
///**
//* @brief ADC thread: start/stop on CM7 command, copy each half buffer to SRAM4.
//* @param argument: Not used
//* @retval None
//*/
///* USER CODE END Header_StartTask02 */
//void StartTask02(void *argument)
//{
//  /* USER CODE BEGIN StartTask02 */
//  MyPrintf("ADC TASK STARTED");
//
//  for (;;)
//  {
//    uint32_t flags = osThreadFlagsWait(ADC_HALF_FLAG | ADC_FULL_FLAG | ADC_ERROR_FLAG |
//                                       ADC_CMD_START_FLAG | ADC_CMD_STOP_FLAG,
//                                       osFlagsWaitAny, osWaitForever);
//
//    if (flags & ADC_CMD_START_FLAG)
//    {
//      MyPrintf("ADC START command received");
//      if ((HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) ||
//          (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK) ||
//          (HAL_TIM_Base_Start(&htim3) != HAL_OK))
//      {
//        MyPrintf("ADC start failed");
//      }
//      else
//      {
//        adc_running = 1U;
//      }
//    }
//
//    if ((flags & ADC_HALF_FLAG) && adc_running)
//    {
//      ADC_CopyBlockToShared(0U);
//    }
//
//    if ((flags & ADC_FULL_FLAG) && adc_running)
//    {
//      ADC_CopyBlockToShared(1U);
//    }
//
//    if (flags & ADC_CMD_STOP_FLAG)
//    {
//      adc_running = 0U;
//      (void)HAL_TIM_Base_Stop(&htim3);
//      (void)HAL_ADC_Stop_DMA(&hadc1);
//
//      MyPrintf("callbacks %lu, copied %lu, overruns %lu",
//               (unsigned long)cb_count, (unsigned long)copy_count,
//               (unsigned long)overrun_count);
//
//      for (uint32_t i = 0U; i < DBG_SAMPLE_COUNT; i += DBG_SAMPLE_STEP)
//      {
//        MyPrintf("ADC[%lu] = %u  pool0 = %u  pool1 = %u", (unsigned long)i,
//                 adc_buffer[i], adc_buffer_pool[0].samples[i], adc_buffer_pool[1].samples[i]);
//      }
//      MyPrintf("ADC STOP command received");
//    }
//
//    if (flags & ADC_ERROR_FLAG)
//    {
//      MyPrintf("ADC ERROR");
//    }
//  }
//  /* USER CODE END StartTask02 */
//}
//
///* USER CODE BEGIN Header_StartTask03 */
///**
//* @brief DAC thread: switch between sine and square on CM7 command.
//* @param argument: Not used
//* @retval None
//*/
///* USER CODE END Header_StartTask03 */
//void StartTask03(void *argument)
//{
//  /* USER CODE BEGIN StartTask03 */
//  for (;;)
//  {
//    uint32_t flags = osThreadFlagsWait(DAC_SINE_FLAG | DAC_SQUARE_FLAG,
//                                       osFlagsWaitAny, osWaitForever);
//
//    if (flags & DAC_SINE_FLAG)
//    {
//      DAC_Restart(DAC_FillSine, "sine");
//    }
//    else if (flags & DAC_SQUARE_FLAG)
//    {
//      DAC_Restart(DAC_FillSquare, "square");
//    }
//  }
//  /* USER CODE END StartTask03 */
//}
//
///* Private application code --------------------------------------------------*/
///* USER CODE BEGIN Application */
//
///* ---------------- HAL callbacks (ISR context) ---------------- */
//
//void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
//{
//  if (hadc->Instance == ADC1)
//  {
//    HAL_GPIO_WritePin(DBG_HALF_FULL_PORT, DBG_HALF_FULL_PIN, GPIO_PIN_SET);
//    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_HALF_FLAG);
//  }
//}
//
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
//{
//  if (hadc->Instance == ADC1)
//  {
//    HAL_GPIO_WritePin(DBG_HALF_FULL_PORT, DBG_HALF_FULL_PIN, GPIO_PIN_RESET);
//    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_FULL_FLAG);
//  }
//}
//
//void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
//{
//  if (hadc->Instance == ADC1)
//  {
//    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_ERROR_FLAG);
//  }
//}
//
///* ---------------- ADC -> SRAM4 ---------------- */
//
///**
//  * @brief Copy one half of the DMA buffer to the shared SRAM4 slot and tell CM7.
//  *        If CM7 has not released the slot yet, the block is dropped (never
//  *        overwrite a block CM7 may be reading).
//  * @param slot 0 = first half, 1 = second half.
//  */
//static void ADC_CopyBlockToShared(uint32_t slot)
//{
//  cb_count++;
//
//  if (ipc_adc_message.ready[slot] == 1U)
//  {
//    overrun_count++;
//    return;
//  }
//
//  HAL_GPIO_WritePin(DBG_CM4_COPY_PORT, DBG_CM4_COPY_PIN, GPIO_PIN_SET);
//  memcpy((void *)adc_buffer_pool[slot].samples,
//         &adc_buffer[slot * ADC_BLOCK_SIZE],
//         ADC_BLOCK_SIZE * sizeof(uint16_t));
//  __DMB();                                      /* data visible before flag */
//  ADC_SetCommand((slot == 0U) ? IPC_ADC_HALF : IPC_ADC_FULL);
//  HAL_GPIO_WritePin(DBG_CM4_COPY_PORT, DBG_CM4_COPY_PIN, GPIO_PIN_RESET);
//
//  copy_count++;
//}
//
///* ---------------- IPC from CM7 ---------------- */
//
///**
//  * @brief Print every message CM7 has queued, then hand the buffer back to CM7.
//  */
//static void IPC_HandlePrintfQueue(void)
//{
//  while (ipc_message.printf_queue.read_index != ipc_message.printf_queue.write_index)
//  {
//    uint32_t idx = ipc_message.printf_queue.read_index;
//    __DMB();                                    /* read index before data */
//    MyPrintfBlock_t *block = (MyPrintfBlock_t *)ipc_message.printf_queue.queue[idx];
//
//    MyPrintf("CM4 received: %s", block->buff);
//
//    uint32_t next_rel = (ipc_message.release_queue.write_index + 1U) % MY_PRINTF_RELEASE_QUEUE_SIZE;
//    if (next_rel != ipc_message.release_queue.read_index)
//    {
//      ipc_message.release_queue.queue[ipc_message.release_queue.write_index] = (uintptr_t)block;
//      __DMB();
//      ipc_message.release_queue.write_index = next_rel;
//    }
//
//    ipc_message.printf_queue.read_index = (idx + 1U) % MY_PRINTF_QUEUE_SIZE;
//  }
//}
//
///**
//  * @brief Forward a pending CM7 command to the ADC or DAC thread.
//  */
//static void IPC_HandleCommand(void)
//{
//  IPC_Command_t cmd = ipc_message.command;
//
//  if (cmd == IPC_CMD_NONE)
//  {
//    return;
//  }
//  ipc_message.command = IPC_CMD_NONE;
//
//  switch (cmd)
//  {
//    case IPC_CMD_ADC_START:  (void)osThreadFlagsSet(ADC_THREADHandle, ADC_CMD_START_FLAG); break;
//    case IPC_CMD_ADC_STOP:   (void)osThreadFlagsSet(ADC_THREADHandle, ADC_CMD_STOP_FLAG);  break;
//    case IPC_CMD_DAC_SINE:   (void)osThreadFlagsSet(DACHandle, DAC_SINE_FLAG);            break;
//    case IPC_CMD_DAC_SQUARE: (void)osThreadFlagsSet(DACHandle, DAC_SQUARE_FLAG);          break;
//    default: break;
//  }
//}
//
///* ---------------- DAC ---------------- */
//
///**
//  * @brief Stop DAC output, refill the lookup table, restart.
//  * @param fill Function that fills dac_buffer.
//  * @param name Waveform name for printout.
//  */
//static void DAC_Restart(void (*fill)(void), const char *name)
//{
//  (void)HAL_TIM_Base_Stop(&htim2);
//  (void)HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
//
//  fill();
//
//  if ((HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_buffer, DAC_LUT_SIZE, DAC_ALIGN_12B_R) != HAL_OK) ||
//      (HAL_TIM_Base_Start(&htim2) != HAL_OK))
//  {
//    MyPrintf("DAC %s start failed", name);
//  }
//  else
//  {
//    MyPrintf("DAC %s started", name);
//  }
//}
//
///** @brief One sine period, 0..DAC_MAX_CODE, centred at mid-scale. */
//static void DAC_FillSine(void)
//{
//  for (uint32_t i = 0U; i < DAC_LUT_SIZE; i++)
//  {
//    float s = sinf(2.0f * DAC_PI * (float)i / (float)DAC_LUT_SIZE);
//    dac_buffer[i] = (uint32_t)((s + 1.0f) * ((float)DAC_MAX_CODE / 2.0f));
//  }
//}
//
///** @brief One square period, 50 % duty: low half, then high half. */
//static void DAC_FillSquare(void)
//{
//  for (uint32_t i = 0U; i < DAC_LUT_SIZE; i++)
//  {
//    dac_buffer[i] = (i < (DAC_LUT_SIZE / 2U)) ? 0U : DAC_MAX_CODE;
//  }
//}
//
///* USER CODE END Application */
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
#include "ipc.h"
#include "my_printf_ipc.h"
#include "my_printf.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "dac.h"
#include "lab3modify.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* ADC double buffer: two halves of ADC_BLOCK_SIZE samples (DMA circular) */
#define ADC_BUFFER_SIZE         (2U * ADC_BLOCK_SIZE)

/* Thread flags for the ADC thread */
#define ADC_HALF_FLAG           (1U << 0)   /* first half of buffer filled  */
#define ADC_FULL_FLAG           (1U << 1)   /* second half of buffer filled */
#define ADC_ERROR_FLAG          (1U << 2)   /* HAL_ADC_ErrorCallback        */
#define ADC_CMD_START_FLAG      (1U << 3)   /* START command from CM7       */
#define ADC_CMD_STOP_FLAG       (1U << 4)   /* STOP command from CM7        */

/* Thread flags for the DAC thread */
#define DAC_SINE_FLAG           (1U << 5)
#define DAC_SQUARE_FLAG         (1U << 6)

/* DAC waveform: one period = DAC_LUT_SIZE samples at TIM2 rate (100 kHz -> 1 kHz) */
#define DAC_LUT_SIZE            100U
#define DAC_MAX_CODE            4095U       /* 12-bit DAC full scale */
#define DAC_PI                  3.14159265f

/* Command polling period of the CM4 default task */
#define CMD_POLL_PERIOD_MS      10U
#define HELLO_PERIOD_TICKS      100U        /* 100 x 10 ms = 1 s */

/* Debug printout of samples after STOP */
#define DBG_SAMPLE_COUNT        100U
#define DBG_SAMPLE_STEP         10U

/* Logic analyser pins */
#define DBG_HALF_FULL_PORT      GPIOA       /* high = 1st half done, low = 2nd half done */
#define DBG_HALF_FULL_PIN       GPIO_PIN_3
#define DBG_CM4_COPY_PORT       GPIOB       /* high while copying to SRAM4 */
#define DBG_CM4_COPY_PIN        GPIO_PIN_1
#define LED_YELLOW_PORT         GPIOE
#define LED_YELLOW_PIN          GPIO_PIN_1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static uint16_t adc_buffer[ADC_BUFFER_SIZE];   /* DMA target, D2 SRAM */
static uint32_t dac_buffer[DAC_LUT_SIZE];      /* DAC lookup table    */
static volatile uint8_t adc_running = 0U;

static volatile uint32_t cb_count      = 0U;   /* half/full callbacks seen          */
static volatile uint32_t copy_count    = 0U;   /* blocks copied to SRAM4            */
static volatile uint32_t overrun_count = 0U;   /* blocks dropped: CM7 still owns it */
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ADC_THREAD */
osThreadId_t ADC_THREADHandle;
const osThreadAttr_t ADC_THREAD_attributes = {
  .name = "ADC_THREAD",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DAC */
osThreadId_t DACHandle;
const osThreadAttr_t DAC_attributes = {
  .name = "DAC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void DAC_FillSine(void);
static void DAC_FillSquare(void);
static void DAC_Restart(void (*fill)(void), const char *name);
static void ADC_CopyBlockToShared(uint32_t slot);
static void IPC_HandlePrintfQueue(void);
static void IPC_HandleCommand(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  (void)MyPrintfInit(&huart3);
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

  /* creation of ADC_THREAD */
  ADC_THREADHandle = osThreadNew(StartTask02, NULL, &ADC_THREAD_attributes);

  /* creation of DAC */
  DACHandle = osThreadNew(StartTask03, NULL, &DAC_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Default task: prints CM7 messages, forwards CM7 commands, blinks yellow LED.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  uint32_t hello_ticks = 0U;

  for (;;)
  {
    if (++hello_ticks >= HELLO_PERIOD_TICKS)
    {
      hello_ticks = 0U;
      MyPrintf("hello started cm4");
    }

    IPC_HandlePrintfQueue();
    IPC_HandleCommand();

    HAL_GPIO_TogglePin(LED_YELLOW_PORT, LED_YELLOW_PIN);
    osDelay(CMD_POLL_PERIOD_MS);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief ADC thread: start/stop on CM7 command, copy each half buffer to SRAM4.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  MyPrintf("ADC TASK STARTED");

  for (;;)
  {
    uint32_t flags = osThreadFlagsWait(ADC_HALF_FLAG | ADC_FULL_FLAG | ADC_ERROR_FLAG |
                                       ADC_CMD_START_FLAG | ADC_CMD_STOP_FLAG,
                                       osFlagsWaitAny, osWaitForever);

    if (flags & ADC_CMD_START_FLAG)
    {
      MyPrintf("ADC START command received");
      if ((HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) ||
          (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK) ||
          (HAL_TIM_Base_Start(&htim3) != HAL_OK))
      {
        MyPrintf("ADC start failed");
      }
      else
      {
        adc_running = 1U;
      }
    }

    if ((flags & ADC_HALF_FLAG) && adc_running)
    {
      ADC_CopyBlockToShared(0U);
    }

    if ((flags & ADC_FULL_FLAG) && adc_running)
    {
      ADC_CopyBlockToShared(1U);
    }

    if (flags & ADC_CMD_STOP_FLAG)
    {
      adc_running = 0U;
      (void)HAL_TIM_Base_Stop(&htim3);
      (void)HAL_ADC_Stop_DMA(&hadc1);

      MyPrintf("callbacks %lu, copied %lu, overruns %lu",
               (unsigned long)cb_count, (unsigned long)copy_count,
               (unsigned long)overrun_count);

      for (uint32_t i = 0U; i < DBG_SAMPLE_COUNT; i += DBG_SAMPLE_STEP)
      {
        MyPrintf("ADC[%lu] = %u  pool0 = %u  pool1 = %u", (unsigned long)i,
                 adc_buffer[i], adc_buffer_pool[0].samples[i], adc_buffer_pool[1].samples[i]);
      }
      MyPrintf("ADC STOP command received");
    }

    if (flags & ADC_ERROR_FLAG)
    {
      MyPrintf("ADC ERROR");
    }
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief DAC thread: switch between sine and square on CM7 command.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  for (;;)
  {
    uint32_t flags = osThreadFlagsWait(DAC_SINE_FLAG | DAC_SQUARE_FLAG,
                                       osFlagsWaitAny, osWaitForever);

    if (flags & DAC_SINE_FLAG)
    {
      DAC_Restart(DAC_FillSine, "sine");
    }
    else if (flags & DAC_SQUARE_FLAG)
    {
      DAC_Restart(DAC_FillSquare, "square");
    }
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* ---------------- HAL callbacks (ISR context) ---------------- */

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    HAL_GPIO_WritePin(DBG_HALF_FULL_PORT, DBG_HALF_FULL_PIN, GPIO_PIN_SET);
    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_HALF_FLAG);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    HAL_GPIO_WritePin(DBG_HALF_FULL_PORT, DBG_HALF_FULL_PIN, GPIO_PIN_RESET);
    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_FULL_FLAG);
  }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  if (hadc->Instance == ADC1)
  {
    (void)osThreadFlagsSet(ADC_THREADHandle, ADC_ERROR_FLAG);
  }
}

/* ---------------- ADC -> SRAM4 ---------------- */

/**
  * @brief Copy one half of the DMA buffer to the shared SRAM4 slot and tell CM7.
  *        If CM7 has not released the slot yet, the block is dropped (never
  *        overwrite a block CM7 may be reading).
  * @param slot 0 = first half, 1 = second half.
  */
static void ADC_CopyBlockToShared(uint32_t slot)
{
  cb_count++;

  if (ipc_adc_message.ready[slot] == 1U)
  {
    overrun_count++;
    return;
  }

  HAL_GPIO_WritePin(DBG_CM4_COPY_PORT, DBG_CM4_COPY_PIN, GPIO_PIN_SET);
  memcpy((void *)adc_buffer_pool[slot].samples,
         &adc_buffer[slot * ADC_BLOCK_SIZE],
         ADC_BLOCK_SIZE * sizeof(uint16_t));
  __DMB();                                      /* data visible before flag */
  ADC_SetCommand((slot == 0U) ? IPC_ADC_HALF : IPC_ADC_FULL);
  HAL_GPIO_WritePin(DBG_CM4_COPY_PORT, DBG_CM4_COPY_PIN, GPIO_PIN_RESET);

#if (LAB3_NOTIFY_MODE == NOTIFY_HSEM)
  /* Take + release the HSEM: the release raises HSEM1 IRQ on CM7 */
  if (HAL_HSEM_FastTake(HSEM_ID_ADC_READY) == HAL_OK)
  {
    HAL_HSEM_Release(HSEM_ID_ADC_READY, 0U);
  }
#endif

  copy_count++;
}

/* ---------------- IPC from CM7 ---------------- */

/**
  * @brief Print every message CM7 has queued, then hand the buffer back to CM7.
  */
static void IPC_HandlePrintfQueue(void)
{
  while (ipc_message.printf_queue.read_index != ipc_message.printf_queue.write_index)
  {
    uint32_t idx = ipc_message.printf_queue.read_index;
    __DMB();                                    /* read index before data */
    MyPrintfBlock_t *block = (MyPrintfBlock_t *)ipc_message.printf_queue.queue[idx];

    MyPrintf("CM4 received: %s", block->buff);

    uint32_t next_rel = (ipc_message.release_queue.write_index + 1U) % MY_PRINTF_RELEASE_QUEUE_SIZE;
    if (next_rel != ipc_message.release_queue.read_index)
    {
      ipc_message.release_queue.queue[ipc_message.release_queue.write_index] = (uintptr_t)block;
      __DMB();
      ipc_message.release_queue.write_index = next_rel;
    }

    ipc_message.printf_queue.read_index = (idx + 1U) % MY_PRINTF_QUEUE_SIZE;
  }
}

/**
  * @brief Forward a pending CM7 command to the ADC or DAC thread.
  */
static void IPC_HandleCommand(void)
{
  IPC_Command_t cmd = ipc_message.command;

  if (cmd == IPC_CMD_NONE)
  {
    return;
  }
  ipc_message.command = IPC_CMD_NONE;

  switch (cmd)
  {
    case IPC_CMD_ADC_START:  (void)osThreadFlagsSet(ADC_THREADHandle, ADC_CMD_START_FLAG); break;
    case IPC_CMD_ADC_STOP:   (void)osThreadFlagsSet(ADC_THREADHandle, ADC_CMD_STOP_FLAG);  break;
    case IPC_CMD_DAC_SINE:   (void)osThreadFlagsSet(DACHandle, DAC_SINE_FLAG);            break;
    case IPC_CMD_DAC_SQUARE: (void)osThreadFlagsSet(DACHandle, DAC_SQUARE_FLAG);          break;
    default: break;
  }
}

/* ---------------- DAC ---------------- */

/**
  * @brief Stop DAC output, refill the lookup table, restart.
  * @param fill Function that fills dac_buffer.
  * @param name Waveform name for printout.
  */
static void DAC_Restart(void (*fill)(void), const char *name)
{
  (void)HAL_TIM_Base_Stop(&htim2);
  (void)HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);

  fill();

  if ((HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_buffer, DAC_LUT_SIZE, DAC_ALIGN_12B_R) != HAL_OK) ||
      (HAL_TIM_Base_Start(&htim2) != HAL_OK))
  {
    MyPrintf("DAC %s start failed", name);
  }
  else
  {
    MyPrintf("DAC %s started", name);
  }
}

/** @brief One sine period, 0..DAC_MAX_CODE, centred at mid-scale. */
static void DAC_FillSine(void)
{
  for (uint32_t i = 0U; i < DAC_LUT_SIZE; i++)
  {
    float s = sinf(2.0f * DAC_PI * (float)i / (float)DAC_LUT_SIZE);
    dac_buffer[i] = (uint32_t)((s + 1.0f) * ((float)DAC_MAX_CODE / 2.0f));
  }
}

/** @brief One square period, 50 % duty: low half, then high half. */
static void DAC_FillSquare(void)
{
  for (uint32_t i = 0U; i < DAC_LUT_SIZE; i++)
  {
    dac_buffer[i] = (i < (DAC_LUT_SIZE / 2U)) ? 0U : DAC_MAX_CODE;
  }
}

/* USER CODE END Application */
