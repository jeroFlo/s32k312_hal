
/**
 * @file helper.h
 * @brief Structures and definitiones useful across peripherals
 *
 * @author Jenifer Romero Flores
 * @date September 2026
 */ 

#define CPU_NOP() __asm volatile("nop")
#define SIUL2_MSCR_SSS(x) (SIUL2_MSCR_SSS_0((x) & 0x01U)|SIUL2_MSCR_SSS_1(((x) >> 1) & 0x01U)\
		                  |SIUL2_MSCR_SSS_2(((x) >> 2) & 0x01U))
#define AIPS_SLOW_CLK 24000000UL
typedef enum
{
    DISABLE,
    ENABLE
} boolState_t;
