/**
  ******************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V2.0
  * @date    2018-xx-xx
  * @brief   lcd应用函数接口
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_elcdif.h" 
#include "fsl_clock.h"
#include "fsl_pxp.h"
#include "LQ_MT9V034M.h"
#include "./lcd/bsp_lcd.h" 



/*******************************************************************************
 * 变量
 ******************************************************************************/

/* 帧中断标志 */
volatile bool s_frameDone = false;

/* 显存 */
AT_NONCACHEABLE_SECTION_ALIGN( uint8_t s_frameBuffer[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH], FRAME_BUFFER_ALIGN);

pxp_ps_buffer_config_t psBufferConfig;
pxp_output_buffer_config_t outputBufferConfig;
/* 指向当前的显存 */
static uint32_t CurrentFrameBuffer = (uint32_t)s_frameBuffer[0];
static uint32_t NextFrameBuffer = (uint32_t)s_frameBuffer[1];
static sFONT *LCD_Currentfonts = &Font16x32;
/*******************************************************************************
 * 宏
 ******************************************************************************/

        
/*******************************************************************************
 * 声明
 ******************************************************************************/
static void LCD_IOMUXC_MUX_Config(void);
static void LCD_IOMUXC_PAD_Config(void);
static void LCD_ELCDIF_Config(void);

