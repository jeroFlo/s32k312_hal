#include "S32K312.h"
#include "core_cm7.h"

#define SIUL2_MSCR_SSS(x) (SIUL2_MSCR_SSS_0((x) & 0x01U)|SIUL2_MSCR_SSS_1(((x) >> 1) & 0x01U)\
		                  |SIUL2_MSCR_SSS_2(((x) >> 2) & 0x01U))

/**
 * @brief 
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
    DISABLE,
    ENABLE
} boolState_t;

// GPIO initialization/config as output
void OGPIO_Init_PTA29(void);
void OGPIO_Init_PTA30(void);
void OGPIO_Init_PTA31(void);

// GPIO initialization/config as input
void IGPIO_Init_PTB19(void);
void IGPIO_Init_PTB26(void);

// Interruption config for PTB26
void GPIO_SetInterrupt_PTB26(edgeType_t, boolState_t);
void GPIO_ClearInterruptFlag_PTB26(void);
void NVIC_EnableIRQ_PTB26(void);

// GPIO set/get 
void GPIO_Toggle_PTA29(void);
void GPIO_Toggle_PTA30(void);
void GPIO_Toggle_PTA31(void);
void GPIO_Set_PTA29(boolState_t);
void GPIO_Set_PTA30(boolState_t);
void GPIO_Set_PTA31(boolState_t);
uint8_t GPIO_Get_PTB26(void);

typedef void (*GPIO_Callback_t)(void);
void GPIO_IRQCallback(GPIO_Callback_t);
