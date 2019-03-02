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
	uint16_t psc=7;  //预分频器,0~7,表示2^psc分频.
	pwm_config_t pwm_init_structure;
	uint32_t sourceclock;
	pwm_signal_param_t pwm_ignal[2];
	pwm_clock_prescale_t pwm_prescale=(pwm_clock_prescale_t)psc; //分频
    

	//初始化Pwmx 模块N的通道X
  PWM_GetDefaultConfig(&pwm_init_structure);              //先初始化为默认配置
  pwm_init_structure.clockSource=kPWM_BusClock;           //时钟源为IP BUS=IPG_CLK_ROOT=150MHz
  pwm_init_structure.prescale=pwm_prescale;               //设置分频
  pwm_init_structure.reloadLogic=kPWM_ReloadPwmFullCycle; //全周期更新
  pwm_init_structure.pairOperation=kPWM_Independent;      //PMWA PWMB独立模式
	PWM_Init(Pwmx,Module_x,&pwm_init_structure);       //初始化Pwmx模块3

	//屏蔽故障检测功能
  //Pwmx->SM[0].DISMAP[0]=0; 
	Pwmx->SM[1].DISMAP[0]=0;
	Pwmx->SM[2].DISMAP[0]=0;
    
	//设置Pwmx通道
	sourceclock=CLOCK_GetFreq(kCLOCK_IpgClk);
    
  pwm_ignal[0].pwmChannel=Channel1;                     //PWMA通道
  pwm_ignal[0].level=kPWM_HighTrue;                      //高电平有效
  pwm_ignal[0].dutyCyclePercent=80;                    //占空比
//	pwm_ignal[1].pwmChannel=kPWM_PwmB;                     //PWM通道
//	pwm_ignal[1].level=kPWM_HighTrue;                      //高电平有效
//  pwm_ignal[1].dutyCyclePercent=50;                    //占空比
	//设置Pwmx，中央对齐模式
  PWM_SetupPwm(Pwmx,Module_x,pwm_ignal,1,kPWM_CenterAligned,freq,sourceclock);

  PWM_SetPwmLdok(Pwmx,subModulesToUpdate,true);    //设置PWM的load ok位
  PWM_StartTimer(Pwmx,subModulesToUpdate);         //开启定时器
}

void PWMInit()
{
	PWMPinInit();
	PWM_bsp_Init(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,10000);
	PWM_bsp_Init(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmB,10000);
	PWM_bsp_Init(PWM2,kPWM_Module_2,kPWM_Control_Module_2,kPWM_PwmA,50);
}

