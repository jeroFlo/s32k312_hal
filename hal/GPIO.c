#include "stddef.h"
#include "GPIO.h"


static GPIO_Callback_t Callback_PTB26 = NULL;

/**
 * @brief PTA29 configure as GPIO output. See reference manual page 424 & IO signal table
 *
 */
void OGPIO_Init_PTA29(void)
{
	IP_SIUL2->MSCR[29] = SIUL2_MSCR_OBE(1) | SIUL2_MSCR_SSS(0);
}

/**
 * @brief PTA30 configure as GPIO output. See reference manual page 424 & IO signal table
 * 
 */
void OGPIO_Init_PTA30(void)
{
	IP_SIUL2->MSCR[30] = SIUL2_MSCR_OBE(1) | SIUL2_MSCR_SSS(0);
}

/**
 * @brief PTA31 configure as GPIO output. See reference manual page 424 & IO signal table
 * 
 */
void OGPIO_Init_PTA31(void)
{
	IP_SIUL2->MSCR[31] = SIUL2_MSCR_OBE(1) | SIUL2_MSCR_SSS(0);
}

/**
 * @brief PTB19 configure as GPIO input. See reference manual page 424 & IO signal table
 *
 */
void IGPIO_Init_PTB19(void)
{
	IP_SIUL2->MSCR[51] = SIUL2_MSCR_IBE(1)| SIUL2_MSCR_SSS(0);
}

/**
 * @brief PTB26 configure as GPIO input. See reference manual page 424 & IO signal table
 * 
 */
void IGPIO_Init_PTB26(void)
{
	IP_SIUL2->MSCR[58] = SIUL2_MSCR_IBE(1)| SIUL2_MSCR_SSS(0);
}

/**
 * @brief Set interruption for PTB26
 *
 * @param edge trigger type: FALLING_EDGE, RISING_EDGE or BOTH_EDGE
 * @param enable enabling interruption SIUL module. ENABLE or DISABLE allowed
 */
void GPIO_SetInterrupt_PTB26(edgeType_t edge, boolState_t enable)
{
    if (enable)
    {
      // enabling IRQ SIUL
	IP_SIUL2->IMCR[541-512] = SIUL2_MSCR_SSS(2); // see IO signal table for PTB26

    switch (edge)
    {
    case FALLING_EDGE:
        IP_SIUL2->IREER0 &= ~SIUL2_IREER0_IREE13_MASK;
        IP_SIUL2->IFEER0 |= SIUL2_IFEER0_IFEE13_MASK;
        break;
    case RISING_EDGE:
        IP_SIUL2->IREER0 |= SIUL2_IREER0_IREE13_MASK;
        IP_SIUL2->IFEER0 &= ~SIUL2_IFEER0_IFEE13_MASK;
    default:
        IP_SIUL2->IREER0 |= SIUL2_IREER0_IREE13_MASK;
        IP_SIUL2->IFEER0 |= SIUL2_IFEER0_IFEE13_MASK;
        break;
    }

    IP_SIUL2->DIRSR0 &= ~SIUL2_DIRSR0_DIRSR13_MASK;	//Select IRQ, no DMA request

    GPIO_ClearInterruptFlag_PTB26();
    IP_SIUL2->DIRER0 |= SIUL2_DIRER0_EIRE13_MASK; // enable interruption

    } else {
        IP_SIUL2->DIRER0 &= ~SIUL2_DIRER0_EIRE13_MASK;  // disable interruption
    }

}

/**
 * @brief clear interruption for PTB26
 * 
 */
void GPIO_ClearInterruptFlag_PTB26(void)
{
    IP_SIUL2->DISR0 = SIUL2_DISR0_EIF13_MASK;
}

/**
 * @brief Enable interruption at NVIC level for PTB26
 * 
 */
void NVIC_EnableIRQ_PTB26(void){
    NVIC_EnableIRQ(SIUL_1_IRQn);	//Page 306 Reference Manual
}

/**
 * @brief toggle output state for PTA29
 * 
 */
void GPIO_Toggle_PTA29(void)
{
    IP_SIUL2->GPDO29 ^= 1U; //Page 439 Reference Manual
}

/**
 * @brief toggle output state for PTA30
 * 
 */
void GPIO_Toggle_PTA30(void)
{
    IP_SIUL2->GPDO30 ^= 1U;
}

/**
 * @brief toggle output state for PTA31
 * 
 */
void GPIO_Toggle_PTA31(void)
{
    IP_SIUL2->GPDO31 ^= 1U;
}

/**
 * @brief Set output state for PTA29
 * 
 * @param state ENABLE or DISABLE
 */
void GPIO_Set_PTA29(boolState_t state)
{
    IP_SIUL2->GPDO29 = ~state;
}

/**
 * @brief Set output state for PTA30
 * 
 * @param state ENABLE or DISABLE
 */
void GPIO_Set_PTA30(boolState_t state)
{
    IP_SIUL2->GPDO30 = ~state;
}

/**
 * @brief Set output state for PTA31
 * 
 * @param state ENABLE or DISABLE
 */
void GPIO_Set_PTA31(boolState_t state)
{
    IP_SIUL2->GPDO31 = ~state;
}

/**
 * @brief Get input state for PTB26
 * 
 * @return current input state
 */
uint8_t GPIO_Get_PTB26(void)
{
	return (uint8_t) IP_SIUL2->GPDI58;
}

/**
 * @brief API interface to process an interruption call
 * 
 * @param callback reference function to be executed
 */
void GPIO_IRQCallback(GPIO_Callback_t callback)
{
	Callback_PTB26 = callback;

}

/**
 * @brief ISR
 * 
 */
#define __INTERRUPT_SIUL_1  __attribute__ ((interrupt ("SIUL1")))
__INTERRUPT_SIUL_1 void SIUL_1_Handler(void)
{
    if ((IP_SIUL2->DISR0 & SIUL2_DISR0_EIF13_MASK) != 0U)
    {
    	GPIO_ClearInterruptFlag_PTB26();
    	if(Callback_PTB26 != NULL)
    	{

    		Callback_PTB26();
    	}

    }
}
