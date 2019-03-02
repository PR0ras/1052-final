#include "fsl_pit.h"
#include "bsp_pit.h"
#include "1052_NVIC.h"
pit_config_t pit_config;
uint32_t cnt = 0;

//初始化PIT通道0,PIT时钟源为perclk_clk_root=75MHz.
//ldval: CH0初始值,0~0xFFFFFFFF
//递减计数,当ldval==0时,产生中断.
//定时时间=ldval/PERCLK_CLK_ROOT
void PIT_CH0_Int_Init(uint32_t ldval)
{
    PIT_GetDefaultConfig(&pit_config);  //初始化为默认配置
    pit_config.enableRunInDebug = true; //调试模式下PIT继续运行
    PIT_Init(PIT, &pit_config);         //初始化PIT定时器

    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, ldval);                       //设置倒计时初始值
//    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, 75000000);
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable); //使能中断
//    PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable); //使能中断
//    //RT1052_NVIC_SetPriority(PIT_IRQn, 6, 0);                           //抢占优先级6，子优先级0
    
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag); //清楚中断标志位
	
    PIT_StartTimer(PIT, kPIT_Chnl_0); //打开PIT
		//RT1052_NVIC_SetPriority(PIT_IRQn,5,0);
		EnableIRQ(PIT_IRQn);              //使能PIT中断
    //PIT_StartTimer(PIT, kPIT_Chnl_1); //打开PIT
	PIT_StopTimer(PIT, kPIT_Chnl_0); //打开PIT
	//PIT_Deinit(PIT);
}

//PIT中断服务函数
void PIT_IRQHandler(void)
{
    //PIT CH0中断
    if ((PIT_GetStatusFlags(PIT, kPIT_Chnl_0) & kPIT_TimerFlag) == kPIT_TimerFlag)
    {
        cnt++;
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag); //清楚中断标志位
    }
   if ((PIT_GetStatusFlags(PIT, kPIT_Chnl_1) & kPIT_TimerFlag) == kPIT_TimerFlag)
    {
//        //cnt++;
//        FPS=FPStmp;
//        FPStmp=0;
        PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, kPIT_TimerFlag); //清楚中断标志位
    }
    __DSB(); //数据同步屏蔽指令
}
