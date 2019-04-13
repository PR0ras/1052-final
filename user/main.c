/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   用V2.3.1版本库建的工程模板
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 

#include "board.h"
#include "fsl_debug_console.h"
#include "./led/bsp_led.h"
#include "carmer.h"
#include "bsp_FlexPwm.h"
#include "bsp_lpuart.h"
/* RT-Thread 头文件 */
#include "rtthread.h"

/*
*************************************************************************
*                               宏定义&变量
*************************************************************************
*/

#define LED1_THREAD_PRIORITY            25
#define LED1_THREAD_STACK_SIZE          512
#define LED1_THREAD_TIMESLICE           5


extern int16_t s_volt[8]; //ADC数据，8个通道
extern int16_t speed_now;



int16_t aims_speed=50;


/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块指针 */
static  rt_thread_t led1_thread = RT_NULL,
                    run_thread = RT_NULL,
                    AD7606_thread = RT_NULL,
										led2_thread = RT_NULL;


/* 指向信号量的指针 */
static rt_sem_t sem = RT_NULL;




/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
// static void thread1_entry(void* parameter);
// static void thread2_entry(void* parameter);
static void camera_entry(void* parameter);
static void run_entry(void* parameter);
static void AD7606_entry(void* parameter);



/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 
	   * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	   * 即在component.c文件中的rtthread_startup()函数中完成了。
	   * 所以在main函数中，只需要创建线程和启动线程即可。
	   */

	
	led1_thread = rt_thread_create("led1",                     /* 线程名字，字符串形式 */
                                 led1_thread_entry,          /* 线程入口函数 */
                                 (void*)1,                    /* 线程入口函数参数 */
                                 LED1_THREAD_STACK_SIZE,     /* 线程栈大小，单位为字节 */
                                 LED1_THREAD_PRIORITY,       /* 线程优先级，数值越大，优先级越小 */                  
                                 LED1_THREAD_TIMESLICE);     /* 线程时间片 */
		   
  // if (led1_thread != RT_NULL)
  // {
	//   rt_thread_startup(led1_thread);
  // }
  // else
  //   return -1; 

	led2_thread = rt_thread_create("led2",                     /* 线程名字，字符串形式 */
                                 led1_thread_entry,          /* 线程入口函数 */
                                 (void*)2,                    /* 线程入口函数参数 */
                                 512,     /* 线程栈大小，单位为字节 */
                                 LED1_THREAD_PRIORITY,       /* 线程优先级，数值越大，优先级越小 */                  
                                 20);     /* 线程时间片 */	
	 if (led2_thread != RT_NULL)
     rt_thread_startup(led2_thread);
   else
     return -1;
	

  AD7606_thread = rt_thread_create("AD7606",                     /* 线程名字，字符串形式 */
                                 AD7606_entry,          /* 线程入口函数 */
                                 (void*)2,                    /* 线程入口函数参数 */
                                 512,     /* 线程栈大小，单位为字节 */
                                 LED1_THREAD_PRIORITY,       /* 线程优先级，数值越大，优先级越小 */                  
                                 2);     /* 线程时间片 */	
	 if (AD7606_thread != RT_NULL)
     rt_thread_startup(AD7606_thread);
   else
     return -1;

  	run_thread = rt_thread_create("run",                     /* 线程名字，字符串形式 */
                                 run_entry,          /* 线程入口函数 */
                                 (void*)2,                    /* 线程入口函数参数 */
                                 512,     /* 线程栈大小，单位为字节 */
                                 LED1_THREAD_PRIORITY,       /* 线程优先级，数值越大，优先级越小 */                  
                                 2);     /* 线程时间片 */	
	 if (run_thread != RT_NULL)
     rt_thread_startup(run_thread);
   else
    return -1;


  return 0;
}


/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
		rt_uint32_t count = 0;
    rt_uint32_t no = (rt_uint32_t) parameter; /* 获得线程的入口参数 */
    while (1)
    {
        CORE_BOARD_LED_ON;
        rt_thread_delay(500);   /* 延时，单位为tick */
      //PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,4000);
     // PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,2000+5000);
        CORE_BOARD_LED_OFF;        
        rt_thread_delay(500);   /* 延时，单位为tick */
    }
}


 static void thread1_entry(void* parameter)
 {
     rt_uint32_t count = 0;

     while (1)
     {
         /* 线程1采用低优先级运行，一直打印计数值 */
         PRINTF("thread count: %d\r\n", count++ );
 				//LCD_P6x8Str(100,0,"s0");
 				//LCD_P6x8Str(15,1,"s1");
 				//Dis_num(80,2,count++);
 				rt_thread_delay(2000);

     }
 }

 static void thread2_entry(void* parameter)
 {
     /* 线程2拥有较高的优先级，以抢占线程1而获得执行 */

     /* 线程2启动后先睡眠10个OS Tick */
     rt_thread_delay(200);

     /*
      * 线程2唤醒后直接删除线程1，删除线程1后，线程1自动脱离就绪线程
      * 队列
      */
     rt_thread_suspend(led1_thread);
   

     /*
      * 线程2继续休眠10个OS Tick然后退出，线程2休眠后应切换到idle线程
      * idle线程将执行真正的线程1控制块和线程栈的删除
      */
     rt_thread_delay(500);

     /*
      * 线程2运行结束后也将自动被删除(线程控制块和线程栈依然在idle线
      * 程中释放)
      */
 		 rt_thread_resume(led1_thread);
 		 rt_thread_delay(500);
 		  while (1)
 			{
 				;
 //				rt_thread_delay(200);
 //				rt_thread_suspend(led1_thread);
 //				rt_thread_delay(500);
 				rt_thread_resume(led1_thread);
 			}
 } 



static void camera_entry(void* parameter)
{

	CAM_DIS();
}


static void run_entry(void* parameter)
{
  while(1)
	{
    rt_thread_delay(20);
    run();
    //Speedupdate();
	}
}

static void AD7606_entry(void* parameter)
{
  while(1)
  { 

    // RGB_WHITE_LED_ON;
    // rt_thread_delay(500);
    //     RGB_LED_COLOR_RED;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_GREEN;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_BLUE;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_YELLOW;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_PURPLE;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_CYAN;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_WHITE;
    //     rt_thread_delay(500);
    //     RGB_LED_COLOR_OFF; 

    AD7606_Scan();
    AD7606_Mak();
    rt_thread_delay(500);
  } 

}
/****************************END OF FILE**********************/
