#include "stddef.h"
#include "timer.h"



static timer0_Callback_t Callback_t0 = NULL;


/**
 * @brief Enable Timer0 module and load period
 * 
 * @param period miliseconds
 */
void timer0_Init(uint32_t period)
{
    IP_PIT_0->MCR = 0; // enable PIT module
    IP_PIT_0->TIMER[0].LDVAL =(PIT_CLOCK / 1000UL) * period - 1UL; // load period
}
/**
 * @brief Starts timer0 counting
 * 
 */
void timer0_Start(void)
{
    IP_PIT_0->TIMER[0].TCTRL |= PIT_TCTRL_TEN(1);
}
/**
 * @brief Stops timer0 
 * 
 */
void timer0_Stop(void)
{
    IP_PIT_0->TIMER[0].TCTRL = PIT_TCTRL_TEN(0);
}
/**
 * @brief Clear interruption flag for timer0
 * 
 */
void timer0_ClearInterruptFlag(void)
{
    IP_PIT_0->TIMER[0].TFLG = PIT_TFLG_TIF_MASK;
}
/**
 * @brief Clear timer0 flag, wrapper of interruptionFlag
 * 
 */
void timer0_ClearFlag(void)
{
    timer0_ClearInterruptFlag();
}
/**
 * @brief To check if timer0 finishes the counting
 * 
 * @return uint8_t 
 */
uint8_t isTimer0Expired(void)
{
	return (uint8_t) IP_PIT_0->TIMER[0].TFLG & PIT_TFLG_TIF_MASK;
}
/**
 * @brief Enable interruption in timer0
 * 
 */
void timer0_SetInterrupt(void)
{
    IP_PIT_0->TIMER[0].TCTRL |= PIT_TCTRL_TIE(1);
}

/**
 * @brief API interface to process an interruption call
 * 
 * @param callback reference function to be executed
 */
void timer0_IRQCallback(timer0_Callback_t callback)
{
	Callback_t0 = callback;

}

/**
 * @brief TImer ISR
 * 
 */
#define __INTERRUPT_PIT0  __attribute__ ((interrupt ("PIT0")))
__INTERRUPT_PIT0 void PIT0_Handler(void)
{
    timer0_ClearInterruptFlag();

    if(Callback_t0 != NULL)
    {

    	Callback_t0();
    }
}

/**
 * @brief Enable interruption at NVIC level for timer0
 * 
 */
void NVIC_EnableIRQ_timer0(void){
    NVIC_EnableIRQ(PIT0_IRQn);
}