static const uint32_t elcdif_gray2rgb565_lut[256] = {
    GRAY_2_RGB565(0),  GRAY_2_RGB565(1),  GRAY_2_RGB565(2),  GRAY_2_RGB565(3),
    GRAY_2_RGB565(4),  GRAY_2_RGB565(5),  GRAY_2_RGB565(6),  GRAY_2_RGB565(7),
    GRAY_2_RGB565(8),  GRAY_2_RGB565(9),  GRAY_2_RGB565(10), GRAY_2_RGB565(11),
    GRAY_2_RGB565(12), GRAY_2_RGB565(13), GRAY_2_RGB565(14), GRAY_2_RGB565(15),
    GRAY_2_RGB565(16), GRAY_2_RGB565(17), GRAY_2_RGB565(18), GRAY_2_RGB565(19),
    GRAY_2_RGB565(20), GRAY_2_RGB565(21), GRAY_2_RGB565(22), GRAY_2_RGB565(23),
    GRAY_2_RGB565(24), GRAY_2_RGB565(25), GRAY_2_RGB565(26), GRAY_2_RGB565(27),
    GRAY_2_RGB565(28), GRAY_2_RGB565(29), GRAY_2_RGB565(30), GRAY_2_RGB565(31),
    
    GRAY_2_RGB565(32), GRAY_2_RGB565(33), GRAY_2_RGB565(34), GRAY_2_RGB565(35),
    GRAY_2_RGB565(36), GRAY_2_RGB565(37), GRAY_2_RGB565(38), GRAY_2_RGB565(39),
    GRAY_2_RGB565(40), GRAY_2_RGB565(41), GRAY_2_RGB565(42), GRAY_2_RGB565(43),
    GRAY_2_RGB565(44), GRAY_2_RGB565(45), GRAY_2_RGB565(46), GRAY_2_RGB565(47),
    GRAY_2_RGB565(48), GRAY_2_RGB565(49), GRAY_2_RGB565(50), GRAY_2_RGB565(51),
    GRAY_2_RGB565(52), GRAY_2_RGB565(53), GRAY_2_RGB565(54), GRAY_2_RGB565(55),
    GRAY_2_RGB565(56), GRAY_2_RGB565(57), GRAY_2_RGB565(58), GRAY_2_RGB565(59),
    GRAY_2_RGB565(60), GRAY_2_RGB565(61), GRAY_2_RGB565(62), GRAY_2_RGB565(63),
    
    GRAY_2_RGB565(64), GRAY_2_RGB565(65), GRAY_2_RGB565(66), GRAY_2_RGB565(67),
    GRAY_2_RGB565(68), GRAY_2_RGB565(69), GRAY_2_RGB565(70), GRAY_2_RGB565(71),
    GRAY_2_RGB565(72), GRAY_2_RGB565(73), GRAY_2_RGB565(74), GRAY_2_RGB565(75),
    GRAY_2_RGB565(76), GRAY_2_RGB565(77), GRAY_2_RGB565(78), GRAY_2_RGB565(79),
    GRAY_2_RGB565(80), GRAY_2_RGB565(81), GRAY_2_RGB565(82), GRAY_2_RGB565(83),
    GRAY_2_RGB565(84), GRAY_2_RGB565(85), GRAY_2_RGB565(86), GRAY_2_RGB565(87),
    GRAY_2_RGB565(88), GRAY_2_RGB565(89), GRAY_2_RGB565(90), GRAY_2_RGB565(91),
    GRAY_2_RGB565(92), GRAY_2_RGB565(93), GRAY_2_RGB565(94), GRAY_2_RGB565(95),
    
    GRAY_2_RGB565(96), GRAY_2_RGB565(97), GRAY_2_RGB565(98), GRAY_2_RGB565(99),
    GRAY_2_RGB565(100),GRAY_2_RGB565(101),GRAY_2_RGB565(102),GRAY_2_RGB565(103),
    GRAY_2_RGB565(104),GRAY_2_RGB565(105),GRAY_2_RGB565(106),GRAY_2_RGB565(107),
    GRAY_2_RGB565(108),GRAY_2_RGB565(109),GRAY_2_RGB565(110),GRAY_2_RGB565(111),
    GRAY_2_RGB565(112),GRAY_2_RGB565(113),GRAY_2_RGB565(114),GRAY_2_RGB565(115),
    GRAY_2_RGB565(116),GRAY_2_RGB565(117),GRAY_2_RGB565(118),GRAY_2_RGB565(119),
    GRAY_2_RGB565(120),GRAY_2_RGB565(121),GRAY_2_RGB565(122),GRAY_2_RGB565(123),
    GRAY_2_RGB565(124),GRAY_2_RGB565(125),GRAY_2_RGB565(126),GRAY_2_RGB565(127),
    
    GRAY_2_RGB565(128),GRAY_2_RGB565(129),GRAY_2_RGB565(130),GRAY_2_RGB565(131),
    GRAY_2_RGB565(132),GRAY_2_RGB565(133),GRAY_2_RGB565(134),GRAY_2_RGB565(135),
    GRAY_2_RGB565(136),GRAY_2_RGB565(137),GRAY_2_RGB565(138),GRAY_2_RGB565(139),
    GRAY_2_RGB565(140),GRAY_2_RGB565(141),GRAY_2_RGB565(142),GRAY_2_RGB565(143),
    GRAY_2_RGB565(144),GRAY_2_RGB565(145),GRAY_2_RGB565(146),GRAY_2_RGB565(147),
    GRAY_2_RGB565(148),GRAY_2_RGB565(149),GRAY_2_RGB565(150),GRAY_2_RGB565(151),
    GRAY_2_RGB565(152),GRAY_2_RGB565(153),GRAY_2_RGB565(154),GRAY_2_RGB565(155),
    GRAY_2_RGB565(156),GRAY_2_RGB565(157),GRAY_2_RGB565(158),GRAY_2_RGB565(159),
    
    GRAY_2_RGB565(160),GRAY_2_RGB565(161),GRAY_2_RGB565(162),GRAY_2_RGB565(163),
    GRAY_2_RGB565(164),GRAY_2_RGB565(165),GRAY_2_RGB565(166),GRAY_2_RGB565(167),
    GRAY_2_RGB565(168),GRAY_2_RGB565(169),GRAY_2_RGB565(170),GRAY_2_RGB565(171),
    GRAY_2_RGB565(172),GRAY_2_RGB565(173),GRAY_2_RGB565(174),GRAY_2_RGB565(175),
    
    GRAY_2_RGB565(176),GRAY_2_RGB565(177),GRAY_2_RGB565(178),GRAY_2_RGB565(179),
    GRAY_2_RGB565(180),GRAY_2_RGB565(181),GRAY_2_RGB565(182),GRAY_2_RGB565(183),
    GRAY_2_RGB565(184),GRAY_2_RGB565(185),GRAY_2_RGB565(186),GRAY_2_RGB565(187),
    GRAY_2_RGB565(188),GRAY_2_RGB565(189),GRAY_2_RGB565(190),GRAY_2_RGB565(191),
    
    GRAY_2_RGB565(192),GRAY_2_RGB565(193),GRAY_2_RGB565(194),GRAY_2_RGB565(195),
    GRAY_2_RGB565(196),GRAY_2_RGB565(197),GRAY_2_RGB565(198),GRAY_2_RGB565(199),
    GRAY_2_RGB565(200),GRAY_2_RGB565(201),GRAY_2_RGB565(202),GRAY_2_RGB565(203),
    GRAY_2_RGB565(204),GRAY_2_RGB565(205),GRAY_2_RGB565(206),GRAY_2_RGB565(207),
    
    GRAY_2_RGB565(208),GRAY_2_RGB565(209),GRAY_2_RGB565(210),GRAY_2_RGB565(211),
    GRAY_2_RGB565(212),GRAY_2_RGB565(213),GRAY_2_RGB565(214),GRAY_2_RGB565(215),
    GRAY_2_RGB565(216),GRAY_2_RGB565(217),GRAY_2_RGB565(218),GRAY_2_RGB565(219),
    GRAY_2_RGB565(220),GRAY_2_RGB565(221),GRAY_2_RGB565(222),GRAY_2_RGB565(223),
    
    GRAY_2_RGB565(224),GRAY_2_RGB565(225),GRAY_2_RGB565(226),GRAY_2_RGB565(227),
    GRAY_2_RGB565(228),GRAY_2_RGB565(229),GRAY_2_RGB565(230),GRAY_2_RGB565(231),
    GRAY_2_RGB565(232),GRAY_2_RGB565(233),GRAY_2_RGB565(234),GRAY_2_RGB565(235),
    GRAY_2_RGB565(236),GRAY_2_RGB565(237),GRAY_2_RGB565(238),GRAY_2_RGB565(239),
    
    GRAY_2_RGB565(240),GRAY_2_RGB565(241),GRAY_2_RGB565(242),GRAY_2_RGB565(243),
    GRAY_2_RGB565(244),GRAY_2_RGB565(245),GRAY_2_RGB565(246),GRAY_2_RGB565(247),
    GRAY_2_RGB565(248),GRAY_2_RGB565(249),GRAY_2_RGB565(250),GRAY_2_RGB565(251),
    GRAY_2_RGB565(252),GRAY_2_RGB565(253),GRAY_2_RGB565(254),GRAY_2_RGB565(255)
};
/**
* @brief  初始化LCD相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void LCD_IOMUXC_MUX_Config(void)
{
    /* 所有引脚均不开启SION功能 */
    /* 时序控制信号线 */
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LCD_CLK, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LCD_ENABLE, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LCD_HSYNC, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LCD_VSYNC, 0U);
  
    /* RGB565数据信号线，
     DATA0~DATA4:B3~B7
     DATA5~DATA10:G2~G7
     DATA11~DATA15:R3~R7 */
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_04_LCD_DATA00, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_05_LCD_DATA01, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_LCD_DATA02, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_LCD_DATA03, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_LCD_DATA04, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_LCD_DATA05, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_LCD_DATA06, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_11_LCD_DATA07, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_12_LCD_DATA08, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_13_LCD_DATA09, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_14_LCD_DATA10, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_15_LCD_DATA11, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_00_LCD_DATA12, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_LCD_DATA13, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_02_LCD_DATA14, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_LCD_DATA15, 0U);   
    
    /* LCD_BL背光控制信号线 */
    IOMUXC_SetPinMux(LCD_BL_IOMUXC, 0U); 
}


