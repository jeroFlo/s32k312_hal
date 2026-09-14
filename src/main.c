


#include "../hal/GPIO.h"
#include "app.h"

/******************************************************************************
 * GPIO Bare Metal
 * S32K312
 ******************************************************************************/
//#define GPIO_TEST

#ifdef GPIO_TEST

/**
 * @brief basic delay implemented with nop cmds
 * 
 * @param t number of nop to perform
 */
static void delay(volatile uint32_t t)
{
    while(t--)
    {
        CPU_NOP();
    }
}


/**
 * @brief Interruption handler/accion for board switch
 * 
 */
static void GPIO_Handler_BoardSwitch(void)
{
	SET_BOARD_LED_BLUE(HIGH);
    SET_BOARD_LED_RED(LOW);
}


int main(void)
{

    // Initializing LEDs and SW
    GPIO_Init_BoardLedRed();
    GPIO_Init_BoardLedBlue();
    GPIO_Init_BoardSwitch();

    // Configuring SW interruption    
	GPIO_SetInterrupt_BoardSwitch(FALLING_EDGE, ENABLE);
    NVIC_EnableIRQ_BoardSwitch();
    GPIO_IRQCallback(GPIO_Handler_BoardSwitch);

    //CPU IRQ, ENABLE BY DEFAULT, NO NEED IT
    //__enable_irq();   //NVIC: Nested Vector Interrupt Controller

    while(1)
    {
    	// do
    	// { // Polling routine
        //     SET_BOARD_LED_BLUE(HIGH);
        //     SET_BOARD_LED_RED(LOW);
        // }
        // while (isBoardSwitchPressed());

        SET_BOARD_LED_BLUE(LOW);
        SET_BOARD_LED_RED(HIGH);

        delay(CLOCK_CYCLES);
        SET_BOARD_LED_RED(LOW);
        delay(CLOCK_CYCLES);



    }
    return 0;
}
#else

#include "../hal/timer.h"

/**
 * @brief Interruption handler/accion for timer
 * 
 */
static void timer0_Handler(void)
{
	TOGGLE_BOARD_LED_BLUE();
}

int main(void)
{

    GPIO_Init_BoardLedBlue();
    timer0_Init(TIMER_PERIOD_MS);

    timer0_SetInterrupt();
    timer0_IRQCallback(timer0_Handler);
    NVIC_EnableIRQ_timer0();
    
    timer0_Start();



    while (1)
    {
        // // polling routinr
        // if (isTimer0Expired())
        // {
        //     timer0_ClearFlag();
        //     TOGGLE_BOARD_LED_BLUE();
        // }
    }
    return 0;
}


#endif
