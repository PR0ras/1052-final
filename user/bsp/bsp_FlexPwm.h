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
// #define RED_BLACK                 //电机接线左红右黑，若为左黑右红注释该语句
void PWMPinInit(void);
void PWM_bsp_Init(PWM_Type *Pwmx,pwm_submodule_t Module_x,uint8_t subModulesToUpdate,uint32_t freq);
void PWMx_SMx_DutySet(PWM_Type *Pwmx,	pwm_submodule_t Module_x,uint8_t subModulesToUpdate,pwm_channels_t pwmSignal,uint16_t duty);
void PWMInit(void);
void MOTOR_PWMs_Reload(int32_t pwm_Difference);
void DJ_PWM_Reload(int32_t Offset);
void run(void);
void diriction(void);
float get_duoji_Kp(void);
#endif /* __BSP_PWM_H */
