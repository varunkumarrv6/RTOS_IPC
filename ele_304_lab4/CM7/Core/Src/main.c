///* USER CODE BEGIN Header */
///**
//  ******************************************************************************
//  * @file           : main.c
//  * @brief          : Main program body
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
///* Includes ------------------------------------------------------------------*/
//#include "main.h"
//#include "FreeRTOS.h"
//#include "cmsis_os2.h"
//#include "dma.h"
//#include "mdma.h"
//#include "gpio.h"
//
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//#include "math.h"
//#include "ipc.h"
//#include "my_printf_ipc.h"
//#include "my_printf.h"
//#include "arm_math.h"
//#include <stdint.h>
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
//#define ADC_BLOCK_SIZE 1024U
///* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
///*                             demonstration code based on hardware semaphore */
///* This define is present in both CM7/CM4 projects                            */
///* To comment when developping/debugging on a single core                     */
//#define DUAL_CORE_BOOT_SYNC_SEQUENCE
//
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//#ifndef HSEM_ID_0
//#define HSEM_ID_0 (0U) /* HW semaphore 0*/
//#endif
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
//
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
//
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
//
///* USER CODE BEGIN PV */
//volatile uint32_t half_transfer_count = 0;
//volatile uint32_t full_transfer_count = 0;
///* USER CODE END PV */
//
///* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
//void MX_FREERTOS_Init(void);
///* USER CODE BEGIN PFP */
//
///* USER CODE END PFP */
//
///* Private user code ---------------------------------------------------------*/
///* USER CODE BEGIN 0 */
//
//
//
//
//
//
//__attribute__((section(".dtcm_fft"), aligned(16)))
//static float_t fft_input[2][ADC_BLOCK_SIZE];//two buffers for storing and applying fft on for half and full complete
//__attribute__((section(".dtcm_fft"), aligned(16)))
//static float32_t fft_output[2][ADC_BLOCK_SIZE];
//__attribute__((section(".dtcm_fft"), aligned(16)))//aids in conversion from uint16 to float
//static uint16_t raw_in[2][ADC_BLOCK_SIZE];   /* MDMA drops the raw samples here */
//static arm_rfft_fast_instance_f32 rfft;
////void CM7_ProcessADCCommand(void)
////{
////    /* Process first ADC block */
////    if (ipc_adc_message.ready[0] == 1U)
////    {
//////    	MyPrintf("fft half done\r\n");
////        for (uint32_t i = 0; i < ADC_BLOCK_SIZE; i++)
////        {
////            fft_input[0][i] =
////                (float)adc_buffer_pool[0].samples[i];
////        }
////
////        /* Acknowledge only after the DTCM copy is complete */
////        __DMB();
////        ipc_adc_message.ready[0] = 0U;
////        if (arm_rfft_fast_init_f32(&rfft, ADC_BLOCK_SIZE) != ARM_MATH_SUCCESS)
////        {
////            Error_Handler();
////        }
////        arm_rfft_fast_f32(
////            &rfft,
////            fft_input[0],
////            fft_output[0],
////            0U
////        );
////        if(ipc_message.command==IPC_CMD_ADC_STOP)
////        	MyPrintf("%f ",fft_output[0]);
////        half_transfer_count++;
////    }
////
////    /* Process second ADC block */
////    if (ipc_adc_message.ready[1] == 1U)
////    {
//////    	MyPrintf("fft full done\r\n");
////        for (uint32_t i = 0; i < ADC_BLOCK_SIZE; i++)
////        {
////            fft_input[1][i] =
////                (float)adc_buffer_pool[1].samples[i];
////        }
////
////        /* Acknowledge only after the DTCM copy is complete */
////        __DMB();
////        ipc_adc_message.ready[1] = 0U;
////        if (arm_rfft_fast_init_f32(&rfft, ADC_BLOCK_SIZE) != ARM_MATH_SUCCESS)
////        {
////            Error_Handler();
////        }
////        arm_rfft_fast_f32(
////            &rfft,
////            fft_input[1],
////            fft_output[1],
////            0U
////        );
////        if(ipc_message.command==IPC_CMD_ADC_STOP)
////           	MyPrintf("%f ",fft_output[1]);
////        full_transfer_count++;
////    }
////}
//static volatile uint8_t last_block = 0xFFU;
//static uint8_t printed = 0U;
//
////void CM7_ProcessADCCommand(void)
////{
////    /* PART 1: process any block the CM4 has marked as ready */
////    for (uint8_t b = 0U; b < 2U; b++)
////    {
////        if (ipc_adc_message.ready[b] == 1U)
////        {
////            /* copy samples to DTCM, converting uint16 -> float */
////            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* pin goes high: copy starts */
////            for (uint32_t i = 0U; i < ADC_BLOCK_SIZE; i++)
////            {
////                fft_input[b][i] = (float32_t)adc_buffer_pool[b].samples[i];
////            }
////            __DMB();
////            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* pin goes low: copy done */
////
////            ipc_adc_message.ready[b] = 0U;              /* tell CM4 the block is free */
////
////            /* run the FFT */
////            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);      /* FFT starts */
////            arm_rfft_fast_f32(&rfft, fft_input[b], fft_output[b], 0U);
////            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);      /* FFT done */
////
////            last_block = b;
////        }
////    }
////
////    /* PART 2: after STOP, print results once */
////    if ((adc_stop == 2U) && (printed == 0U) && (last_block != 0xFFU))
////    {
////        printed = 1U;
////        const float32_t *o = fft_output[last_block];
////
////        /* 2a: find the strongest bin */
////        uint32_t peak_bin = 1U;
////        float32_t peak_mag = 0.0f;
////        for (uint32_t i = 1U; i < (ADC_BLOCK_SIZE / 2U); i++)
////        {
////            float32_t m = hypotf(o[2U * i], o[2U * i + 1U]);
////            if (m > peak_mag)
////            {
////                peak_mag = m;
////                peak_bin = i;
////            }
////        }
////        MyPrintf("block %u: peak bin %u = %u Hz, mag %d\r\n",
////                 (unsigned)last_block,
////                 (unsigned)peak_bin,
////                 (unsigned)((peak_bin * 100000UL) / ADC_BLOCK_SIZE),
////                 (int)peak_mag);
////        osDelay(20);
////
////        /* 2b: check the tone's period from the raw samples (first 10 crossings) */
////        uint32_t sum = 0U;
////        for (uint32_t i = 0U; i < ADC_BLOCK_SIZE; i++)
////        {
////            sum += adc_buffer_pool[last_block].samples[i];
////        }
////        uint32_t mean = sum / ADC_BLOCK_SIZE;
////        MyPrintf("mean = %u\r\n", (unsigned)mean);
////        osDelay(20);
////
////        uint32_t prev = 0U;
////        uint32_t count = 0U;
////        for (uint32_t i = 1U; (i < ADC_BLOCK_SIZE) && (count < 10U); i++)
////        {
////            uint16_t before = adc_buffer_pool[last_block].samples[i - 1U];
////            uint16_t now    = adc_buffer_pool[last_block].samples[i];
////
////            if ((before < mean) && (now >= mean))      /* signal crosses the mean going up */
////            {
////                MyPrintf("cross at %u, gap %u\r\n", (unsigned)i, (unsigned)(i - prev));
////                prev = i;
////                count++;
////                osDelay(20);                           /* let the CM4 print it */
////            }
////        }
////
////        /* 2c: the bin table, as before */
////        for (uint32_t i = 1U; (i + 7U) < (ADC_BLOCK_SIZE / 2U); i += 8U)
////        {
////            MyPrintf("%u: %d %d %d %d %d %d %d %d\r\n", (unsigned)i,
////                (int)hypotf(o[2*i],      o[2*i+1]),
////                (int)hypotf(o[2*i+2],    o[2*i+3]),
////                (int)hypotf(o[2*i+4],    o[2*i+5]),
////                (int)hypotf(o[2*i+6],    o[2*i+7]),
////                (int)hypotf(o[2*i+8],    o[2*i+9]),
////                (int)hypotf(o[2*i+10],   o[2*i+11]),
////                (int)hypotf(o[2*i+12],   o[2*i+13]),
////                (int)hypotf(o[2*i+14],   o[2*i+15]));
////            osDelay(20);
////        }
////    }
////}
//
//
//static void mdma_copy(uint8_t b)
//{
//    if (HAL_MDMA_Start(&hmdma_mdma_channel0_sw_0,
//                       (uint32_t)&adc_buffer_pool[b].samples[0],
//                       (uint32_t)&raw_in[b][0],
//                       ADC_BLOCK_SIZE * 2U, 1U) != HAL_OK) Error_Handler();
//    HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
//    if (HAL_MDMA_PollForTransfer(&hmdma_mdma_channel0_sw_0,
//                                 HAL_MDMA_FULL_TRANSFER, 10U) != HAL_OK)
//        Error_Handler();
//}
//static uint32_t blocks_total = 0U;
//static uint32_t blocks_torn  = 0U;
//
//void CM7_ProcessADCCommand(void)
//{
//    /* PART 1: process any block the CM4 has marked as ready */
//    for (uint8_t b = 0U; b < 2U; b++)
//    {
//        if (ipc_adc_message.ready[b] == 1U)
//        {
//            /* copy samples to DTCM, converting uint16 -> float  (cpu based)*/
////            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* copy starts */
////            for (uint32_t i = 0U; i < ADC_BLOCK_SIZE; i++)
////            {
////                fft_input[b][i] = (float32_t)adc_buffer_pool[b].samples[i];
////            }
////            __DMB();
////
////            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* copy done */
////
////            ipc_adc_message.ready[b] = 0U;              /* tell CM4 the block is free */
//
////            /* TEST: is the block torn? Compare crossing gaps with the first gap.
////               Works for any fs. With fewer than 3 crossings it cannot judge, so bad stays 0. */
////            float32_t mean = 0.0f;
////            for (uint32_t i = 0U; i < ADC_BLOCK_SIZE; i++)
////            {
////                mean += fft_input[b][i];
////            }
////            mean = mean / (float32_t)ADC_BLOCK_SIZE;
////
////            uint32_t prev = 0U;
////            uint32_t ref_gap = 0U;
////            uint32_t bad = 0U;
////            for (uint32_t i = 1U; i < ADC_BLOCK_SIZE; i++)
////            {
////                if ((fft_input[b][i - 1U] < mean) && (fft_input[b][i] >= mean))
////                {
////                    if (prev != 0U)                     /* need two crossings for a gap */
////                    {
////                        uint32_t gap = i - prev;
////                        if (ref_gap == 0U)
////                        {
////                            ref_gap = gap;              /* first gap is the reference */
////                        }
////                        else if ((gap + 2U < ref_gap) || (gap > ref_gap + 2U))
////                        {
////                            bad++;                      /* gap differs from the reference */
////                        }
////                    }
////                    prev = i;
////                }
////            }
////            blocks_total++;
////            if (bad > 0U) { blocks_torn++; }
//            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* MDMA copy starts */
//            mdma_copy(b);                               /* returns when the copy is complete */
//            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);      /* MDMA copy done */
//
//            ipc_adc_message.ready[b] = 0U;              /* slot free for CM4 */
//
//            for (uint32_t i = 0U; i < ADC_BLOCK_SIZE; i++)       /* uint16 -> float, from DTCM */
//                fft_input[b][i] = (float32_t)raw_in[b][i];
//            /* run the FFT */
//            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);      /* FFT starts */
//            arm_rfft_fast_f32(&rfft, fft_input[b], fft_output[b], 0U);
//            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);      /* FFT done */
//
////            if (bad == 0U) { last_block = b; }          /* only remember clean blocks */
//            blocks_total++;
//            last_block = b;
//        }
//    }
//
//    /* PART 2a: stopped, but no clean block was found - say so instead of staying silent */
//    if ((adc_stop == 2U) && (printed == 0U) && (last_block == 0xFFU))
//    {
//        printed = 1U;
//        MyPrintf("no clean block: total %u, torn %u\r\n",
//                 (unsigned)blocks_total, (unsigned)blocks_torn);
//    }
//
//    /* PART 2b: after STOP, print results once */
//    if ((adc_stop == 2U) && (printed == 0U) && (last_block != 0xFFU))
//    {
//        printed = 1U;
//        const float32_t *o = fft_output[last_block];
//
//        MyPrintf("blocks total %u, torn %u\r\n",
//                 (unsigned)blocks_total, (unsigned)blocks_torn);
//        osDelay(20);
//
//        /* strongest bin (bin 0 = DC is skipped) */
//        uint32_t peak_bin = 1U;
//        float32_t peak_mag = 0.0f;
//        for (uint32_t i = 1U; i < (ADC_BLOCK_SIZE / 2U); i++)
//        {
//            float32_t m = hypotf(o[2U * i], o[2U * i + 1U]);
//            if (m > peak_mag)
//            {
//                peak_mag = m;
//                peak_bin = i;
//            }
//        }
//        MyPrintf("block %u: peak bin %u, mag %d\r\n",
//                 (unsigned)last_block, (unsigned)peak_bin, (int)peak_mag);
//        osDelay(20);
//
//        /* bin k -> k * fs / N ; fs = 100 kHz, N = 1024 -> 97.66 Hz per bin */
//        for (uint32_t i = 0U; i < 64U; i += 4U)
//        {
//            MyPrintf("bins %2u..%2u: %d %d %d %d\r\n", (unsigned)i, (unsigned)(i + 3U),
//                (int)hypotf(o[2U*i],      (i == 0U) ? 0.0f : o[2U*i + 1U]),
//                (int)hypotf(o[2U*i + 2U], o[2U*i + 3U]),
//                (int)hypotf(o[2U*i + 4U], o[2U*i + 5U]),
//                (int)hypotf(o[2U*i + 6U], o[2U*i + 7U]));
//            osDelay(20);
//        }
//    }
//}
///* USER CODE END 0 */
//
///**
//  * @brief  The application entry point.
//  * @retval int
//  */
//int main(void)
//{
//
//  /* USER CODE BEGIN 1 */
//
//  /* USER CODE END 1 */
///* USER CODE BEGIN Boot_Mode_Sequence_0 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//  int32_t timeout;
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
///* USER CODE END Boot_Mode_Sequence_0 */
//
//  /* Enable the CPU Cache */
//
//  /* Enable I-Cache---------------------------------------------------------*/
//  SCB_EnableICache();
//
///* USER CODE BEGIN Boot_Mode_Sequence_1 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
//  /* Wait until CPU2 boots and enters in stop mode or timeout*/
//  timeout = 0xFFFF;
//  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
//  if ( timeout < 0 )
//  {
//  Error_Handler();
//  }
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
//  memset((void *)&cm7_myprintf_pool, 0, sizeof(cm7_myprintf_pool));
//  memset((void *)&cm4_myprintf_pool, 0, sizeof(cm4_myprintf_pool));   /* ADD THIS */
//  memset((void *)&ipc_message,       0, sizeof(ipc_message));
//  memset((void *)&ipc_adc_message,   0, sizeof(ipc_adc_message));
//
///* USER CODE END Boot_Mode_Sequence_1 */
//  /* MCU Configuration--------------------------------------------------------*/
//
//  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();
//
//  /* USER CODE BEGIN Init */
//
//  /* USER CODE END Init */
//
//  /* Configure the system clock */
//  SystemClock_Config();
///* USER CODE BEGIN Boot_Mode_Sequence_2 */
//#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
///* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
//HSEM notification */
///*HW semaphore Clock enable*/
//__HAL_RCC_HSEM_CLK_ENABLE();
///*Take HSEM */
//HAL_HSEM_FastTake(HSEM_ID_0);
///*Release HSEM in order to notify the CPU2(CM4)*/
//HAL_HSEM_Release(HSEM_ID_0,0);
///* wait until CPU2 wakes up from stop mode */
//timeout = 0xFFFF;
//while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
//if ( timeout < 0 )
//{
//Error_Handler();
//}
//#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
///* USER CODE END Boot_Mode_Sequence_2 */
//
//  /* USER CODE BEGIN SysInit */
//
//  /* USER CODE END SysInit */
//
//  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_DMA_Init();
//  MX_MDMA_Init();
//  /* USER CODE BEGIN 2 */
//  // in main(), before osKernelInitialize()
//  if (arm_rfft_fast_init_f32(&rfft, ADC_BLOCK_SIZE) != ARM_MATH_SUCCESS)
//      Error_Handler();
//  /* USER CODE END 2 */
//
//  /* Init scheduler */
//  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
//  MX_FREERTOS_Init();
//
//  /* Start scheduler */
//  osKernelStart();
//
//  /* We should never get here as control is now taken by the scheduler */
//
//  /* Infinite loop */
//  /* USER CODE BEGIN WHILE */
//  while (1)
//  {
//    /* USER CODE END WHILE */
//
//    /* USER CODE BEGIN 3 */
//  }
//  /* USER CODE END 3 */
//}
//
///**
//  * @brief System Clock Configuration
//  * @retval None
//  */
//void SystemClock_Config(void)
//{
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//  /** Supply configuration update enable
//  */
//  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
//
//  /** Configure the main internal regulator output voltage
//  */
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//
//  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
//
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_DIV2;
//  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//  RCC_OscInitStruct.PLL.PLLM = 2;
//  RCC_OscInitStruct.PLL.PLLN = 50;
//  RCC_OscInitStruct.PLL.PLLP = 2;
//  RCC_OscInitStruct.PLL.PLLQ = 5;
//  RCC_OscInitStruct.PLL.PLLR = 2;
//  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
//  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
//  RCC_OscInitStruct.PLL.PLLFRACN = 0;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
//                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
//  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
//  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
//
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
//
///* USER CODE BEGIN 4 */
//
///* USER CODE END 4 */
//
///**
//  * @brief  Period elapsed callback in non blocking mode
//  * @note   This function is called  when TIM7 interrupt took place, inside
//  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
//  * a global variable "uwTick" used as application time base.
//  * @param  htim : TIM handle
//  * @retval None
//  */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  /* USER CODE BEGIN Callback 0 */
//
//  /* USER CODE END Callback 0 */
//  if (htim->Instance == TIM7)
//  {
//    HAL_IncTick();
//  }
//  /* USER CODE BEGIN Callback 1 */
//
//  /* USER CODE END Callback 1 */
//}
//
///**
//  * @brief  This function is executed in case of error occurrence.
//  * @retval None
//  */
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
//  while (1)
//  {
//  }
//  /* USER CODE END Error_Handler_Debug */
//}
//#ifdef USE_FULL_ASSERT
///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t *file, uint32_t line)
//{
//  /* USER CODE BEGIN 6 */
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//  /* USER CODE END 6 */
//}
//#endif /* USE_FULL_ASSERT */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "dma.h"
#include "mdma.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
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
/* DUAL_CORE_BOOT_SYNC_SEQUENCE: Define for dual core boot synchronization    */
/*                             demonstration code based on hardware semaphore */
/* This define is present in both CM7/CM4 projects                            */
/* To comment when developping/debugging on a single core                     */
#define DUAL_CORE_BOOT_SYNC_SEQUENCE

