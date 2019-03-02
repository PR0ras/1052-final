#ifndef __BSP_KEY_H
#define __BSP_KEY_H



#include "fsl_common.h"

/******************************************************************
 * 宏定义
  ******************************************************************/
#define KEY0	GPIO_PinRead(GPIO1,5)  //KEY0按键GPIO1_05
#define WK_UP	GPIO_PinRead(GPIO5,0)  //WK_UP按键GPIO5_00

#define KEY0_PRES 	1  	//KEY0按下后返回值
#define WKUP_PRES   2	//WKUP按下后返回值

void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);
void delay_ms(uint32_t count);

/******************************************************************
 * 声明
 ******************************************************************/


#endif /* __BSP_KEY_H */
