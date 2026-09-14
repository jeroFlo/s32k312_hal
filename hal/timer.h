#include "S32K312.h"
#include "core_cm7.h"

#define PIT_CLOCK      48000000UL  //  clock frequency after reset


void timer0_Init(uint32_t);
void timer0_Start(void);
void timer0_Stop(void);
void timer0_ClearInterruptFlag(void);
void timer0_ClearFlag(void);
uint8_t isTimer0Expired(void);
void timer0_SetInterrupt(void);

typedef void (*timer0_Callback_t)(void);
void timer0_IRQCallback(timer0_Callback_t);
void NVIC_EnableIRQ_timer0(void);