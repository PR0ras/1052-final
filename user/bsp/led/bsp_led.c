/**
  ******************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
	
#include "pad_config.h"  
#include "./led/bsp_led.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{	
	  
		/* ����gpio��ʼ�����ýṹ�� */
    gpio_pin_config_t led_config;
		
    /** ���İ��LED�ƣ�IOMUXC MUX��PAD���� **/
		
    /* �������ŵĸ���ģʽ */
		IOMUXC_SetPinMux(
      CORE_BOARD_LED_IOMUXC,    /* ����Ϊ��ͨIO */
      0U);                      /* ��ʹ��SION���� */
		
		/*�������Ź���*/
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
    /* ����˵�� : */
		/* ת������: ת��������
			����ǿ��: R0/6 
			�������� : medium(100MHz)
			��©����: �ر� 
			��/����������: �ر�
			��/������ѡ��: �������������ѹرգ�������Ч��
			����/����ѡ��: 100Kŷķ�����������ѹرգ�������Ч��
			�ͻ�������: �ر� */      
      
     /* RGB LED�ƣ�ʹ��ͬ����IOMUXC MUX��PAD���� */
  
    IOMUXC_SetPinMux(
      RGB_RED_LED_IOMUXC,    /* ����Ϊ��ͨIO */
      0U);                  /* ��ʹ��SION���� */
		
		/*�������Ź���*/
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
      RGB_GREEN_LED_IOMUXC,    /* ����Ϊ��ͨIO */
      0U);                 /* ��ʹ��SION���� */
		
		/*�������Ź���*/
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
      RGB_BLUE_LED_IOMUXC,    /* ����Ϊ��ͨIO */
      0U);                /* ��ʹ��SION���� */
		
		/*�������Ź���*/
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
      
      
     /** ���İ��LED�ƣ�GPIO���� **/       
    led_config.direction = kGPIO_DigitalOutput; //���ģʽ
    led_config.outputLogic =  1;                //Ĭ�ϸߵ�ƽ
    led_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
		
		/* ��ʼ�� LED GPIO. */
    GPIO_PinInit(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, &led_config);
    
    /* ʹ��ͬ����LED config����RGB LED�� */
    GPIO_PinInit(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, &led_config);
    GPIO_PinInit(RGB_GREEN_LED_GPIO, RGB_GREEN_LED_GPIO_PIN, &led_config);
    GPIO_PinInit(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, &led_config);
}

/*********************************************END OF FILE**********************/
