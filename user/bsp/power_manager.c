#include "power_manager.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
	
#include "pad_config.h"  

void Peripheral_power_ON()
{
    gpio_pin_config_t led_config;
		
    /** 核心板的LED灯，IOMUXC MUX及PAD配置 **/
		
    /* 设置引脚的复用模式 */
		IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_B0_14_GPIO1_IO14,    /* 配置为普通IO */
      0U);                      /* 不使用SION功能 */
		
		/*设置引脚功能*/
		IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_B0_14_GPIO1_IO14,        
      SRE_0_SLOW_SLEW_RATE|
      DSE_6_R0_6|
      SPEED_2_MEDIUM_100MHz|
      ODE_0_OPEN_DRAIN_DISABLED|
      PKE_0_PULL_KEEPER_DISABLED|
      PUE_0_KEEPER_SELECTED|
      PUS_0_100K_OHM_PULL_DOWN|
      HYS_0_HYSTERESIS_DISABLED
      );

    led_config.direction = kGPIO_DigitalOutput; //输出模式
    led_config.outputLogic =  1;                //默认高电平
    led_config.interruptMode = kGPIO_NoIntmode; //不使用中断

    GPIO_PinInit(GPIO1, 14U, &led_config);

}
