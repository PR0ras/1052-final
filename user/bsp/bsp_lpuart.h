#ifndef __BSP_LPUART__
#define __BSP_LPUART__

#include "fsl_common.h"


void uart_Init(void);
void uartcontrol(uint8_t *tmp);
void uartsend(uint16_t *txuartdata);
void Speedupdate(void);
void uart_Init(void);
void testSend(void);
extern uint8_t camaddress,rxflag;
extern uint16_t camdata;
#endif
