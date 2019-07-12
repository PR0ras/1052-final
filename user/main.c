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
#include "oled2.h"
#include "bwLabel.h"
#include "bsp_lpuart.h"
#include "bsp_Timer.h"
#include "LQ_MT9V034M.h"
#include "bsp_key.h"
/* RT-Thread 头文件 */
#include "rtthread.h"

/*
*************************************************************************
*                               宏定义&变量
*************************************************************************
*/

#define LED1_THREAD_PRIORITY 25
#define LED1_THREAD_STACK_SIZE 512
#define LED1_THREAD_TIMESLICE 5

extern int16_t s_volt[8]; //ADC数据，8个通道
extern int16_t speed_now;
extern int16_t jiaodu_err;
extern float duoji_Kp;
int16_t aims_speed = 50;

/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块指针 */
static rt_thread_t led1_thread = RT_NULL,
                   run_thread = RT_NULL,
                   AD7606_thread = RT_NULL,
                   led2_thread = RT_NULL,
                   camera_thread = RT_NULL,
                   cmd_thread = RT_NULL,
                   key_thread = RT_NULL,
                   Display_thread = RT_NULL,
                   Motor_thread = RT_NULL;

/* 指向信号量的指针 */
static rt_sem_t sem = RT_NULL;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void led1_thread_entry(void *parameter);
// static void thread1_entry(void* parameter);
// static void thread2_entry(void* parameter);
static void camera_entry(void *parameter);
static void run_entry(void *parameter);
static void AD7606_entry(void *parameter);
static void cmd_entry(void *parameter);
static void key_entry(void *parameter);
static void Display_entry(void *parameter);
static void Motor_entry(void *parameter);
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

  camera_thread = rt_thread_create("camera",             /* 线程名字，字符串形式 */
                                   camera_entry,         /* 线程入口函数 */
                                   (void *)2,            /* 线程入口函数参数 */
                                   4096 * 3,             /* 线程栈大小，单位为字节 */
                                   LED1_THREAD_PRIORITY, /* 线程优先级，数值越大，优先级越小 */
                                   8);                  /* 线程时间片 */

  led1_thread = rt_thread_create("led1",                 /* 线程名字，字符串形式 */
                                 led1_thread_entry,      /* 线程入口函数 */
                                 (void *)1,              /* 线程入口函数参数 */
                                 LED1_THREAD_STACK_SIZE, /* 线程栈大小，单位为字节 */
                                 LED1_THREAD_PRIORITY,   /* 线程优先级，数值越大，优先级越小 */
                                 LED1_THREAD_TIMESLICE); /* 线程时间片 */

  AD7606_thread = rt_thread_create("AD7606",             /* 线程名字，字符串形式 */
                                   AD7606_entry,         /* 线程入口函数 */
                                   (void *)2,            /* 线程入口函数参数 */
                                   512,                  /* 线程栈大小，单位为字节 */
                                   LED1_THREAD_PRIORITY, /* 线程优先级，数值越大，优先级越小 */
                                   2);                   /* 线程时间片 */

  run_thread = rt_thread_create("run",                /* 线程名字，字符串形式 */
                                run_entry,            /* 线程入口函数 */
                                (void *)2,            /* 线程入口函数参数 */
                                512,                  /* 线程栈大小，单位为字节 */
                                LED1_THREAD_PRIORITY, /* 线程优先级，数值越大，优先级越小 */
                                3);                   /* 线程时间片 */

  cmd_thread = rt_thread_create("led2",               /* 线程名字，字符串形式 */
                                cmd_entry,            /* 线程入口函数 */
                                (void *)3,            /* 线程入口函数参数 */
                                512,                  /* 线程栈大小，单位为字节 */
                                LED1_THREAD_PRIORITY, /* 线程优先级，数值越大，优先级越小 */
                                2);                   /* 线程时间片 */

  key_thread = rt_thread_create("led2",                   /* 线程名字，字符串形式 */
                                key_entry,                /* 线程入口函数 */
                                (void *)3,                /* 线程入口函数参数 */
                                512,                      /* 线程栈大小，单位为字节 */
                                LED1_THREAD_PRIORITY,     /* 线程优先级，数值越大，优先级越小 */
                                2);                       /* 线程时间片 */

  Display_thread = rt_thread_create("OLED",               /* 线程名字，字符串形式 */
                                    Display_entry,        /* 线程入口函数 */
                                    (void *)3,            /* 线程入口函数参数 */
                                    512,                  /* 线程栈大小，单位为字节 */
                                    LED1_THREAD_PRIORITY, /* 线程优先级，数值越大，优先级越小 */
                                    2);                   /* 线程时间片 */

  Motor_thread = rt_thread_create("Motor",                /* 线程名字，字符串形式 */
                                  Motor_entry,            /* 线程入口函数 */
                                  (void *)3,              /* 线程入口函数参数 */
                                  512,                    /* 线程栈大小，单位为字节 */
                                  LED1_THREAD_PRIORITY,   /* 线程优先级，数值越大，优先级越小 */
                                  2);                     /* 线程时间片 */

  //摄像头
  if (camera_thread != RT_NULL)
  {
    rt_thread_startup(camera_thread);
  }
  else
    return -1;
  //LED灯
  if (led1_thread != RT_NULL)
  {
    rt_thread_startup(led1_thread);
  }
  else
    return -1;
