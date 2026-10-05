#include "UART.h"
#include "S32K312.h"
#include "core_cm7.h"
#include "GPIO.h"

static void (*Callback_URx)(void) = 0;
ringBuffer_t rxBuffer;

void ringBuffer_init(ringBuffer_t *buffer)
{
    buffer->head = 0U;
    buffer->tail = 0U;
}

int ringBuffer_push(ringBuffer_t *buffer, uint8_t value)
{
    uint16_t nextHead = (uint16_t)((buffer->head + 1U) % UART6_RX_BUFFER_SIZE);

    if (nextHead == buffer->tail) {
        return 0;
    }

    buffer->data[buffer->head] = value;
    buffer->head = nextHead;
    return 1;
}

int ringBuffer_pop(ringBuffer_t *buffer, uint8_t *value)
{
    if (buffer->head == buffer->tail) {
        return 0;
    }

    *value = buffer->data[buffer->tail];
    buffer->tail = (uint16_t)((buffer->tail + 1U) % UART6_RX_BUFFER_SIZE);
    return 1;
}



void UART6_Init(uint32_t baudrate)
{
    ringBuffer_init(&rxBuffer);

    /* Page 2 Schematics
     * TX PTA16 143
     * RX PTA15 145
     *
     * S32K312_IOMUX.xls S32K312_IO Signal Table Tab
     * PTA16 SIUL_MSCR16 = 5 LPUART6_TX
     * PTA15 SIUL_IMCR705= 2 LPUART6_RX
     * */

    IP_SIUL2->MSCR[16] = SIUL2_MSCR_SSS(5)| SIUL2_MSCR_OBE_MASK; //Chapter 10

    IP_SIUL2->MSCR[15]= SIUL2_MSCR_SSS(1) | SIUL2_MSCR_IBE_MASK; // No esta documentado
    IP_SIUL2->IMCR[705-512] = SIUL2_MSCR_SSS(2); 		//

    // Salir de Power Down, esta en bajo consumo de energia por default. 
    // basicamente es prender el clock.

	IP_MC_ME->PRTN1_COFB2_CLKEN |= MC_ME_PRTN1_COFB2_CLKEN_REQ80(1);  //Page 1740 (1745)

	IP_MC_ME->PRTN1_PUPD = 1; // bit the power up 

	IP_MC_ME->CTL_KEY = 0x5AF0; // llaves para poder hacer power up
	IP_MC_ME->CTL_KEY = 0xA50F; // este es el negado del anterior 

	while (IP_MC_ME->PRTN0_PUPD) // verificar que el power up se haya hecho correctamente
	{
		//definir un time out
        // poner un contador de tiempo para que no se quede en un loop infinito
	}

    /*-------------------------------------------------------
      1. Deshabilitar TX y RX
    -------------------------------------------------------*/
    IP_LPUART_6->CTRL = 0;

    /*-------------------------------------------------------
      2. Configurar baudrate
         Default: FIRC 48 MHZ, DIV=2
         Clock UART =  24 MHz
         Baud = baudrate

         SBR = 24MHz/(16*baudrate)=13.02
    -------------------------------------------------------*/

    uint16_t sbr = (uint16_t)(AIPS_SLOW_CLK/(16*baudrate));

    IP_LPUART_6->BAUD =LPUART_BAUD_OSR(15) | LPUART_BAUD_SBR(sbr);  //(15 << 24) | (13);      /* OSR =16 */

    // hay un error de 0.02 lo cual deberia ser suficiente. si la trama es de 10 bits el error debe ser menor al 5%
    // OSR: over sampling rate para el rx se puede cambiar. segun el manual es el numero + 1
    // SBR: baud rate divisor, es el valor que se pone en el registro para que el baud rate sea el correcto.

    /*-------------------------------------------------------
      3. 8N1
    -------------------------------------------------------*/

    IP_LPUART_6->CTRL = 0;

    /*-------------------------------------------------------
      4. Habilitar TX y RX + RX interrupt
    -------------------------------------------------------*/

    IP_LPUART_6->CTRL |= LPUART_CTRL_TE(1) + LPUART_CTRL_RE(1) + LPUART_CTRL_RIE(1); //LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
    // GPIO_Init_BoardLedRed();
    // SET_BOARD_LED_RED(LOW);
    NVIC_EnableIRQ(LPUART6_IRQn);
    //NVIC->ISER[(((uint32_t)LPUART6_IRQn ) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)LPUART6_IRQn) & 0x1FUL));
    // >>5 es para dividir entre 32, ya que cada registro ISER tiene 32 bits

}


void uprintc(char c)
{
    volatile int timeout = 1000000;
    while(!(IP_LPUART_6->STAT & LPUART_STAT_TDRE_MASK) && --timeout);
    if (timeout == 0) {
        return;
    }
    IP_LPUART_6->DATA = c;
}

void uprint(const char *s)
{
    while(*s){
        uprintc(*s++);
    }
}

void UART6_RX_IRQCallback(void (*callback)(void))
{
    Callback_URx = callback;
}

char ugetc(void)
{
    while(!(IP_LPUART_6->STAT & LPUART_STAT_RDRF_MASK));
    // IP_LPUART->DATA es para leer y recibir. Mismo registro para leer y escribir. Cuando se escribe, se pone en la cola de transmision. Cuando se lee, se obtiene el dato recibido.
    // no podemos saber que es lo que escribi, habria que leerlo de nuevo o hacer magia con software
    return (char)IP_LPUART_6->DATA;
}

#define __INTERRUPT_LPUART6  __attribute__ ((interrupt ("LPUART6")))
__INTERRUPT_LPUART6 void LPUART6_Handler(void)
{
    if (IP_LPUART_6->STAT & LPUART_STAT_RDRF_MASK) {
        uint8_t receivedByte = (uint8_t)IP_LPUART_6->DATA;

        ringBuffer_push(&rxBuffer, receivedByte);
        //uprintc((char)receivedByte); // echo
        if (Callback_URx != 0) {
            Callback_URx();
        }
    }

}