/**
* @brief  初始化LCD相关IOMUXC的PAD属性配置
* @param  无
* @retval 无
*/
static void LCD_IOMUXC_PAD_Config(void)
{  
    /* 所有引脚均使用同样的PAD配置 */
    /* 时序控制信号线 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_00_LCD_CLK,0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_LCD_ENABLE, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_02_LCD_HSYNC, 0x1070);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_03_LCD_VSYNC, 0x1070); 

    /* RGB565数据信号线，
     DATA0~DATA4:B3~B7
     DATA5~DATA10:G2~G7
     DATA11~DATA15:R3~R7 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_04_LCD_DATA00, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_05_LCD_DATA01, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_LCD_DATA02, 0x1070);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_LCD_DATA03, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_LCD_DATA04, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_LCD_DATA05, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_LCD_DATA06, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_11_LCD_DATA07, 0x1070);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_12_LCD_DATA08, 0x1070);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_13_LCD_DATA09, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_14_LCD_DATA10, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_15_LCD_DATA11, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_00_LCD_DATA12, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_LCD_DATA13, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_02_LCD_DATA14, 0x1070); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_LCD_DATA15, 0x1070); 
    
    /* LCD_BL背光控制信号线 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_15_GPIO1_IO15, 0x1070);
}

/**
* @brief  初始化ELCDIF外设
* @param  无
* @retval 无
*/
static void LCD_ELCDIF_Config(void)
{	
    const elcdif_rgb_mode_config_t config = {
        .panelWidth = LCD_PIXEL_WIDTH,
        .panelHeight = LCD_PIXEL_HEIGHT,
        .hsw = APP_HSW,
        .hfp = APP_HFP,
        .hbp = APP_HBP,
        .vsw = APP_VSW,
        .vfp = APP_VFP,
        .vbp = APP_VBP,
        .polarityFlags = APP_POL_FLAGS,
        .bufferAddr = (uint32_t)s_frameBuffer[0],
        .pixelFormat = kELCDIF_PixelFormatRAW8,
        .dataBus = kELCDIF_DataBus8Bit,
    };
 
  ELCDIF_RgbModeInit(APP_ELCDIF, &config);
  ELCDIF_UpdateLut(LCDIF, kELCDIF_Lut0, 0, elcdif_gray2rgb565_lut, sizeof(elcdif_gray2rgb565_lut) / sizeof(elcdif_gray2rgb565_lut[0]));
  ELCDIF_UpdateLut(LCDIF, kELCDIF_Lut1, 0, elcdif_gray2rgb565_lut, sizeof(elcdif_gray2rgb565_lut) / sizeof(elcdif_gray2rgb565_lut[0]));
  ELCDIF_EnableLut(APP_ELCDIF, true);
  //ELCDIF_RgbModeStart(APP_ELCDIF);
}

/**
* @brief  初始化ELCDIF使用的时钟
* @param  无
* @retval 无
*/
void LCD_InitClock(void)
{
    /*
     * 要把帧率设置成60Hz，所以像素时钟频率为:
     * 水平像素时钟个数：(APP_IMG_WIDTH + APP_HSW + APP_HFP + APP_HBP ) 
     * 垂直像素时钟个数：(APP_IMG_HEIGHT + APP_VSW + APP_VFP + APP_VBP)
     * 
     * 像素时钟频率：(800 + 1 + 10 + 46) * (480 + 1 + 22 + 23) * 60 = 27.05M.
     * 本例子设置 LCDIF 像素时钟频率为 27M.
     */

    /*
     * 初始化 Video PLL.
     * Video PLL 输出频率为 
     * OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 108MHz.
     */
    clock_video_pll_config_t config = {
        .loopDivider = 36, .postDivider = 8, .numerator = 0, .denominator = 0,
    };

    CLOCK_InitVideoPll(&config);

    /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    /* 选择为vedio PLL*/
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    /* 设置分频 */  
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1);

    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}

