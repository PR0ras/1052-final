#ifndef __BSP_LCD_H
#define	__BSP_LCD_H

#include "fsl_common.h"
#include "./font/fonts.h"


#define APP_ELCDIF    LCDIF
#define APP_PXP       PXP


#define LCD_PIXEL_WIDTH     800
#define LCD_PIXEL_HEIGHT    480

#define APP_IMG_WIDTH   LCD_PIXEL_WIDTH
#define APP_IMG_HEIGHT  LCD_PIXEL_HEIGHT


#define APP_HSW 1
#define APP_HFP 22
#define APP_HBP 46
#define APP_VSW 1
#define APP_VFP 22
#define APP_VBP 22
#define APP_POL_FLAGS \
    (kELCDIF_DataEnableActiveHigh | kELCDIF_VsyncActiveLow | kELCDIF_HsyncActiveLow | kELCDIF_DriveDataOnRisingClkEdge)



#define APP_LCDIF_DATA_BUS    kELCDIF_DataBus8Bit


#define LCD_BL_GPIO 				      GPIO1
#define LCD_BL_GPIO_PIN 		      (15U)
#define LCD_BL_IOMUXC			        IOMUXC_GPIO_AD_B0_15_GPIO1_IO15

#define FRAME_BUFFER_ALIGN    64


/* PS input buffer is square. */
#if APP_IMG_WIDTH > APP_IMG_HEIGHT
#define APP_PS_SIZE (APP_IMG_HEIGHT / 2U)
#else
#define APP_PS_SIZE (APP_IMG_WIDTH / 2U)
#endif

#define APP_PS_ULC_X 0U
#define APP_PS_ULC_Y 0U

#define APP_BPP 1U /* Use 24-bit RGB888 format. */

#define LCD_XRGB_8888             1
//#define LCD_RGB_888             1
//#define LCD_RGB_565             1

#define LCD_INTERRUPT_DISABLE   0
#define LCD_INTERRUPT_ENABLE    (!LCD_INTERRUPT_DISABLE)

#define LINE_DIR_HORIZONTAL       0x0
#define LINE_DIR_VERTICAL         0x1

#define RGB565_R(rgb565)                ((uint8_t)(((uint16_t)(rgb565)>>11) &0x1F))
#define RGB565_G(rgb565)                ((uint8_t)(((uint16_t)(rgb565)>> 5) &0x3F))
#define RGB565_B(rgb565)                ((uint8_t)( (uint16_t)(rgb565)      &0x1F))

#define GRAY_2_RGB565(gray)             ((uint16_t)((((uint8_t)(gray)>>3)<<11)|(((uint8_t)(gray)>>2)<<5)|((uint8_t)(gray)>>3)))
#define GRAY_2_RGB888(gray)             ((uint32_t)((((uint8_t)gray)<<16)|(((uint8_t)gray)<<8)|(((uint8_t)gray)<<0)))
#define RGB565_2_GRAY(rgb565)           ((uint8_t)(((RGB565_R(rgb565)*235+RGB565_G(rgb565)*613+RGB565_B(rgb565)*625)+1)>>8))  //  31*235+63*613+31*625+1  = 255*256
/** 
  * @brief  LCD color  
  */
#define LCD_COLOR_WHITE          0xFFFFFF
#define LCD_COLOR_BLACK          0x000000
#define LCD_COLOR_GREY           0xC0C0C0
#define LCD_COLOR_BLUE           0x0000FF
#define LCD_COLOR_BLUE2          0x0000A0
#define LCD_COLOR_RED            0xFF0000
#define LCD_COLOR_MAGENTA        0xFF00FF
#define LCD_COLOR_GREEN          0x00FF00
#define LCD_COLOR_CYAN           0x00FFFF
#define LCD_COLOR_YELLOW         0xFFFF00



extern volatile bool s_frameDone;
extern uint8_t s_frameBuffer[2][APP_IMG_HEIGHT][APP_IMG_WIDTH];


void LCD_BackLight_ON(void);
void LCD_Init(bool enableInterrupt);
void APP_FillFrameBuffer(uint8_t frameBuffer[APP_IMG_HEIGHT][APP_IMG_WIDTH]);
void LCD_InterruptConfig(void);


void LCD_SetColors(uint32_t TextColor, uint32_t BackColor); 
void LCD_GetColors(uint32_t *TextColor, uint32_t *BackColor);
void LCD_SetTextColor(uint32_t Color);
void LCD_SetBackColor(uint32_t Color);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, char Ascii);
void LCD_DispString(uint16_t Xpos, uint16_t Ypos, const uint8_t * pStr );
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void LCD_ClearLine(uint16_t Line);
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);

void LCD_PXP_Config(void);

#endif /* __BSP_LCD_H */