#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */

/* SRAM4 (D3) holds all shared IPC data; must be non-cacheable if D-cache is on */
#define SRAM4_BASE_ADDR   0x38000000UL
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
#if (LAB3_ENABLE_DCACHE == 1)
static void MPU_Config_SRAM4_NonCacheable(void);
#endif
static void IPC_ResetSharedVariables(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  int32_t timeout;
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_0 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */

  /* Cache experiment switches (lab3_config.h) */
#if (LAB3_ENABLE_ICACHE == 0)
  SCB_DisableICache();                     /* undo the CubeMX-generated enable */
#endif
#if (LAB3_ENABLE_DCACHE == 1)
  MPU_Config_SRAM4_NonCacheable();         /* shared data must bypass the cache */
  SCB_EnableDCache();
#endif

  /* CM4 is still in STOP mode: safe to clear all shared SRAM4 data */
  IPC_ResetSharedVariables();
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
#if defined(DUAL_CORE_BOOT_SYNC_SEQUENCE)
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
#endif /* DUAL_CORE_BOOT_SYNC_SEQUENCE */
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_MDMA_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief Zero every shared object in SRAM4. Called while CM4 is in STOP mode.
  *        The sections are NOLOAD, so startup code never clears them.
  */
static void IPC_ResetSharedVariables(void)
{
  memset((void *)&ipc_message,       0, sizeof(ipc_message));
  memset((void *)&ipc_adc_message,   0, sizeof(ipc_adc_message));
  memset((void *)&cm7_myprintf_pool, 0, sizeof(cm7_myprintf_pool));
  memset((void *)&cm4_myprintf_pool, 0, sizeof(cm4_myprintf_pool));
  memset((void *)adc_buffer_pool,    0, sizeof(adc_buffer_pool));
}

#if (LAB3_ENABLE_DCACHE == 1)
/**
  * @brief Make the whole 64 KB SRAM4 non-cacheable, shareable for the CM7.
  *        Without this, CM7 would read stale cached copies of data CM4 wrote.
  *        DTCM is never cached, so the FFT buffers need nothing.
  */
static void MPU_Config_SRAM4_NonCacheable(void)
{
  MPU_Region_InitTypeDef region = {0};

  HAL_MPU_Disable();

  region.Enable           = MPU_REGION_ENABLE;
  region.Number           = MPU_REGION_NUMBER0;
  region.BaseAddress      = SRAM4_BASE_ADDR;
  region.Size             = MPU_REGION_SIZE_64KB;
  region.SubRegionDisable = 0x00U;
  region.TypeExtField     = MPU_TEX_LEVEL1;          /* TEX=1, C=0, B=0: normal, non-cacheable */
  region.AccessPermission = MPU_REGION_FULL_ACCESS;
  region.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  region.IsShareable      = MPU_ACCESS_SHAREABLE;
  region.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
  region.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  HAL_MPU_ConfigRegion(&region);

  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  MyPrintf("CM7 ASSERT failed: %s line %lu", (char *)file, (unsigned long)line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
