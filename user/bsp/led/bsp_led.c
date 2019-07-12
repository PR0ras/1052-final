/**
  ******************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   led应用函数接口
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
	
#include "pad_config.h"  
#include "./led/bsp_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
 void Init_delay(uint16_t i)
 {
   uint16_t j=100;
   while(j--)
    while(i--)
    ;
 }
void Init_OK(void)
{
uint8_t i;

    BEE1(1);
    RGB_LED_COLOR_RED;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_GREEN;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_BLUE;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_CYAN;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_PURPLE;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_YELLOW;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_WHITE;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);

    BEE1(0);
    RGB_LED_COLOR_OFF;
    for(i=0;i<20;i++)
    Init_delay(LED_DELAY_COUNT);

for(i=0;i<2;i++)
  {
    BEE1(1);
    RGB_LED_COLOR_RED;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_GREEN;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_BLUE;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_CYAN;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_PURPLE;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_YELLOW;
    Init_delay(LED_DELAY_COUNT);
    RGB_LED_COLOR_WHITE;
    Init_delay(LED_DELAY_COUNT);

    BEE1(0);
    RGB_LED_COLOR_OFF;
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);
    Init_delay(LED_DELAY_COUNT);

  }  
} 
void BEE_GPIO_Config(void)
{
  gpio_pin_config_t bee_config;

  IOMUXC_SetPinMux(
  BEE_IOMUXC,    /* 配置为普通IO */
  0U);                /* 不使用SION功能 */
		
	/*设置引脚功能*/
	IOMUXC_SetPinConfig(
  BEE_IOMUXC,        
  SRE_0_SLOW_SLEW_RATE|
  DSE_6_R0_6|
  SPEED_2_MEDIUM_100MHz|
  ODE_0_OPEN_DRAIN_DISABLED|
  PKE_0_PULL_KEEPER_DISABLED|
  PUE_0_KEEPER_SELECTED|
  PUS_0_100K_OHM_PULL_DOWN|
  HYS_0_HYSTERESIS_DISABLED
  );
   
  bee_config.direction = kGPIO_DigitalOutput; //输出模式
  bee_config.outputLogic =  0;                //默认低电平
  bee_config.interruptMode = kGPIO_NoIntmode; //不使用中断

  GPIO_PinInit(BEE_GPIO, BEE_GPIO_PIN, &bee_config);
}


void LED_GPIO_Config(void)
{	
	  
		/* 定义gpio初始化配置结构体 */
    gpio_pin_config_t led_config;
		
    /** 核心板的LED灯，IOMUXC MUX及PAD配置 **/
		
    /* 设置引脚的复用模式 */
		IOMUXC_SetPinMux(
      CORE_BOARD_LED_IOMUXC,    /* 配置为普通IO */
      0U);                      /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      CORE_BOARD_LED_IOMUXC,        
      SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );	
    /* 配置说明 : */
		/* 转换速率: 转换速率慢
			驱动强度: R0/6 
			带宽配置 : medium(100MHz)
			开漏配置: 关闭 
			拉/保持器配置: 关闭
			拉/保持器选择: 保持器（上面已关闭，配置无效）
			上拉/下拉选择: 100K欧姆下拉（上面已关闭，配置无效）
			滞回器配置: 关闭 */      
      
     /* RGB LED灯，使用同样的IOMUXC MUX及PAD配置 */

     #if defined  (USE_LCD) && (USE_LCD == 0U)
  
    IOMUXC_SetPinMux(
      RGB_RED_LED_IOMUXC,    /* 配置为普通IO */
      0U);                  /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      RGB_RED_LED_IOMUXC,        
      SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );	
                      
    IOMUXC_SetPinMux(
      RGB_GREEN_LED_IOMUXC,    /* 配置为普通IO */
      0U);                 /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      RGB_GREEN_LED_IOMUXC,        
      SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );	
                      
    IOMUXC_SetPinMux(
      RGB_BLUE_LED_IOMUXC,    /* 配置为普通IO */
      0U);                /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      RGB_BLUE_LED_IOMUXC,        
     SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );	
                      
    IOMUXC_SetPinMux(
      RGB_WHITE_LED_IOMUXC,    /* 配置为普通IO */
      0U);                /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      RGB_WHITE_LED_IOMUXC,        
     SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );    
    #endif

      
     /** 核心板的LED灯，GPIO配置 **/       
    led_config.direction = kGPIO_DigitalOutput; //输出模式
    led_config.outputLogic =  1;                //默认高电平
    led_config.interruptMode = kGPIO_NoIntmode; //不使用中断
		
		/* 初始化 LED GPIO. */
    GPIO_PinInit(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, &led_config);
    
    #if defined  (USE_LCD) && (USE_LCD == 0U)
    /* 使用同样的LED config配置RGB LED灯 */
    GPIO_PinInit(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, &led_config);
    GPIO_PinInit(RGB_GREEN_LED_GPIO, RGB_GREEN_LED_GPIO_PIN, &led_config);
    GPIO_PinInit(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, &led_config);
    GPIO_PinInit(RGB_WHITE_LED_GPIO, RGB_WHITE_LED_GPIO_PIN, &led_config);
    #endif
    
    
}

/*********************************************END OF FILE**********************/