/**
* @brief  初始化背光引脚并点亮
* @param  无
* @retval 无
*/
void LCD_BackLight_ON(void)
{    
    /* 背光，高电平点亮 */
    gpio_pin_config_t config = {
      kGPIO_DigitalOutput, 
      1,
      kGPIO_NoIntmode
    };

    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}



/**
* @brief  初始化液晶屏
* @param  enableInterrupt ：是否使能中断
* @retval 无
*/
void LCD_Init(bool enableInterrupt)
{
  *((uint32_t *)0x41044100) = 0x0000000f;
	*((uint32_t *)0x41044104) = 0x0000000f;

  LCD_IOMUXC_MUX_Config();
  LCD_IOMUXC_PAD_Config();
  LCD_ELCDIF_Config();
  LCD_InitClock();
  LCD_BackLight_ON();
  
  if(enableInterrupt)
  {
    LCD_InterruptConfig();
  }
}

// void LCD_PXP_Config(void)
// {
//     PXP_Init(APP_PXP);
// 	PXP_SetAlphaSurfacePosition(APP_PXP, 0xFFFFU, 0xFFFFU, 0U, 0U);
//     /* PS configure. */
// 	psBufferConfig.pixelFormat =  kPXP_PsPixelFormatY8;//kPXP_PsPixelFormatRGB888;
// 	psBufferConfig.swapByte = false; 
//     psBufferConfig.bufferAddrU = 0U;
//     psBufferConfig.bufferAddrV = 0U;
//     psBufferConfig.pitchBytes = 752;  
	
