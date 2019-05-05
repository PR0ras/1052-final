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
<<<<<<< HEAD
	key_config.direction=kGPIO_DigitalInput;	//输入
	key_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	key_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO5,0,&key_config); 			//初始化GPIO5_00 
    
   //KEY0默认电平设置
   key_config.direction=kGPIO_DigitalInput;	//输入
	key_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	key_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO1,5,&key_config); 			//初始化GPIO1_05
=======
	key_config.direction=kGPIO_DigitalInput;	//����
	key_config.interruptMode=kGPIO_IntLowLevel;	//��ʹ���жϹ���
	key_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO5,0,&key_config); 			//��ʼ��GPIO5_00 
	GPIO_PinInit(GPIO1,5,&key_config); 			//��ʼ��GPIO1_05
>>>>>>> 00b4bfe05531a4111e967151bb27ae86511aeb9d
	
	/* ��IOMUXC_SNVS ʱ�� */
  CLOCK_EnableClock(kCLOCK_IomuxcSnvs);    

  /* ����GPIO�˿��ж� */
  EnableIRQ(GPIO5_Combined_0_15_IRQn);
  EnableIRQ(GPIO1_Combined_0_15_IRQn);
  
  /* ����GPIO�˿�ĳ�����ŵ��ж� */
  GPIO_PortEnableInterrupts(GPIO5,1U << 0U);        
  GPIO_PortEnableInterrupts(GPIO1,1U << 5U);  

<<<<<<< HEAD
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up=1;     //按键松开标志
    if(mode==1)
			key_up=1;    //支持连按
    if(key_up&&(KEY0==0||WK_UP==0))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       
					return KEY0_PRES;
        else if(WK_UP==0) 
					return WKUP_PRES;      
    }
		else if(KEY0==1&&WK_UP==1)
			key_up=1;
    return 0;   //无按键按下
=======
>>>>>>> 00b4bfe05531a4111e967151bb27ae86511aeb9d
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
