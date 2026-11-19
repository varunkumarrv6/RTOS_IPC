/**
 * @file   lab3_config.h
 * @brief  Build-time switches for the Lab 3 experiments.
 *         Shared by CM4 and CM7 (place in Ipc_common/Inc).
 *         Change ONE line, rebuild BOTH cores, run, capture logic analyser + UART.
 *
 *  Experiment ladder used in the report:
 *    1) CPU copy, poll,  I-cache on,  D-cache off   (baseline)
 *    2) MDMA,     poll,  I-cache on,  D-cache off
 *    3) MDMA,     poll,  I-cache on,  D-cache on
 *    4) MDMA,     HSEM,  I-cache on,  D-cache on    (no polling on CM7)
 *  (I-cache off can be added as a "0)" row to show its effect.)
 */
#ifndef LAB3_CONFIG_H
#define LAB3_CONFIG_H

/* ---------------- SRAM4 -> DTCM transfer on CM7 ---------------- */
#define COPY_CPU            0   /* CPU loop: uint16 SRAM4 -> float DTCM in one pass   */
#define COPY_MDMA           1   /* MDMA: uint16 SRAM4 -> uint16 DTCM, then CPU to float */
#define LAB3_COPY_MODE      COPY_MDMA

/* ---------------- How CM7 learns a block is ready ---------------- */
#define NOTIFY_POLL         0   /* CM7 thread checks ready[] every LAB3_POLL_PERIOD_MS */
#define NOTIFY_HSEM         1   /* CM4 releases an HSEM -> HSEM1 IRQ on CM7 -> thread flag */
#define LAB3_NOTIFY_MODE    NOTIFY_HSEM
#define LAB3_POLL_PERIOD_MS 1U  /* only used with NOTIFY_POLL */
#define HSEM_ID_ADC_READY   1U  /* HSEM 0 is used by the boot sequence */

/* ---------------- CM7 caches ---------------- */
#define LAB3_ENABLE_ICACHE  1  /* 0 = disable the I-cache CubeMX enabled */
#define LAB3_ENABLE_DCACHE  1   /* 1 = MPU makes SRAM4 non-cacheable, then D-cache on */

/* ---------------- ADC sample rate (TIM3 on CM4) ---------------- */
/* fs = LAB3_TIMER_CLK_HZ / ((PSC + 1) * (ARR + 1))
 *   199 / 9  -> 100 kHz   (Task 1b nominal)
 *    99 / 4  -> 400 kHz
 *    58 / 1  -> ~1.695 MHz (max-rate experiment)                       */
#define LAB3_TIMER_CLK_HZ   200000000UL
#define LAB3_TIM3_PSC       0U
#define LAB3_TIM3_ARR       40U
#define LAB3_FS_HZ          (LAB3_TIMER_CLK_HZ / (((uint32_t)LAB3_TIM3_PSC + 1U) * ((uint32_t)LAB3_TIM3_ARR + 1U)))

/* ---------------- Test sequence (CM7 control thread) ---------------- */
#define LAB3_WAVE_SQUARE    0   /* 0 = sine, 1 = square */
#define LAB3_RUN_TIME_MS    20000U

#endif /* LAB3_CONFIG_H */
