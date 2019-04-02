#include "bsp_ad7606_spi.h"

#define CH_NUM 8   //需要检测的通道数量

static int16_t s_adc_now[8];
static int16_t s_dat[8];
int16_t s_volt[8];
/*
*********************************************************************************************************
*	函 数 名: AD7606_Mak
*	功能说明: 处理采样后的数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_Mak(void)
{
	uint8_t i;
	int16_t adc;

	for (i = 0;i < CH_NUM; i++)
	{	
		s_dat[i] = AD7606_ReadAdc(i);
	/* 
		32767 = 5V , 这是理论值，实际可以根据5V基准的实际值进行公式矫正 
		volt[i] = ((int16_t)dat[i] * 5000) / 32767;	计算实际电压值（近似估算的），如需准确，请进行校准            
		volt[i] = dat[i] * 0.3051850947599719
	*/
		
		adc = s_dat[i];
		s_volt[i] = (adc * 5000) / 32767;
		
	}
}
/*
*********************************************************************************************************
*	函 数 名: GetAdcFormFifo
*	功能说明: 从FIFO中读取一个ADC值
*	形    参：_usReadAdc : 存放ADC结果的变量指针
*	返 回 值: 1 表示OK，0表示暂无数据
*********************************************************************************************************
*/
int16_t AD7606_ReadAdc(uint8_t _ch)
{
	int16_t sAdc;
	
	rt_hw_interrupt_disable();	//关中断
	sAdc = s_adc_now[_ch];
	rt_hw_interrupt_enable();  //开中断

	return sAdc;
}







void AD7606_Scan(void) 		/* 此函数代码按照时序编写 */
{
	uint8_t i;			

	/* BUSY = 0 时.ad7606处于空闲状态ad转换结束 */	
	if (BUSY_IS_LOW())	   
    {
		AD_CS_value(0); /* SPI片选 = 0 */
	
		for (i = 0; i < CH_NUM; i++)
		{
			s_adc_now[i] = bsp_spiRead1();
			
			s_adc_now[i] = s_adc_now[i] * 256 + bsp_spiRead1(); /* 读数据 */	
		}
	
		AD_CS_value(1); /* SPI片选 = 1 */

		AD7606_StartConv();	/* 给开始信号 */	
	}
}	 
/*
*********************************************************************************************************
*	函 数 名: bsp_spiRead1
*	功能说明: 从SPI总线接收8个bit数据。  SCK上升沿采集数据, SCK空闲时为高电平
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_spiRead1(void)
{

	uint8_t i;
	uint8_t read = 0;

	for (i = 0; i < 8; i++)
	{
		SCK_value(0);
		bsp_spiDelay();    //延时函数，需更改
		read = read << 1;
		if (MISO_IS_HIGH())
		{
			read++;
		}
		SCK_value(1);
		bsp_spiDelay();
	}
	return read;
}

void bsp_spiDelay(void)   //延时函数，需更改
{
	uint32_t i;

	/*
		延迟5时， F407 (168MHz主频） GPIO模拟，实测 SCK 周期 = 480ns (大约2M)
	*/
	for (i = 0; i < 5; i++);
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_StartConv
*	功能说明: 启动AD7606的ADC转换
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_StartConv(void)
{
	/* 上升沿开始转换，低电平持续时间至少25ns  */
	AD_CONVST_value(0);
	AD_CONVST_value(0);
	AD_CONVST_value(0);	/* 连续执行2次，低电平约50ns */
	
	AD_CONVST_value(1);
}





/*
*********************************************************************************************************
*	函 数 名: bsp_InitAD7606
*	功能说明: 初始化AD7606 SPI口线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spi_InitAD7606(void)
{
	AD7606_ConfigGPIO();		/* 配置GPIO */
	
	AD7606_Reset();				/* 硬件复位复AD7606 */
	
	AD_CONVST_value(1);			/* CONVST脚设置为高电平 */	
}
/*
*********************************************************************************************************
*	函 数 名: AD7606_RESET
*	功能说明: 硬件复位
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void AD7606_Reset(void)
{
	/* CS  SCLK拉高 */
		AD_CS_value(1);

 	/* AD7606是高电平复位，要求最小脉宽50ns */
	AD_RESET_value(0);
	
	AD_RESET_value(1);
    AD_RESET_value(1);
    AD_RESET_value(1);
    AD_RESET_value(1);
	
	AD_RESET_value(0);
}


