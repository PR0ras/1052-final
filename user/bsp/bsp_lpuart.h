#ifndef __BSP_LPUART__
#define __BSP_LPUART__

#include "fsl_common.h"



void uartcontrol(uint8_t *tmp);
void uartsend(uint16_t *txuartdata);
void Speedupdate(void);
#endif
