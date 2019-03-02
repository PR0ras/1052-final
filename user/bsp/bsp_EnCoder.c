#include "bsp_EnCoder.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_qtmr.h"
#include "fsl_pit.h"

static int16_t tmp=0;

void ENCPinInit(void)
{
	//IO��������
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_QTIMER3_TIMER0,0);	//CNT
  IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,0);	//Dir
	//����IO����GPIO_AD_B0_03��GPIO_SD_B1_03�Ĺ���
	//��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_QTIMER3_TIMER0,0x10B0);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_QTIMER3_TIMER1,0x10B0);
}

void PITInit(void)
{
	pit_config_t pit_config;
	PIT_GetDefaultConfig(&pit_config);  //��ʼ��ΪĬ������
  pit_config.enableRunInDebug=true;   //����ģʽ��PIT��������
  PIT_Init(PIT,&pit_config);          //��ʼ��PIT��ʱ��
	PIT_SetTimerPeriod(PIT,kPIT_Chnl_1,660000);//���õ���ʱ��ʼֵ
  PIT_EnableInterrupts(PIT,kPIT_Chnl_1,kPIT_TimerInterruptEnable);//ʹ���ж�
	//RT1052_NVIC_SetPriority(PIT_IRQn,6,0);	//��ռ���ȼ�6�������ȼ�0
	EnableIRQ(PIT_IRQn);
	PIT_StartTimer(PIT,kPIT_Chnl_1);
}

void ENCInit(void)
{
	ENCPinInit();
	qtmr_config_t qtmrConfig;
	QTMR_GetDefaultConfig(&qtmrConfig);   //�õ�Ĭ�ϵĲ���
	qtmrConfig.primarySource = kQTMR_ClockCounter0InputPin;    //Counter0InputPin��Ϊ��Ҫ������Դ�������������룬J11�������������A������
  qtmrConfig.secondarySource = kQTMR_Counter1InputPin;       //Counter1InputPin��Ϊ��Ҫ������Դ�����ڷ������룬K11�������������Dir����
	QTMR_Init(TMR3, kQTMR_Channel_0, &qtmrConfig);             //��ʼ��TMR3��ͨ��0
	QTMR_StartTimer(TMR3, kQTMR_Channel_0, kQTMR_PriSrcRiseEdgeSecDir);   //����TMR3ͨ��0�ļ�����������ģʽʹ�ô��������ģʽ
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
	 //PIT CH1�ж�
	if((PIT_GetStatusFlags(PIT,kPIT_Chnl_1)&kPIT_TimerFlag)==kPIT_TimerFlag)
	{
		tmp=(int16_t)TMR3->CHANNEL[kQTMR_Channel_0].CNTR;
		TMR3->CHANNEL[kQTMR_Channel_0].CNTR = 0;
		PIT_ClearStatusFlags(PIT,kPIT_Chnl_1,kPIT_TimerFlag);//����жϱ�־λ
	}
	 __DSB();				//����ͬ������ָ��
}
