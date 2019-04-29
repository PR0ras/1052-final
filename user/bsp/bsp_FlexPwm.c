#include "bsp_FlexPwm.h"

//电机控制参数
  float Kp=86;//60;
  float Ki=24.6;//2.65;//0.25;
  float Kd=0.64;//0.6;
  float in=0;
  int16_t err=0;
  int16_t l_err=0;
  int16_t ll_err=0;
  int16_t actual_speed=0; 
  int16_t now_PWM=0;
  int32_t Integral_err=0;
	extern int16_t aims_speed;
  extern int16_t speed_now;

//舵机控制参数
  uint16_t jiaodu=Median;
  float duoji_Kp=0,direct_Kd=0.98;
  uint16_t duoji_err=0,duoji_err_last=0;
  uint16_t jiaodu_err=0;
//                                    长直道  短直道    斜入直到  十字     障碍     小S湾    中S弯   大S弯    小弯     中弯    大弯   特大弯    大转向     环路
/*转向Kp*/   int direct_kp_array[]={    0   ,   0    ,    3    ,  6    ,  7     ,   8   ,   12   ,  14  ,  14    ,  15   ,  18  ,  18   ,    20 ,      80  };

/*转向Kp基准*/ float kp_val=5;//所有的p都会加上这个数
void PWMPinInit(void)
{
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02,0U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_FLEXPWM2_PWMA01,0x10B0u); 		
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_FLEXPWM2_PWMB01,0x10B0u); 
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_FLEXPWM2_PWMA02,0x10B0u); 
}

void PWM_bsp_Init(PWM_Type *Pwmx,pwm_submodule_t Module_x,uint8_t subModulesToUpdate,uint32_t freq)
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
  Pwmx->SM[0].DISMAP[0]=0; 
	Pwmx->SM[1].DISMAP[0]=0;
	Pwmx->SM[2].DISMAP[0]=0;
    
    //设置Pwmx通道
  sourceclock=CLOCK_GetFreq(kCLOCK_IpgClk);
    
  pwm_ignal[0].pwmChannel=kPWM_PwmA;                     //PWMA通道
  pwm_ignal[0].level=kPWM_HighTrue;                      //高电平有效
//    pwm_ignal[0].dutyCyclePercent=50;                    //占空比
  pwm_ignal[1].pwmChannel=kPWM_PwmB;                     //PWM通道
  pwm_ignal[1].level=kPWM_HighTrue;                      //高电平有效
//    pwm_ignal[1].dutyCyclePercent=50;                    //占空比
	//设置Pwmx，中央对齐模式
  PWM_SetupPwm(Pwmx,Module_x,pwm_ignal,2,kPWM_EdgeAligned,freq,sourceclock);

  PWM_SetPwmLdok(Pwmx,subModulesToUpdate,true);    //设置PWM的load ok位
  PWM_StartTimer(Pwmx,subModulesToUpdate);         //开启定时器
}

void PWMInit()
{
	PWMPinInit();
	PWM_bsp_Init(PWM2,kPWM_Module_1,kPWM_Control_Module_1,400);
	PWM_bsp_Init(PWM2,kPWM_Module_2,kPWM_Control_Module_2,300);
  //电机1占空比为固定值
  #ifdef RED_BLACK
  PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,pwm_Reference);
  #else
  PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmB,pwm_Reference);
  #endif
}

void PWMx_SMx_DutySet(PWM_Type *Pwmx,	pwm_submodule_t Module_x,uint8_t subModulesToUpdate,pwm_channels_t pwmSignal,uint16_t duty) 
{
    PWM_UpdatePwmDutycycle(Pwmx,Module_x,pwmSignal,kPWM_EdgeAligned,duty); //更新PWM占空比
    PWM_SetPwmLdok(Pwmx,subModulesToUpdate,true);    //设置PWM的load ok位
}

void MOTOR_PWMs_Reload(int32_t pwm_Difference)
{ //更新电机2占空比
  #ifdef RED_BLACK
  PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmB,pwm_Reference+pwm_Difference);
  #else
  PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,pwm_Reference+pwm_Difference);
  #endif
}

