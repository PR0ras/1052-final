#include "bsp_FlexPwm.h"
#include "fsl_pwm.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  

void PWMPinInit(void)
{
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02,0U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01,0x10B0u); 		
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01,0x10B0u); 
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02,0x10B0u); 
}

void PWM_bsp_Init(PWM_Type *Pwmx,pwm_submodule_t Module_x,uint8_t subModulesToUpdate,pwm_channels_t Channel1,uint32_t freq)
{
	uint16_t psc=7;  //Ԥ��Ƶ��,0~7,��ʾ2^psc��Ƶ.
	pwm_config_t pwm_init_structure;
	uint32_t sourceclock;
	pwm_signal_param_t pwm_ignal[2];
	pwm_clock_prescale_t pwm_prescale=(pwm_clock_prescale_t)psc; //��Ƶ
    

	//��ʼ��Pwmx ģ��N��ͨ��X
  PWM_GetDefaultConfig(&pwm_init_structure);              //�ȳ�ʼ��ΪĬ������
  pwm_init_structure.clockSource=kPWM_BusClock;           //ʱ��ԴΪIP BUS=IPG_CLK_ROOT=150MHz
  pwm_init_structure.prescale=pwm_prescale;               //���÷�Ƶ
  pwm_init_structure.reloadLogic=kPWM_ReloadPwmFullCycle; //ȫ���ڸ���
  pwm_init_structure.pairOperation=kPWM_Independent;      //PMWA PWMB����ģʽ
	PWM_Init(Pwmx,Module_x,&pwm_init_structure);       //��ʼ��Pwmxģ��3

	//���ι��ϼ�⹦��
  //Pwmx->SM[0].DISMAP[0]=0; 
	Pwmx->SM[1].DISMAP[0]=0;
	Pwmx->SM[2].DISMAP[0]=0;
    
	//����Pwmxͨ��
	sourceclock=CLOCK_GetFreq(kCLOCK_IpgClk);
    
  pwm_ignal[0].pwmChannel=Channel1;                     //PWMAͨ��
  pwm_ignal[0].level=kPWM_HighTrue;                      //�ߵ�ƽ��Ч
  pwm_ignal[0].dutyCyclePercent=80;                    //ռ�ձ�
//	pwm_ignal[1].pwmChannel=kPWM_PwmB;                     //PWMͨ��
//	pwm_ignal[1].level=kPWM_HighTrue;                      //�ߵ�ƽ��Ч
//  pwm_ignal[1].dutyCyclePercent=50;                    //ռ�ձ�
	//����Pwmx���������ģʽ
  PWM_SetupPwm(Pwmx,Module_x,pwm_ignal,1,kPWM_CenterAligned,freq,sourceclock);

  PWM_SetPwmLdok(Pwmx,subModulesToUpdate,true);    //����PWM��load okλ
  PWM_StartTimer(Pwmx,subModulesToUpdate);         //������ʱ��
}

void PWMInit()
{
	PWMPinInit();
	PWM_bsp_Init(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,10000);
	PWM_bsp_Init(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmB,10000);
	PWM_bsp_Init(PWM2,kPWM_Module_2,kPWM_Control_Module_2,kPWM_PwmA,50);
}

