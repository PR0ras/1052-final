#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 
#include "bsp_key.h"

uint8_t keypres=0;

void KEY_Init(void)
{
		gpio_pin_config_t key_config;
	
	//IO功能设置
	IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);	        //SNVS_WAKEUP配置为ALT5,即GPIO5_00
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);	    //GPIO_AD_B0_05配置为ALT5,即GPIO1_IO05
	IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
	key_config.direction=kGPIO_DigitalInput;	//输入
	key_config.interruptMode=kGPIO_IntLowLevel;	//不使用中断功能
	key_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO5,0,&key_config); 			//初始化GPIO5_00 
	GPIO_PinInit(GPIO1,5,&key_config); 			//初始化GPIO1_05
	
	/* 开IOMUXC_SNVS 时钟 */
  CLOCK_EnableClock(kCLOCK_IomuxcSnvs);    

  /* 开启GPIO端口中断 */
  EnableIRQ(GPIO5_Combined_0_15_IRQn);
  EnableIRQ(GPIO1_Combined_0_15_IRQn);
  
  /* 开启GPIO端口某个引脚的中断 */
  GPIO_PortEnableInterrupts(GPIO5,1U << 0U);        
  GPIO_PortEnableInterrupts(GPIO1,1U << 5U);  

}

void delay_ms(uint32_t count)
{
    static  uint32_t i = 0;
		uint32_t j=0;
		for(j=0;j<100000;++j)
			for (i = 0; i < count; i++)
			{			
					__asm("NOP"); /* 调用nop空指令 */
			}
}

void GPIO5_Combined_0_15_IRQHandler(void)
{
	/* 清除中断标志位 */
	GPIO_PortClearInterruptFlags(GPIO5,1U << 0U);
	/* 设置按键中断标志 */
		keypres = WKUP_PRES;;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}

void GPIO1_Combined_0_15_IRQHandler(void)
{
	/* 清除中断标志位 */
	GPIO_PortClearInterruptFlags(GPIO1,1U << 5U);
	/* 设置按键中断标志 */
	keypres = KEY0_PRES;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}
