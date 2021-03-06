#include "bsp_EnCoder.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_qtmr.h"
#include "fsl_pit.h"

static int16_t tmp=0;

void ENCPinInit(void)
{
	//IO功能设置
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_QTIMER3_TIMER0,0);	//CNT
  IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,0);	//Dir
	//配置IO引脚GPIO_AD_B0_03和GPIO_SD_B1_03的功能
	//低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_QTIMER3_TIMER0,0x10B0);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,0x10B0);
}

void PITInit(void)
{
	pit_config_t pit_config;
	PIT_GetDefaultConfig(&pit_config);  //初始化为默认配置
  pit_config.enableRunInDebug=true;   //调试模式下PIT继续运行
  PIT_Init(PIT,&pit_config);          //初始化PIT定时器
	PIT_SetTimerPeriod(PIT,kPIT_Chnl_1,660000);//设置倒计时初始值
  PIT_EnableInterrupts(PIT,kPIT_Chnl_1,kPIT_TimerInterruptEnable);//使能中断
	//RT1052_NVIC_SetPriority(PIT_IRQn,6,0);	//抢占优先级6，子优先级0
	EnableIRQ(PIT_IRQn);
	PIT_StartTimer(PIT,kPIT_Chnl_1);
}

void ENCInit(void)
{
	ENCPinInit();
	qtmr_config_t qtmrConfig;
	QTMR_GetDefaultConfig(&qtmrConfig);   //得到默认的参数
	qtmrConfig.primarySource = kQTMR_ClockCounter0InputPin;    //Counter0InputPin作为主要的输入源，用于脉冲输入，J11引脚与编码器的A相相连
  qtmrConfig.secondarySource = kQTMR_Counter1InputPin;       //Counter1InputPin作为次要的输入源，用于方向输入，K11引脚与编码器的Dir相连
	QTMR_Init(TMR3, kQTMR_Channel_0, &qtmrConfig);             //初始化TMR3的通道0
	QTMR_StartTimer(TMR3, kQTMR_Channel_0, kQTMR_PriSrcRiseEdgeSecDir);   //开启TMR3通道0的计数器，计数模式使用带方向计数模式
	PITInit();
}

int16_t DisCNT(void)
{
	//int16_t tmp=(int16_t)TMR3->CHANNEL[kQTMR_Channel_0].CNTR;
	//TMR3->CHANNEL[kQTMR_Channel_0].CNTR = 0;
	return tmp;
}

void PIT_IRQHandler(void)
{
	 //PIT CH1中断
	if((PIT_GetStatusFlags(PIT,kPIT_Chnl_1)&kPIT_TimerFlag)==kPIT_TimerFlag)
	{
		tmp=(int16_t)TMR3->CHANNEL[kQTMR_Channel_0].CNTR;
		TMR3->CHANNEL[kQTMR_Channel_0].CNTR = 0;
		PIT_ClearStatusFlags(PIT,kPIT_Chnl_1,kPIT_TimerFlag);//清楚中断标志位
	}
	 __DSB();				//数据同步屏蔽指令
}
