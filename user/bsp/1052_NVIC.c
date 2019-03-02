#include "1052_NVIC.h"
void RT1052_NVIC_SetPriority(IRQn_Type IRQn,uint32_t prep,uint32_t subp)
{ 
	uint32_t prioritygroup = 0x00;
  
	prioritygroup=NVIC_GetPriorityGrouping();
	NVIC_SetPriority(IRQn,NVIC_EncodePriority(prioritygroup,prep,subp));
}
void RT1052_NVIC_SetPriorityGrouping(uint8_t grpx)
{
	NVIC_SetPriorityGrouping((uint32_t)grpx);
}
