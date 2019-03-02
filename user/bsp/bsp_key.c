#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 
#include "bsp_key.h"

void KEY_Init(void)
{
		gpio_pin_config_t key_config;
	
	//IO��������
	IOMUXC_SetPinMux(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0);	        //SNVS_WAKEUP����ΪALT5,��GPIO5_00
  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0);	    //GPIO_AD_B0_05����ΪALT5,��GPIO1_IO05
	IOMUXC_SetPinConfig(IOMUXC_SNVS_WAKEUP_GPIO5_IO00,0xF080);      
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_05_GPIO1_IO05,0xF080);    
	key_config.direction=kGPIO_DigitalInput;	//����
	key_config.interruptMode=kGPIO_NoIntmode;	//��ʹ���жϹ���
	key_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO5,0,&key_config); 			//��ʼ��GPIO5_00 
    
   //KEY0Ĭ�ϵ�ƽ����
   key_config.direction=kGPIO_DigitalInput;	//����
	key_config.interruptMode=kGPIO_NoIntmode;	//��ʹ���жϹ���
	key_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO1,5,&key_config); 			//��ʼ��GPIO1_05
	
}

uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up=1;     //�����ɿ���־
    if(mode==1)
			key_up=1;    //֧������
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
    return 0;   //�ް�������
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
