


#include "../hal/GPIO.h"
#include "app.h"

/******************************************************************************
 * GPIO Bare Metal
 * S32K312
 ******************************************************************************/

/**
 * @brief basic delay implemented with nop cmds
 * 
 * @param t number of nop to perform
 */
static void delay(volatile uint32_t t)
{
    while(t--)
    {
        __asm volatile("nop");
    }
}

/**
 * @brief Initializing board LEDs
 *
 * @param pin Accepted values 29, 30, 31, otherwise
 *          RED(29) as default. See schematic page 12
 */
static void GPIO_Init_BoardLed(uint32_t pin)
{
    // validating PIN input
    if (!(pin ^ APP_BOARD_LED_GREEN))
    {
        OGPIO_Init_PTA30();
    } else if (!(pin ^ APP_BOARD_LED_BLUE)) {
        OGPIO_Init_PTA31();
    } else {
        OGPIO_Init_PTA29();
    }
}

/**
 * @brief Initializing board switches/buttons
 *
 * @param pin Accepted values 19, 26, otherwise SW2(19) as default
 */
static void GPIO_Init_BoardSwitch(uint32_t pin)
{
    // validating PIN input
    if (~(pin ^ APP_BOARD_SW3))
    {
        IGPIO_Init_PTB26();
    } else {
        IGPIO_Init_PTB19();
    }
}

/**
 * @brief Set the BoardLed object
 * 
 * @param pin pin number to be set
 * @param state value to assign, ENABLE or DISABLE 
 */
static void set_BoardLed(uint32_t pin, boolState_t state){

     // validating PIN input
    if (!(pin ^ APP_BOARD_LED_GREEN))
    {
        GPIO_Set_PTA30(state);
    } else if (!(pin ^ APP_BOARD_LED_BLUE)) {
        GPIO_Set_PTA31(state);
    } else {
        GPIO_Set_PTA29(state);
    }
}

/**
 * @brief toggle the pin led passed
 * 
 * @param pin  pin number to be set
 */
static void toggle_BoardLed(uint32_t pin){

     // validating PIN input
    if (!(pin ^ APP_BOARD_LED_GREEN))
    {
        GPIO_Toggle_PTA30();
    } else if (!(pin ^ APP_BOARD_LED_BLUE)) {
        GPIO_Toggle_PTA31();
    } else {
        GPIO_Toggle_PTA29();
    }
}

/**
 * @brief Interruption handler/accion for PTB26
 * 
 */
static void GPIO_Handler_PTB26(void)
{
	set_BoardLed(APP_BOARD_TEST_LED, ENABLE);
}

int main(void)
{


    GPIO_Init_BoardLed(APP_BOARD_LED_GREEN);
    GPIO_Init_BoardLed(APP_BOARD_TEST_LED);

    IGPIO_Init_PTB26();
	GPIO_SetInterrupt_PTB26(FALLING_EDGE, ENABLE);
    NVIC_EnableIRQ_PTB26();

    GPIO_IRQCallback(GPIO_Handler_PTB26);

    //CPU IRQ, ENABLE BY DEFAULT, NO NEED IT
    //__enable_irq();   //NVIC: Nested Vector Interrupt Controller

    while(1)
    {
   	// do { // Polling routine
    //        set_BoardLed(APP_BOARD_TEST_LED, ENABLE);
    //        set_BoardLed(APP_BOARD_LED_GREEN, DISABLE);
    //    }
    //    while (0 == GPIO_Get_PTB26());

        set_BoardLed(APP_BOARD_TEST_LED, DISABLE);
        set_BoardLed(APP_BOARD_LED_GREEN, ENABLE);

        delay(5000000);
        set_BoardLed(APP_BOARD_LED_GREEN, DISABLE);
        delay(5000000);



    }
    return 0;
}
