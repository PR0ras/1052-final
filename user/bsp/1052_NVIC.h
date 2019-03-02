#ifndef __1052_NVIC__
#define __1052_NVIC__
#include "fsl_common.h"
void RT1052_NVIC_SetPriority(IRQn_Type IRQn,uint32_t prep,uint32_t subp);
void RT1052_NVIC_SetPriorityGrouping(uint8_t grpx);

#endif
