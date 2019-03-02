#ifndef __BSP_KEY_H
#define __BSP_KEY_H



#include "fsl_common.h"

/******************************************************************
 * �궨��
  ******************************************************************/
#define KEY0	GPIO_PinRead(GPIO1,5)  //KEY0����GPIO1_05
#define WK_UP	GPIO_PinRead(GPIO5,0)  //WK_UP����GPIO5_00

#define KEY0_PRES 	1  	//KEY0���º󷵻�ֵ
#define WKUP_PRES   2	//WKUP���º󷵻�ֵ

void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);
void delay_ms(uint32_t count);

/******************************************************************
 * ����
 ******************************************************************/


#endif /* __BSP_KEY_H */