/*
*********************************************************************************************************
*	函 数 名: AD7606_ConfigGPIO
*	功能说明: 配置GPIO。 
*********************************************************************************************************
*/
static void AD7606_ConfigGPIO(void)
{
     /* GPIO configuration of DoutA on GPIO_AD_B1_03 (pin M12) */
  gpio_pin_config_t DoutA_config = {
      .direction = kGPIO_DigitalInput,
      .outputLogic = 0U,
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_AD_B1_03 (pin M12) */
  GPIO_PinInit(GPIO1, 19U, &DoutA_config);



  /* GPIO configuration of CONVST on GPIO_B0_11 (pin A10) */
  gpio_pin_config_t CONVST_config = {
      .direction = kGPIO_DigitalOutput,
      .outputLogic = 0U,
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_B0_11 (pin A10) */
  GPIO_PinInit(GPIO2, 11U, &CONVST_config);



  /* GPIO configuration of RESET on GPIO_B0_13 (pin D10) */
  gpio_pin_config_t RESET_config = {
      .direction = kGPIO_DigitalOutput,
      .outputLogic = 0U,
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_B0_13 (pin D10) */
  GPIO_PinInit(GPIO2, 13U, &RESET_config);



  /* GPIO configuration of SCLK on GPIO_B0_15 (pin E11) */
  gpio_pin_config_t SCLK_config = {
      .direction = kGPIO_DigitalOutput,
      .outputLogic = 0U,
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_B0_15 (pin E11) */
  GPIO_PinInit(GPIO2, 15U, &SCLK_config);



  /* GPIO configuration of CS on GPIO_B1_01 (pin B11) */
  gpio_pin_config_t CS_config = {
      .direction = kGPIO_DigitalOutput,
      .outputLogic = 0U,
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_B1_01 (pin B11) */
  GPIO_PinInit(GPIO2, 17U, &CS_config);



  /* GPIO configuration of BUSY on GPIO_B1_03 (pin D11) */
  gpio_pin_config_t BUSY_config = {
      .direction = kGPIO_DigitalInput,
      .outputLogic = 0U, 
      .interruptMode = kGPIO_NoIntmode
  };
  /* Initialize GPIO functionality on GPIO_B1_03 (pin D11) */
  GPIO_PinInit(GPIO2, 19U, &BUSY_config);


  IOMUXC_SetPinMux(
      IOMUXC_GPIO_AD_B1_03_GPIO1_IO19,        /* GPIO_AD_B1_03 is configured as GPIO1_IO19 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B0_11_GPIO2_IO11,           /* GPIO_B0_11 is configured as GPIO2_IO11 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B0_13_GPIO2_IO13,           /* GPIO_B0_13 is configured as GPIO2_IO13 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B0_15_GPIO2_IO15,           /* GPIO_B0_15 is configured as GPIO2_IO15 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B1_01_GPIO2_IO17,           /* GPIO_B1_01 is configured as GPIO2_IO17 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinMux(
      IOMUXC_GPIO_B1_03_GPIO2_IO19,           /* GPIO_B1_03 is configured as GPIO2_IO19 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_AD_B1_03_GPIO1_IO19,        /* GPIO_AD_B1_03 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B0_11_GPIO2_IO11,           /* GPIO_B0_11 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B0_13_GPIO2_IO13,           /* GPIO_B0_13 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B0_15_GPIO2_IO15,           /* GPIO_B0_15 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B1_01_GPIO2_IO17,           /* GPIO_B1_01 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */
  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B1_03_GPIO2_IO19,           /* GPIO_B1_03 PAD functional properties : */
      0x10F1U);                               /* Slew Rate Field: Fast Slew Rate
                                                 Drive Strength Field: R0/6
                                                 Speed Field: max(200MHz)
                                                 Open Drain Enable Field: Open Drain Disabled
                                                 Pull / Keep Enable Field: Pull/Keeper Enabled
                                                 Pull / Keep Select Field: Keeper
                                                 Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                 Hyst. Enable Field: Hysteresis Disabled */

}