// 	outputBufferConfig.pixelFormat = kPXP_OutputPixelFormatRGB565;//kPXP_OutputPixelFormatRGB888,
//     outputBufferConfig.interlacedMode = kPXP_OutputProgressive;
//     outputBufferConfig.buffer1Addr = 0U;
//     outputBufferConfig.pitchBytes = 800 * 4;
//     outputBufferConfig.width = LCD_PIXEL_WIDTH;
//     outputBufferConfig.height = LCD_PIXEL_HEIGHT;
	
// 	PXP_EnableCsc1(PXP, false);             
//     PXP_SetProcessSurfaceScaler(PXP,IMAGEW, IMAGEH, 752, 480);
	
//     PXP_SetProcessSurfacePosition(PXP, 32u, 12u, 752 - 1U, 480 - 1U); 
	
//     PXP_SetProcessSurfaceBackGroundColor(APP_PXP, 0U);
// 	PXP_SetRotateConfig(PXP, kPXP_RotateOutputBuffer, kPXP_Rotate180, kPXP_FlipDisable);
//     //PXP_SetProcessSurfaceBufferConfig(APP_PXP, &psBufferConfig);

//     /* Disable AS. */

//     //PXP_SetOutputBufferConfig(APP_PXP, &outputBufferConfig);

//     /* Disable CSC1, it is enabled by default. */
//     PXP_EnableCsc1(APP_PXP, false);
// }

/***************************中断相关******************************/
/**
* @brief  配置ELCDIF中断
* @param  无
* @retval 无
*/
void LCD_InterruptConfig(void)
{
  /* 使能中断 */
  EnableIRQ(LCDIF_IRQn);
   
  /* 配置ELCDIF为CurFrameDoneInterrupt中断 */
  ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
}

/**
* @brief  ELCDIF中断服务函数
* @param  无
* @retval 无
*/
void LCDIF_IRQHandler(void)
{
    uint32_t intStatus;

    intStatus = ELCDIF_GetInterruptStatus(APP_ELCDIF);

    ELCDIF_ClearInterruptStatus(APP_ELCDIF, intStatus);

    if (intStatus & kELCDIF_CurFrameDone)
    {
        s_frameDone = true;
    }

    /* 以下部分是为 ARM 的勘误838869添加的, 
       该错误影响 Cortex-M4, Cortex-M4F内核，       
       立即存储覆盖重叠异常，导致返回操作可能会指向错误的中断
        CM7不受影响，此处保留该代码
    */  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/***************************显示字符相关******************************/


/**
  * @brief  设置字体格式(英文)
  * @param  fonts: 选择要设置的字体格式
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}

/**
  * @brief  获取当前字体格式(英文)
  * @param  None.
  * @retval 当前应用的格式
  */
sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}

