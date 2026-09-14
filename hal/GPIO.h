/**
 * @file GPIO.h
 * @brief GPIO HAL header and definitions.
 *
 * @author Jenifer Romero Flores
 * @date September 2026
 */ 

#include "S32K312.h"
#include "core_cm7.h"
#include "helper.h"


/**
 * @brief Set PIN type
 * 
 * @param x 0 as general I/O GPIO
 */
#define SIUL2_MSCR_SSS(x) (SIUL2_MSCR_SSS_0((x) & 0x01U)|SIUL2_MSCR_SSS_1(((x) >> 1) & 0x01U)\
		                  |SIUL2_MSCR_SSS_2(((x) >> 2) & 0x01U))
        
/**
* @brief Set the BoardLedRed object
*
* @param state value to assign, LOW or HIGH
*/
#define SET_BOARD_LED_RED(state) (IP_SIUL2->GPDO29 = ~(state))

/**
* @brief Toggle the red board LED
*
*/
#define TOGGLE_BOARD_LED_RED() (IP_SIUL2->GPDO29 ^= 1U)
#define SET_BOARD_LED_BLUE(state) (IP_SIUL2->GPDO31 = ~(state))
#define TOGGLE_BOARD_LED_BLUE() (IP_SIUL2->GPDO31 ^= 1U)

/**
 * @brief trigger type for input interruption
 * 
 */
typedef enum
{
    FALLING_EDGE,
    RISING_EDGE,
    BOTH_EDGE

} edgeType_t;

/**
 * @brief 
 * 
 */
typedef enum
{
    LOW,
    HIGH
} pinState_t;


// GPIO initialization/config 
void GPIO_Init_BoardLedRed(void);
void GPIO_Init_BoardLedBlue(void);
void GPIO_Init_BoardSwitch(void);

// Interruption config for PTB26
void GPIO_SetInterrupt_BoardSwitch(edgeType_t, boolState_t);
void GPIO_ClearInterruptFlag_BoardSwitch(void);
void NVIC_EnableIRQ_BoardSwitch(void);

// GPIO to check if pressed
uint8_t isBoardSwitchPressed(void);

// Interruption callbacks
typedef void (*GPIO_Callback_t)(void);
void GPIO_IRQCallback(GPIO_Callback_t);