void DJ_PWM_Reload(int32_t Offset)  //Median中值  Offset偏移量
{
    //更新舵机占空比
   PWMx_SMx_DutySet(PWM2,kPWM_Module_2,kPWM_Control_Module_2,kPWM_PwmA,Offset);
}


void run()
{
	actual_speed=speed_now;
  err=aims_speed-actual_speed;
 
//积分分离
 if(abs(err)>50)
  in=2.0;
 else if(abs(err)>30)
    in=1.0;
 else if(abs(err)>5) 
    in=0.5;
 else 
    in=0.0;

  now_PWM+=Kp*(err-l_err)+in*Ki*err+Kd*(err-2*l_err+ll_err);//增量式
  // Integral_err+=err;//位置式
  // now_PWM=Kp*err+in*Ki*Integral_err+Kd*(err-l_err);//位置式
  l_err=err;
  ll_err=l_err;//增量式

  //限幅
  if(now_PWM>pwm_max) now_PWM=pwm_max;
  if(now_PWM<-pwm_max) now_PWM=-pwm_max; 

  //停车
  if(actual_speed==0&&err==0)
    now_PWM=0;

  MOTOR_PWMs_Reload(now_PWM);
  // PRINTF("err:%d      ,speed_now:%d       ,now_PWM:%d\r\n", err,speed_now,now_PWM );
}


void diriction()
{

  duoji_Kp=get_duoji_Kp();//获取动态的KP
  duoji_err=-jiaodu_err;//误差赋值
  jiaodu=(int)(duoji_Kp*duoji_err+direct_Kd*(duoji_err-duoji_err_last));//舵机PD公式
  duoji_err_last=duoji_err;//纪录误差



//舵机限幅
  if(jiaodu>lim_lift)
    jiaodu=lim_lift;
  if(jiaodu<lim_right)
    jiaodu=lim_right;
//舵机输出
  DJ_PWM_Reload(jiaodu);
}


float get_duoji_Kp()
{
    float kp=0;
  
  //**注意**下边的一堆判断是通过一堆是根据不同的道路类型获取一个基础的kp，
  //这个KP是保证不抖的基本参数，下边有介绍，速度慢的时候可以固定一个常数，之前一个固定参数到2.5米    
  if(zhidao_count_flag&&guai_dian_count==0)
  {
    kp = direct_kp_array[Short_zhidao];
  // gl_path_type=Short_zhidao;
  }  
  else if(valid_line>52&&guai_dian_count==0)//在直到上
  {
    kp=direct_kp_array[lean_zhidao];
  // gl_path_type=lean_zhidao;
  }
  else if(LoopFlag)//环路
  {
    kp=direct_kp_array[Loop_road];
  }
  else if(Shi_zi_flag)
  {
    kp=direct_kp_array[Shi_zi];
      // gl_path_type=Shi_zi;
  }
  else if(valid_line>=48&&guai_dian_count)
  {
      if(gl_zhidao_count>40)//小S弯
      { 
    kp=direct_kp_array[Xiao_S];
    // gl_path_type=Xiao_S;
      }
      if(gl_zhidao_count>25)//中S弯
      {
        kp=direct_kp_array[Zhong_S];
          // gl_path_type=Zhong_S;
      }  
      else//大S弯
      {
        kp=direct_kp_array[Da_S];
        // gl_path_type=Da_S;
      }       
  }
  else//大弯
  {
      if(valid_line>45)//45
      {              
        kp=direct_kp_array[Xiao_wan];
          // gl_path_type=Xiao_wan;
      }
      else if(valid_line>37)//37
      {
    kp=direct_kp_array[Zhong_wan];
          // gl_path_type=Zhong_wan;
      }
      else if(valid_line>30)//30
      {
          kp=direct_kp_array[Da_wan];
          // gl_path_type=Da_wan;
      }
      else
      {
          kp=direct_kp_array[T_Da_wan];
          // gl_path_type=T_Da_wan;
      }
  }
  return kp+kp_val+(IMG_H-valid_line)*(IMG_H-valid_line)/22;//22 33 35 
  //参数说明
  //IMG_H图像最大值，valid_line中线列数
}