#if defined(USE_LCD) && (USE_LCD == 0U)
  if (run_thread != RT_NULL)
    rt_thread_startup(run_thread);
  else
    return -1;
  if (Motor_thread != RT_NULL)
    rt_thread_startup(Motor_thread);
  else
    return -1;
#endif

  // if (AD7606_thread != RT_NULL)
  //   rt_thread_startup(AD7606_thread);
  // else
  //   return -1;
  //摄像头参数
  if (cmd_thread != RT_NULL)
    rt_thread_startup(cmd_thread);
  else
    return -1;
  //按键
  if (key_thread != RT_NULL)
    rt_thread_startup(key_thread);
  else
    return -1;

  //Oled显示
  if (Display_thread != RT_NULL)
    rt_thread_startup(Display_thread);
  else
    return -1;

  return 0;
}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void led1_thread_entry(void *parameter)
{
  rt_uint32_t count = 0;
  rt_uint32_t no = (rt_uint32_t)parameter; /* 获得线程的入口参数 */
  while (1)
  {
    CORE_BOARD_LED_ON;
    rt_thread_delay(500); /* 延时，单位为tick */
                          //PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,4000);
                          // PWMx_SMx_DutySet(PWM2,kPWM_Module_1,kPWM_Control_Module_1,kPWM_PwmA,2000+5000);
    CORE_BOARD_LED_OFF;
    rt_thread_delay(500); /* 延时，单位为tick */
  }
}

static void thread1_entry(void *parameter)
{
  rt_uint32_t count = 0;

  while (1)
  {
    /* 线程1采用低优先级运行，一直打印计数值 */
    PRINTF("thread count: %d\r\n", count++);
    //LCD_P6x8Str(100,0,"s0");
    //LCD_P6x8Str(15,1,"s1");
    //Dis_num(80,2,count++);
    rt_thread_delay(2000);
  }
}

static void thread2_entry(void *parameter)
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

static void camera_entry(void *parameter)
{
  CAM_DIS();
}

static void run_entry(void *parameter)
{
  while (1)
  {
    // DJ_PWM_Reload(Median);
    diriction();
    rt_thread_delay(5);
    // Speedupdate();
  }
}

static void AD7606_entry(void *parameter)
{
  while (1)
  {
    //    AD7606_Scan();
    //    AD7606_Mak();
    rt_thread_delay(500);
  }
}

static void cmd_entry(void *parameter)
{
  while (1)
  {
    if (rxflag)
    {
      //PRINTF("camaddress: %x\r\n", camaddress);
      //PRINTF("camdata: %x\r\n", camdata);
      rt_enter_critical();
      imgremote(camaddress, camdata);
      rt_exit_critical();
      rxflag = 0;
    }
    rt_thread_delay(500);
  }
}

static void key_entry(void *parameter)
{
  uint8_t contis = 0;
  while (1)
  {
    /* code */  
    if (keypres == WKUP_PRES)
    {
      testSend();
      keypres = 0;
    }
    if (keypres == KEY0_PRES)
    {
      contis = ~contis;
      keypres = 0;
    }
    if (contis)
    {
      testSend();
    }
    rt_thread_delay(100);
  }
}

static void Display_entry(void *parameter)
{
  while (1)
  {
    // rt_enter_critical();
    // Dis_numint(1, 0, FPS);
    sprintf(OLEDdispBuff1, "RT=%d LC=%d RC=%d", tmp11,temp_img1,temp_img2);
    LCD_P6x8Str(0,0,OLEDdispBuff1);
    // Dis_numint(55, 0, tmp11);
    // Dis_numint(15, 2, temp_img1);
    // Dis_numint(65, 2, temp_img2);
    Dis_numint(65, 1, DisCNT());
    Dis_numint(15, 1, jiaodu_err);
    Dis_numint(15, 3, mid_NUM1);
    Dis_numint(15, 5, mid_NUM2);
    Dis_numint(15, 7, mid_NUM3);
    Dis_numint(80, 5, duoji_Kp);
    Dis_numint(80, 7, mid_NUM);
    // Dis_numint(65, 7, HD_first_flag);
    // rt_exit_critical();
    rt_thread_delay(50);
  }
}

static void Motor_entry(void *parameter)
{
  run();
  rt_thread_delay(5);
}
/****************************END OF FILE**********************/
