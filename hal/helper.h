
#define CPU_NOP() __asm volatile("nop")

typedef enum
{
    DISABLE,
    ENABLE
} boolState_t;
