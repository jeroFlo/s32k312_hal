#include "stddef.h"
#include "GPIO.h"


static GPIO_Callback_t Callback_SW = NULL;



/**
 * @brief Initializing board LED Red
 *
 */
void GPIO_Init_BoardLedRed(void)
{
    IP_SIUL2->MSCR[29] = SIUL2_MSCR_OBE(1) | SIUL2_MSCR_SSS(0);
}
/**
 * @brief Initializing board LED Blue
 *
 */
void GPIO_Init_BoardLedBlue(void)
{
    IP_SIUL2->MSCR[31] = SIUL2_MSCR_OBE(1) | SIUL2_MSCR_SSS(0);
}

/**
 * @brief Initializing board switch
 *
 */
void GPIO_Init_BoardSwitch(void)
{
    IP_SIUL2->MSCR[58] = SIUL2_MSCR_IBE(1)| SIUL2_MSCR_SSS(0);
}

/**
 * @brief Set interruption for board switch
 *
 * @param edge trigger type: FALLING_EDGE, RISING_EDGE or BOTH_EDGE
 * @param enable enabling interruption SIUL module. ENABLE or DISABLE allowed
 */
void GPIO_SetInterrupt_BoardSwitch(edgeType_t edge, boolState_t enable)
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

    GPIO_ClearInterruptFlag_BoardSwitch();
    IP_SIUL2->DIRER0 |= SIUL2_DIRER0_EIRE13_MASK; // enable interruption

    } else {
        IP_SIUL2->DIRER0 &= ~SIUL2_DIRER0_EIRE13_MASK;  // disable interruption
    }
}

/**
 * @brief clear interruption board switch
 * 
 */
void GPIO_ClearInterruptFlag_BoardSwitch(void)
{
    IP_SIUL2->DISR0 = SIUL2_DISR0_EIF13_MASK;
}

/**
 * @brief Enable interruption at NVIC level for board switch
 * 
 */
void NVIC_EnableIRQ_BoardSwitch(void){
    NVIC_EnableIRQ(SIUL_1_IRQn);	//Page 306 Reference Manual
}

/**
 * @brief To check if switch was pressed
 * 
 * @return true (positive number) if pressed 
 */
uint8_t isBoardSwitchPressed(void)
{
	return (uint8_t) (1 == IP_SIUL2->GPDI58); //PTB26
}

/**
 * @brief API interface to process an interruption call
 * 
 * @param callback reference function to be executed
 */
void GPIO_IRQCallback(GPIO_Callback_t callback)
{
	Callback_SW = callback;

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
    	GPIO_ClearInterruptFlag_BoardSwitch();
    	if(Callback_SW != NULL)
    	{

    		Callback_SW();
    	}

    }
}
