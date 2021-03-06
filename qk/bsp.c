/*****************************************************************************
* Product: "Blinky" example, EK-TM4C123GXL board, preemptive QK kernel
* Last updated for version 5.6.0
* Last updated on  2015-12-18
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* http://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpc.h"
#include "blinky.h"
#include "bsp.h"

#include "stm32f3xx.h"
/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

#ifdef Q_SPY
    #error Simple Blinky Application does not provide Spy build configuration
#endif

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs { /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI  /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    SYSTICK_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);

/* Local-scope objects -----------------------------------------------------*/
uint16_t my_dma_buffer[] = {1<<12,0,0,0,0,0,1<<12,1<<11,1<<11,1<<11,1<<11,1<<11};

/* ISRs used in this project ===============================================*/
void SysTick_Handler(void) {
    QK_ISR_ENTRY();   /* inform QK about entering an ISR */
    QF_TICK_X(0U, (void *)0); /* process time events for rate 0 */
    QK_ISR_EXIT();  /* inform QK about exiting an ISR */
}

/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* NOTE: SystemInit() already called from the startup code
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* configure the FPU usage by choosing one of the options... */
#if 1
    /* OPTION 1:
    * Use the automatic FPU state preservation and the FPU lazy stacking.
    *
    * NOTE:
    * Use the following setting when FPU is used in more than one task or
    * in any ISRs. This setting is the safest and recommended, but requires
    * extra stack space and CPU cycles.
    */
//    FPU->FPCCR |= (1U << FPU_FPCCR_ASPEN_Pos) | (1U << FPU_FPCCR_LSPEN_Pos);
#else
    /* OPTION 2:
    * Do NOT to use the automatic FPU state preservation and
    * do NOT to use the FPU lazy stacking.
    *
    * NOTE:
    * Use the following setting when FPU is used in ONE task only and not
    * in any ISR. This setting is very efficient, but if more than one task
    * (or ISR) start using the FPU, this can lead to corruption of the
    * FPU registers. This option should be used with CAUTION.
    */
//    FPU->FPCCR &= ~((1U << FPU_FPCCR_ASPEN_Pos)
//                   | (1U << FPU_FPCCR_LSPEN_Pos));
#endif

    /* enable clock for to the peripherals used by this application... */
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    GPIOE->MODER = (GPIO_MODER_MODER11_0) | (GPIO_MODER_MODER12_0);
    GPIOE->OTYPER = 0x00000000;
    GPIOE->OSPEEDR = 0x00000000;
    GPIOE->PUPDR = 0x00000000;
    GPIOE->ODR |= (1 << 12);

    /* Try GPIO DMA */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel1->CCR |= DMA_CCR_MEM2MEM;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; /* Half word memory size */
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; /* Half word periph size */
    DMA1_Channel1->CCR |= DMA_CCR_MINC; /* Only memory increment mode */
    DMA1_Channel1->CCR |= DMA_CCR_CIRC; /* Circular mode */
    DMA1_Channel1->CCR |= DMA_CCR_DIR; /* Read from memory */
    DMA1_Channel1->CNDTR = 12;
    DMA1_Channel1->CPAR = (uint32_t)&(GPIOE->ODR);
    DMA1_Channel1->CMAR = (uint32_t)&my_dma_buffer;
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}
/*..........................................................................*/
void BSP_ledOn(uint_fast8_t n) {
  //GPIOB->ODR &= ~(1U << 3);
  my_dma_buffer[2] = 1<<12;
  my_dma_buffer[3] = 1<<12;
  my_dma_buffer[4] = 1<<12;
  my_dma_buffer[5] = 1<<12;
  my_dma_buffer[6] = 1<<12;
//    GPIOF->DATA_Bits[l_led_pin[n]] = 0U;
}
/*..........................................................................*/
void BSP_ledOff(uint_fast8_t n) {
  my_dma_buffer[2] = 0<<12;
  my_dma_buffer[3] = 0<<12;
  my_dma_buffer[4] = 0<<12;
  my_dma_buffer[5] = 0<<12;
  my_dma_buffer[6] = 0<<12;
  //GPIOB->ODR |= (1U << 3);
//    GPIOF->DATA_Bits[l_led_pin[n]] = 0xFFU;
}


/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    /* ... */

    /* enable IRQs... */
    //NVIC_EnableIRQ(SysTick_IRQn);

}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QK_onIdle(void) {
    /* toggle LED2 on and then off, see NOTE01 */
    //QF_INT_DISABLE();
//    GPIOF->DATA_Bits[LED_BLUE] = 0xFFU;
//    GPIOF->DATA_Bits[LED_BLUE] = 0x00U;

    //QF_INT_ENABLE();
    //__WFI();
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const *module, int loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;
    QS_ASSERTION(module, loc, (uint32_t)10000U); /* report assertion to QS */
    NVIC_SystemReset();
}

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/
