#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_pit.h"
#include "bsp_Timer.h"

#include "fsl_pwm.h"


#include "fsl_debug_console.h"
#define pwm_Reference 5000        //  电机PWM基准值
#define pwm_max       5000        //电机限幅
#define Median 5020               //舵机中值
#define lim_right  4050           //舵机右极限
#define lim_lift   5990           //舵机左极限
#define RED_BLACK                 //电机接线左红右黑，若为左黑右红注释该语句
void PWMPinInit(void);
void PWM_bsp_Init(PWM_Type *Pwmx,pwm_submodule_t Module_x,uint8_t subModulesToUpdate,uint32_t freq);
void PWMx_SMx_DutySet(PWM_Type *Pwmx,	pwm_submodule_t Module_x,uint8_t subModulesToUpdate,pwm_channels_t pwmSignal,uint16_t duty);
void PWMInit();
void MOTOR_PWMs_Reload(int32_t pwm_Difference);
void DJ_PWM_Reload(int32_t Offset);
void run();
void diriction();
float get_duoji_Kp();
#endif /* __BSP_PWM_H */
