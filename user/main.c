/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��V2.3.1�汾�⽨�Ĺ���ģ��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 

#include "board.h"
#include "fsl_debug_console.h"
#include "./led/bsp_led.h"
#include "carmer.h"

/* RT-Thread ͷ�ļ� */
#include "rtthread.h"

/*
*************************************************************************
*                               �궨��&����
*************************************************************************
*/

#define LED1_THREAD_PRIORITY            25
#define LED1_THREAD_STACK_SIZE          512
#define LED1_THREAD_TIMESLICE           5

/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ�ָ�� */
static  rt_thread_t led1_thread = RT_NULL,
										led2_thread = RT_NULL;

/* ָ���ź�����ָ�� */
static rt_sem_t sem = RT_NULL;

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void led1_thread_entry(void* parameter);
static void thread1_entry(void* parameter);
static void thread2_entry(void* parameter);
static void camera_entry(void* parameter);
/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	   * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	   * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	   * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	   */

	
	led1_thread = rt_thread_create("led1",                     /* �߳����֣��ַ�����ʽ */
                                 led1_thread_entry,          /* �߳���ں��� */
                                 (void*)1,                    /* �߳���ں������� */
                                 LED1_THREAD_STACK_SIZE,     /* �߳�ջ��С����λΪ�ֽ� */
                                 LED1_THREAD_PRIORITY,       /* �߳����ȼ�����ֵԽ�����ȼ�ԽС */                  
                                 LED1_THREAD_TIMESLICE);     /* �߳�ʱ��Ƭ */
		   
  if (led1_thread != RT_NULL)
  {
	  rt_thread_startup(led1_thread);
  }
  else
    return -1;         
	led2_thread = rt_thread_create("led2",                     /* �߳����֣��ַ�����ʽ */
                                 camera_entry,          /* �߳���ں��� */
                                 (void*)2,                    /* �߳���ں������� */
                                 4096,     /* �߳�ջ��С����λΪ�ֽ� */
                                 LED1_THREAD_PRIORITY,       /* �߳����ȼ�����ֵԽ�����ȼ�ԽС */                  
                                 20);     /* �߳�ʱ��Ƭ */	
	if (led2_thread != RT_NULL)
    rt_thread_startup(led2_thread);
  else
    return -1;
	return 0;
}


/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
		rt_uint32_t count = 0;
    rt_uint32_t no = (rt_uint32_t) parameter; /* ����̵߳���ڲ��� */
    while (1)
    {
        CORE_BOARD_LED_ON;
        rt_thread_delay(500);   /* ��ʱ����λΪtick */
        
        CORE_BOARD_LED_OFF;     
        rt_thread_delay(500);   /* ��ʱ����λΪtick */
      
//        PRINTF("*****��ӭʹ�� Ұ��i.MX RT1052 ������*****\r\n"); 
//		PRINTF("thread%d count: %d\r\n", no, count ++);
		//Dis_num(80,no,count);
		//rt_thread_delay(1000);
    }
}

static void thread1_entry(void* parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
        /* �߳�1���õ����ȼ����У�һֱ��ӡ����ֵ */
        PRINTF("thread count: %d\r\n", count++ );
				//LCD_P6x8Str(100,0,"s0");
				//LCD_P6x8Str(15,1,"s1");
				//Dis_num(80,2,count++);
				rt_thread_delay(2000);

    }
}

static void thread2_entry(void* parameter)
{
    /* �߳�2ӵ�нϸߵ����ȼ�������ռ�߳�1�����ִ�� */

    /* �߳�2��������˯��10��OS Tick */
    rt_thread_delay(200);

    /*
     * �߳�2���Ѻ�ֱ��ɾ���߳�1��ɾ���߳�1���߳�1�Զ���������߳�
     * ����
     */
    rt_thread_suspend(led1_thread);
   

    /*
     * �߳�2��������10��OS TickȻ���˳����߳�2���ߺ�Ӧ�л���idle�߳�
     * idle�߳̽�ִ���������߳�1���ƿ���߳�ջ��ɾ��
     */
    rt_thread_delay(500);

    /*
     * �߳�2���н�����Ҳ���Զ���ɾ��(�߳̿��ƿ���߳�ջ��Ȼ��idle��
     * �����ͷ�)
     */
		 rt_thread_resume(led1_thread);
		 rt_thread_delay(500);
		  while (1)
			{
				;
//				rt_thread_delay(200);
//				rt_thread_suspend(led1_thread);
//				rt_thread_delay(500);
//				rt_thread_resume(led1_thread);
			}
}

static void camera_entry(void* parameter)
{
	CAM_DIS();
}
/****************************END OF FILE**********************/
