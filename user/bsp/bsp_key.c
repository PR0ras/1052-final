#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 
#include "bsp_key.h"

uint8_t keypres=0;

void KEY_Init(void)
{
		gpio_pin_config_t key_config;
	
	//IO��������
	IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);	        //SNVS_WAKEUP����ΪALT5,��GPIO5_00
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);	    //GPIO_AD_B0_05����ΪALT5,��GPIO1_IO05
	IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
	key_config.direction=kGPIO_DigitalInput;	//����
	key_config.interruptMode=kGPIO_IntLowLevel;	//��ʹ���жϹ���
	key_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO5,0,&key_config); 			//��ʼ��GPIO5_00 
	GPIO_PinInit(GPIO1,5,&key_config); 			//��ʼ��GPIO1_05
	
	/* ��IOMUXC_SNVS ʱ�� */
  CLOCK_EnableClock(kCLOCK_IomuxcSnvs);    

  /* ����GPIO�˿��ж� */
  EnableIRQ(GPIO5_Combined_0_15_IRQn);
  EnableIRQ(GPIO1_Combined_0_15_IRQn);
  
  /* ����GPIO�˿�ĳ�����ŵ��ж� */
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
					__asm("NOP"); /* ����nop��ָ�� */
			}
}

void GPIO5_Combined_0_15_IRQHandler(void)
{
	/* ����жϱ�־λ */
	GPIO_PortClearInterruptFlags(GPIO5,1U << 0U);
	/* ���ð����жϱ�־ */
		keypres = WKUP_PRES;;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}

void GPIO1_Combined_0_15_IRQHandler(void)
{
	/* ����жϱ�־λ */
	GPIO_PortClearInterruptFlags(GPIO1,1U << 5U);
	/* ���ð����жϱ�־ */
	keypres = KEY0_PRES;
#if defined __CORTEX_M && (__CORTEX_M == 4U)
	__DSB();
#endif
}
