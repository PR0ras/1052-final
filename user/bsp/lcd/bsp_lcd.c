/**
  ******************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V2.0
  * @date    2018-xx-xx
  * @brief   lcdӦ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
 * ����
 ******************************************************************************/

/* ֡�жϱ�־ */
volatile bool s_frameDone = false;

/* �Դ� */
AT_NONCACHEABLE_SECTION_ALIGN( uint8_t s_frameBuffer[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH], FRAME_BUFFER_ALIGN);

pxp_ps_buffer_config_t psBufferConfig;
pxp_output_buffer_config_t outputBufferConfig;
/* ָ��ǰ���Դ� */
static uint32_t CurrentFrameBuffer = (uint32_t)s_frameBuffer[0];
static uint32_t NextFrameBuffer = (uint32_t)s_frameBuffer[1];
static sFONT *LCD_Currentfonts = &Font16x32;
/*******************************************************************************
 * ��
 ******************************************************************************/

        
/*******************************************************************************
 * ����
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
* @brief  ��ʼ��LCD���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void LCD_IOMUXC_MUX_Config(void)
{
    /* �������ž�������SION���� */
    /* ʱ������ź��� */
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LCD_CLK, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LCD_ENABLE, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LCD_HSYNC, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LCD_VSYNC, 0U);
  
    /* RGB565�����ź��ߣ�
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
    
    /* LCD_BL��������ź��� */
    IOMUXC_SetPinMux(LCD_BL_IOMUXC, 0U); 
}


/**
* @brief  ��ʼ��LCD���IOMUXC��PAD��������
* @param  ��
* @retval ��
*/
static void LCD_IOMUXC_PAD_Config(void)
{  
    /* �������ž�ʹ��ͬ����PAD���� */
    /* ʱ������ź��� */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_00_LCD_CLK,0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_LCD_ENABLE, 0x1070);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_02_LCD_HSYNC, 0x1070);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_03_LCD_VSYNC, 0x1070); 

    /* RGB565�����ź��ߣ�
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
    
    /* LCD_BL��������ź��� */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_15_GPIO1_IO15, 0x1070);
}

/**
* @brief  ��ʼ��ELCDIF����
* @param  ��
* @retval ��
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
* @brief  ��ʼ��ELCDIFʹ�õ�ʱ��
* @param  ��
* @retval ��
*/
void LCD_InitClock(void)
{
    /*
     * Ҫ��֡�����ó�60Hz����������ʱ��Ƶ��Ϊ:
     * ˮƽ����ʱ�Ӹ�����(APP_IMG_WIDTH + APP_HSW + APP_HFP + APP_HBP ) 
     * ��ֱ����ʱ�Ӹ�����(APP_IMG_HEIGHT + APP_VSW + APP_VFP + APP_VBP)
     * 
     * ����ʱ��Ƶ�ʣ�(800 + 1 + 10 + 46) * (480 + 1 + 22 + 23) * 60 = 27.05M.
     * ���������� LCDIF ����ʱ��Ƶ��Ϊ 27M.
     */

    /*
     * ��ʼ�� Video PLL.
     * Video PLL ���Ƶ��Ϊ 
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
    /* ѡ��Ϊvedio PLL*/
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    /* ���÷�Ƶ */  
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1);

    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}

/**
* @brief  ��ʼ���������Ų�����
* @param  ��
* @retval ��
*/
void LCD_BackLight_ON(void)
{    
    /* ���⣬�ߵ�ƽ���� */
    gpio_pin_config_t config = {
      kGPIO_DigitalOutput, 
      1,
      kGPIO_NoIntmode
    };

    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}



/**
* @brief  ��ʼ��Һ����
* @param  enableInterrupt ���Ƿ�ʹ���ж�
* @retval ��
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

/***************************�ж����******************************/
/**
* @brief  ����ELCDIF�ж�
* @param  ��
* @retval ��
*/
void LCD_InterruptConfig(void)
{
  /* ʹ���ж� */
  EnableIRQ(LCDIF_IRQn);
   
  /* ����ELCDIFΪCurFrameDoneInterrupt�ж� */
  ELCDIF_EnableInterrupts(APP_ELCDIF, kELCDIF_CurFrameDoneInterruptEnable);
}

/**
* @brief  ELCDIF�жϷ�����
* @param  ��
* @retval ��
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

    /* ���²�����Ϊ ARM �Ŀ���838869��ӵ�, 
       �ô���Ӱ�� Cortex-M4, Cortex-M4F�ںˣ�       
       �����洢�����ص��쳣�����·��ز������ܻ�ָ�������ж�
        CM7����Ӱ�죬�˴������ô���
    */  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/***************************��ʾ�ַ����******************************/


/**
  * @brief  ���������ʽ(Ӣ��)
  * @param  fonts: ѡ��Ҫ���õ������ʽ
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}

/**
  * @brief  ��ȡ��ǰ�����ʽ(Ӣ��)
  * @param  None.
  * @retval ��ǰӦ�õĸ�ʽ
  */
sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}

/**
  * @brief  ����ʾ������ʾһ��Ӣ���ַ�
  * @param  Xpos ���ַ�����ʼX����
  * @param  Ypos ���ַ�����ʼY����
  * @param  Ascii: Ҫ��ʾ���ַ���ASCII��
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

    /*yBufferPos��ʾ��ǰ�е��Դ�ƫ��λ��*/
    yBufferPos = Ypos * LCD_PIXEL_WIDTH * APP_BPP;

    /*xpixelPos��ʾ�������ص�λ��
    APP_BPP*xPixelPos + yBufferPos ���ǵ�ǰ���ص���Դ�λ��
  */
    xPixelPos += Xpos;

    //��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
    relativePositon = Ascii - ' ';

    //ÿ����ģ���ֽ���
    fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;

    //��ģ�׵�ַ
    /*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
    pfont = (uint8_t *)&LCD_Currentfonts->table[relativePositon * fontLength];

    //ÿ����ģ��LCD_Currentfonts->Height�У�����ÿһ��
    for (page = 0; page < LCD_Currentfonts->Height; page++)
    {
        //ÿ����ģ��LCD_Currentfonts->Width/8 ���ֽڣ�����ÿ���ֽ�
        for (column = 0; column < LCD_Currentfonts->Width / 8; column++)
        {
            uint8_t bitCount = 0;

            //ÿ���ֽ���8������λ������ÿ������λ
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
                /*ָ��ǰ�е���һ����*/
                xPixelPos++;
            }
            /* ָ����ģ���ݵ�һ�¸��ֽ� */
            pfont++;
        }
        /*��ʾ��һ��*/
        /*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
        xPixelPos += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
    }
    xPixelPos = Xpos;
    pfont = (uint8_t *)&LCD_Currentfonts->table[relativePositon * fontLength];
    for (page = 0; page < LCD_Currentfonts->Height; page++)
    {
        //ÿ����ģ��LCD_Currentfonts->Width/8 ���ֽڣ�����ÿ���ֽ�
        for (column = 0; column < LCD_Currentfonts->Width / 8; column++)
        {
            uint8_t bitCount = 0;

            //ÿ���ֽ���8������λ������ÿ������λ
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
                /*ָ��ǰ�е���һ����*/
                xPixelPos++;
            }
            /* ָ����ģ���ݵ�һ�¸��ֽ� */
            pfont++;
        }
        /*��ʾ��һ��*/
        /*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
        xPixelPos += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
    }
}

/**
 * @brief  ����ʾ������ʾ��Ӣ���ַ���,����Һ�����ʱ���Զ����С�
 * @param  Xpos ���ַ�����ʼX����
 * @param  Ypos ���ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
 */
void LCD_DispString(uint16_t Xpos, uint16_t Ypos, const uint8_t *pStr)
{
    while (*pStr != '\0')
    {
        /*�Զ�����*/
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
        /* ��ʾ�����ַ� */
        LCD_DisplayChar(Xpos, Ypos, *pStr);
        Xpos += LCD_Currentfonts->Width;
        pStr++;
    }
}

/**
  * @brief  ��ʾ�ַ���(Ӣ��)
  * @param  Line: ���ݵ�ǰ���������к�
  *     @arg Line(1),Line(2)��
  * @param  *ptr: Ҫ��ʾ���ַ���
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
    uint16_t refcolumn = 0;
    /* ѭ����ʾ�ַ���ֱ�������ַ���������
    ��ֱ��������ʾ�����ַ�
  */
    while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) &
                                             (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width)))
    {
        /* ��ʾ�����ַ� */
        LCD_DisplayChar(refcolumn, Line, *ptr);
        /* ƫ���ַ���ʾλ�� */
        refcolumn += LCD_Currentfonts->Width;
        /* ָ����һ���ַ� */
        ptr++;
    }
}

/**
  * @brief  ���ָ���е��ַ�
  * @param  Line: Ҫ�������,ע��LINE���Ǹ��ݵ�ǰ��������
  *     @arg LINE(1),LINE(2)
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
    uint16_t refcolumn = 0;
    /* ѭ����ʾ����Ļ���Ҳ� */
    while ((refcolumn < LCD_PIXEL_WIDTH) &&
           (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >= LCD_Currentfonts->Width))
    {
        /* ��ʾ�ո��൱�������Ч���� */
        LCD_DisplayChar(refcolumn, Line, ' ');
        /* ƫ���ַ���ʾλ�� */
        refcolumn += LCD_Currentfonts->Width;
    }
}

/**
  * @brief  ������ʾ����
  * @param  Xpos: x����
  * @param  Ypos: y����
  * @retval �Դ�ĵ�ַ
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    return CurrentFrameBuffer + APP_BPP * (Xpos + (LCD_PIXEL_WIDTH * Ypos));
}

/*********************************************END OF FILE**********************/
