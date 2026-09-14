
/**
 * @file helper.h
 * @brief Structures and definitiones useful across peripherals
 *
 * @author Jenifer Romero Flores
 * @date September 2026
 */ 

#define CPU_NOP() __asm volatile("nop")

typedef enum
{
    DISABLE,
    ENABLE
} boolState_t;
