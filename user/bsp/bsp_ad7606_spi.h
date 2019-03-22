#ifndef __BSP_AD7606_SPI_H
#define __BSP_AD7606_SPI_H

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "rtthread.h"
/*
*********************************************************************************************************
*	功能说明: 宏定义、变量定义
*********************************************************************************************************
*/

/* 片选 */
#define AD_CS_value(a)		if (a)	\
					GPIO_PinWrite(GPIO2, 17U, 1U);\
					else		\
					GPIO_PinWrite(GPIO2, 17U, 0U);

/* 复位引脚 */
#define AD_RESET_value(a)	if (a)	\
					GPIO_PinWrite(GPIO2, 13U, 1U);\
					else		\
					GPIO_PinWrite(GPIO2, 13U, 0U);

/* 起始信号 */
#define	AD_CONVST_value(a) if (a)	\
					GPIO_PinWrite(GPIO2, 11U, 1U);\
					else		\
					GPIO_PinWrite(GPIO2, 11U, 0U);

/* 时钟信号 */
#define SCK_value(a)		       if (a)	\
					GPIO_PinWrite(GPIO2, 15U, 1U);\
					else		\
					GPIO_PinWrite(GPIO2, 15U, 0U);


#define BUSY_IS_LOW()				(GPIO_PinRead(GPIO2, 19U) == 0)
#define MISO_IS_HIGH()				(GPIO_PinRead(GPIO1, 19U) == 1)  //DoutA

/*
*********************************************************************************************************
*	功能说明: 函数声明
*********************************************************************************************************
*/
void bsp_spi_InitAD7606(void);
void AD7606_ConfigGPIO(void);
void AD7606_Reset(void);	

void AD7606_Scan(void); 		/* 此函数代码按照时序编写 */
int16_t AD7606_ReadAdc(uint8_t _ch);
uint8_t bsp_spiRead1(void)；





void bsp_spiDelay(void);   //延时函数，需更改

#endif /* __BSP_AD7606_SPI_H */
