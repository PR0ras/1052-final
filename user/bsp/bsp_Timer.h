#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "fsl_common.h"
#include "1052_NVIC.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_qtmr.h"
#include "fsl_pit.h"

void ENCInit(void);
int16_t DisCNT(void);
void PIT_CH0_Int_Init(uint32_t ldval);
#endif /* __BSP_TIMER_H */