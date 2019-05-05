#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 
#include "bsp_key.h"

uint8_t keypres=0;

void KEY_Init(void)
{
		gpio_pin_config_t key_config;
	
	//IOåŠŸèƒ½è®¾ç½®
	IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);	        //SNVS_WAKEUPé…ç½®ä¸ºALT5,å³GPIO5_00
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);	    //GPIO_AD_B0_05é…ç½®ä¸ºALT5,å³GPIO1_IO05
	IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
	key_config.direction=kGPIO_DigitalInput;	//ÊäÈë
	key_config.interruptMode=kGPIO_IntLowLevel;	//²»Ê¹ÓÃÖĞ¶Ï¹¦ÄÜ
	key_config.outputLogic=1;					//Ä¬ÈÏ¸ßµçÆ½
	GPIO_PinInit(GPIO5,0,&key_config); 			//³õÊ¼»¯GPIO5_00 
	GPIO_PinInit(GPIO1,5,&key_config); 			//³õÊ¼»¯GPIO1_05
	
	/* ¿ªIOMUXC_SNVS Ê±ÖÓ */
  CLOCK_EnableClock(kCLOCK_IomuxcSnvs);    

  /* ¿ªÆôGPIO¶Ë¿ÚÖĞ¶Ï */
  EnableIRQ(GPIO5_Combined_0_15_IRQn);
  EnableIRQ(GPIO1_Combined_0_15_IRQn);
  
  /* ¿ªÆôGPIO¶Ë¿ÚÄ³¸öÒı½ÅµÄÖĞ¶Ï */
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
					__asm("NOP"); /* è°ƒç”¨nopç©ºæŒ‡ä»¤ */
			}
}

void GPIO5_Combined_0_15_IRQHandler(void)
{
	/* Çå³ıÖĞ¶Ï±êÖ¾Î» */
	GPIO_PortClearInterruptFlags(GPIO5,1U << 0U);
	/* ÉèÖÃ°´¼üÖĞ¶Ï±êÖ¾ */
		keypres = WKUP_PRES;;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}

void GPIO1_Combined_0_15_IRQHandler(void)
{
	/* Çå³ıÖĞ¶Ï±êÖ¾Î» */
	GPIO_PortClearInterruptFlags(GPIO1,1U << 5U);
	/* ÉèÖÃ°´¼üÖĞ¶Ï±êÖ¾ */
	keypres = KEY0_PRES;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}
