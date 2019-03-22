#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include "fsl_common.h"


#define pwm_Reference 5000          //  电机PWM基准值
#define Median 840               //舵机中值


void DJ_PWM_Reload(uint32_t Offset); 
void MOTOR_PWMs_Reload(uint32_t pwm_Difference)；
void PWMInit();

#endif /* __BSP_PWM_H */