/**
  * @brief  在显示器上显示一个英文字符
  * @param  Xpos ：字符的起始X坐标
  * @param  Ypos ：字符的起始Y坐标
  * @param  Ascii: 要显示的字符的ASCII码
  * @retval None
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, char Ascii)
{
    uint16_t fontLength;
    uint16_t page, column;

    uint16_t relativePositon;
    uint8_t *pfont;

    uint32_t yBufferPos = 0;
    uint32_t xPixelPos = 0;

    /*yBufferPos表示当前行的显存偏移位置*/
    yBufferPos = Ypos * LCD_PIXEL_WIDTH * APP_BPP;

    /*xpixelPos表示部分像素点位置
    APP_BPP*xPixelPos + yBufferPos 就是当前像素点的显存位置
  */
    xPixelPos += Xpos;

    //对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
    relativePositon = Ascii - ' ';

    //每个字模的字节数
    fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;

    //字模首地址
    /*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
    pfont = (uint8_t *)&LCD_Currentfonts->table[relativePositon * fontLength];

    //每个字模有LCD_Currentfonts->Height行，遍历每一行
    for (page = 0; page < LCD_Currentfonts->Height; page++)
    {
        //每个字模有LCD_Currentfonts->Width/8 个字节，遍历每个字节
        for (column = 0; column < LCD_Currentfonts->Width / 8; column++)
        {
            uint8_t bitCount = 0;

            //每个字节有8个数据位，遍历每个数据位
            for (bitCount = 0; bitCount < 8; bitCount++)
            {
                if (*pfont & (0x80 >> bitCount))
                {
                    *(__IO uint8_t *)(CurrentFrameBuffer + xPixelPos + yBufferPos) = 0xFF; //XRGB
                }
                else
                {
                    *(__IO uint8_t *)(CurrentFrameBuffer + xPixelPos + yBufferPos) = 0x0; //XRGB
                }
                /*指向当前行的下一个点*/
                xPixelPos++;
            }
            /* 指向字模数据的一下个字节 */
            pfont++;
        }
        /*显示完一行*/
        /*指向字符显示矩阵下一行的第一个像素点*/
        xPixelPos += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
    }
    xPixelPos = Xpos;
    pfont = (uint8_t *)&LCD_Currentfonts->table[relativePositon * fontLength];
    for (page = 0; page < LCD_Currentfonts->Height; page++)
    {
        //每个字模有LCD_Currentfonts->Width/8 个字节，遍历每个字节
        for (column = 0; column < LCD_Currentfonts->Width / 8; column++)
        {
            uint8_t bitCount = 0;

            //每个字节有8个数据位，遍历每个数据位
            for (bitCount = 0; bitCount < 8; bitCount++)
            {
                if (*pfont & (0x80 >> bitCount))
                {
                    *(__IO uint8_t *)(NextFrameBuffer + xPixelPos + yBufferPos) = 0xFF; //XRGB
                }
                else
                {
                    *(__IO uint8_t *)(NextFrameBuffer + xPixelPos + yBufferPos) = 0x0; //XRGB
                }
                /*指向当前行的下一个点*/
                xPixelPos++;
            }
            /* 指向字模数据的一下个字节 */
            pfont++;
        }
        /*显示完一行*/
        /*指向字符显示矩阵下一行的第一个像素点*/
        xPixelPos += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
    }
}

/**
 * @brief  在显示器上显示中英文字符串,超出液晶宽度时会自动换行。
 * @param  Xpos ：字符的起始X坐标
 * @param  Ypos ：字符的起始Y坐标
 * @param  pStr ：要显示的字符串的首地址
 * @retval 无
 */
void LCD_DispString(uint16_t Xpos, uint16_t Ypos, const uint8_t *pStr)
{
    while (*pStr != '\0')
    {
        /*自动换行*/
        if ((Xpos + LCD_Currentfonts->Width) > LCD_PIXEL_WIDTH)
        {
            Xpos = 0;
            Ypos += LCD_Currentfonts->Height;
        }

        if ((Ypos + LCD_Currentfonts->Height) > LCD_PIXEL_HEIGHT)
        {
            Xpos = 0;
            Ypos = 0;
        }
        /* 显示单个字符 */
        LCD_DisplayChar(Xpos, Ypos, *pStr);
        Xpos += LCD_Currentfonts->Width;
        pStr++;
    }
}

/**
  * @brief  显示字符串(英文)
  * @param  Line: 根据当前字体而变的行号
  *     @arg Line(1),Line(2)等
  * @param  *ptr: 要显示的字符串
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
    uint16_t refcolumn = 0;
    /* 循环显示字符，直至遇到字符串结束符
    或直到单行显示不下字符
  */
    while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) &
                                             (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
    {
        /* 显示单个字符 */
        LCD_DisplayChar(refcolumn, Line, *ptr);
        /* 偏移字符显示位置 */
        refcolumn += LCD_Currentfonts->Width;
        /* 指向下一个字符 */
        ptr++;
    }
}

/**
  * @brief  清除指定行的字符
  * @param  Line: 要清除的行,注意LINE宏是根据当前字体而变的
  *     @arg LINE(1),LINE(2)
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
    uint16_t refcolumn = 0;
    /* 循环显示至屏幕最右侧 */
    while ((refcolumn < LCD_PIXEL_WIDTH) &&
           (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width))
    {
        /* 显示空格（相当于清除的效果） */
        LCD_DisplayChar(refcolumn, Line, ' ');
        /* 偏移字符显示位置 */
        refcolumn += LCD_Currentfonts->Width;
    }
}

/**
  * @brief  设置显示坐标
  * @param  Xpos: x坐标
  * @param  Ypos: y坐标
  * @retval 显存的地址
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    return CurrentFrameBuffer + APP_BPP * (Xpos + (LCD_PIXEL_WIDTH * Ypos));
}

/*********************************************END OF FILE**********************/
