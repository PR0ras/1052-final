#include "carmer.h"
//#include "oled2.h"
#if defined(USE_LCD) && (USE_LCD == 1U)
#include "./lcd/bsp_lcd.h"
#include "./font/fonts.h"
#endif
#include "bwLabel.h"
#include "1052_NVIC.h"
//#include "ALEX_MT9V034.h"
#include "LQ_MT9V034M.h"

/* RT-Thread 头文件 */
#include "rtthread.h"

#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "fsl_pxp.h"
#include "fsl_elcdif.h"

uint8_t Pix_Data[22560] = {0};
uint8_t Lab_Data[22560] = {0};
uint8_t Uart_Data[22560] = {0};
//uint8_t Buffer_Data[600]={0};
//uint8_t cmos[60][80]={0,0};
//uint8_t oledcmos[60][80]={0};

uint8_t camover = 0, FPStmp = 0, FPS = 0,imgpro_running=0;
uint32_t activeFrameAddr = (uint32_t)&csiFrameBuf[0];
uint32_t inactiveFrameAddr = (uint32_t)&csiFrameBuf[0];
uint8_t img_start[2] = {0xaa, 0x55};
uint8_t img_end[2] = {0x55, 0xaa};
uint16_t tmp11=0;

char dispBuff[50];
char dispBuff1[50];
char dispBuff2[50];
char dispBuff3[50];
char dispBuff4[50];
char dispBuff5[50];
char dispBuff6[50];
char dispBuff7[50];
char OLEDdispBuff1[20];
char OLEDdispBuff2[20];

extern pxp_ps_buffer_config_t psBufferConfig;
extern pxp_output_buffer_config_t outputBufferConfig;
extern volatile bool s_frameDone;
//extern void MT9V034_SetFrameResolution(uint16_t height, uint16_t width, uint8_t fps);
extern uint32_t cnt;
extern bool txOnGoing;
extern lpuart_transfer_t sendXfer;
AT_NONCACHEABLE_SECTION_ALIGN(uint8_t uartimg[94][60], FRAME_BUFFER_ALIGN);

static csi_private_data_t csiPrivateData;

static csi_resource_t csiResource = {
	.csiBase = CSI,
};

camera_receiver_handle_t cameraReceiver = {
	.resource = &csiResource,
	.ops = &csi_ops,
	.privateData = &csiPrivateData,
};

//static LQMT9V034_resource_t LQMT9V034Resource = {
//	//摄像头初始化结构体
//	.sccbI2C = LPI2C1,
//	.inputClockFreq_Hz = 27000000,
//};

//camera_device_handle_t cameraDevice = {
//	.resource = &LQMT9V034Resource,
//	.ops = &LQMT9V034_ops,
//};

/* Camera设备属性配置 */
const camera_config_t cameraConfig = {
	.pixelFormat = kVIDEO_PixelFormatXRGB8888, //THIS PARAM HAS NO MEANNING kVIDEO_PixelFormatYUYV kVIDEO_PixelFormatXRGB8888
	.bytesPerPixel = CAMERA_BPP,
	.resolution = FSL_VIDEO_RESOLUTION(CAMERA_WIDTH, CAMERA_HEIGHT),
	.frameBufferLinePitch_Bytes = CAMERA_WIDTH * CAMERA_BPP,
	.interface = kCAMERA_InterfaceGatedClock,
	.controlFlags = CAMERA_CONTROL_FLAGS,
	.framePerSec = 60, //THIS PARAM HAS NO MEANNING
};

void Campin_Init(void)
{
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_CSI_PIXCLK, 0U); /* GPIO_AD_B1_04 is configured as CSI_PIXCLK */
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_13_CSI_VSYNC, 0U);  /* GPIO_AD_B1_06 is configured as CSI_VSYNC */
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_14_CSI_HSYNC, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_08_CSI_DATA09, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_09_CSI_DATA08, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_CSI_DATA07, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_CSI_DATA06, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_12_CSI_DATA05, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_13_CSI_DATA04, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_CSI_DATA03, 0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_CSI_DATA02, 0U);

	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 1U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 1U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 0xD8B0u);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 0xD8B0u);

	//	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_GPIO1_IO16, 1U);	// MT9V034 GPIO~TWI~SCL
	//  IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_GPIO1_IO17, 1U);	// MT9V034 GPIO~TWI~SDA
}

void BOARD_InitCameraResource(void)
{
	LPI2C1_Init(100000);
}

extern void CSI_DriverIRQHandler(void);

void CSI_IRQHandler(void)
{
	CSI_DriverIRQHandler();
}

void CAMCSI_Init(void)
{
	Campin_Init();
	//MT9V034_Default_Settings();
	BOARD_InitCameraResource();
	memset(csiFrameBuf, 0, sizeof(csiFrameBuf));
	CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, NULL, NULL);
	//CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
	//MT9V034_SetFrameResolution(480, 752, 60);
	// MT9V034_night();
	MT9V034_night_lv();
	// MT9V034_daily();
	RT1052_NVIC_SetPriority(CSI_IRQn, 3, 0);
	for (uint32_t i = 0; i < CAMERA_FRAME_BUFFER_COUNT; i++)
	{
		CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(csiFrameBuf[i]));
	}
	//PRINTF("Camera Start\r\n");//摄像头启动成功
}

void CAM_DIS(void)
{
	sendXfer.data = (uint8_t *)csiFrameBuf[0];
	sendXfer.dataSize = sizeof(csiFrameBuf[0]);

	uint8_t curLcdBufferIdx = 1U;
	CAMERA_RECEIVER_Start(&cameraReceiver);
	while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &activeFrameAddr))
	{
		;
	}
	while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &inactiveFrameAddr))
	{
		;
	}
#if defined(USE_LCD) && (USE_LCD == 1U)
	LCDIF->CUR_BUF = activeFrameAddr;
	ELCDIF_SetNextBufferAddr(LCDIF, inactiveFrameAddr);
	ELCDIF_RgbModeStart(LCDIF);
#endif
	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);
	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, inactiveFrameAddr);
	PRINTF("CAMERA Start Success\r\n");
	//	cnt=0;
	//	wallner(csiFrameBuf[0], Pix_Data);
	//	PRINTF("wallner Time:%d \r\n",cnt);
	//	cnt=0;
	//	wallner_new(csiFrameBuf[0], Lab_Data);
	//	PRINTF("wallner_new Time:%d \r\n",cnt);
	cnt = 0;
	//	edge_dect(csiFrameBuf[0]);
	edge_bw((uint8_t *)inactiveFrameAddr, Pix_Data, Lab_Data);
	// bwlabel(Pix_Data,8,Lab_Data);
	//	//testSend();
	PRINTF("bwlabel Time :%d \r\n", cnt);

	while (1)
	{
		FPStmp++;
		curLcdBufferIdx ^= 1U;
		// Return the camera buffer to camera queue.
		while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &inactiveFrameAddr))
		{
		}

		if(!txOnGoing)
			sendXfer.data = (uint8_t *)inactiveFrameAddr;
		// edge_dect((uint8_t *)inactiveFrameAddr);
		// wallner((uint8_t *)inactiveFrameAddr, Pix_Data);
		cnt = 0;
		rt_enter_critical();
		edge_thr((uint8_t *)inactiveFrameAddr, Lab_Data, Pix_Data,Uart_Data);
		rt_exit_critical();
		tmp11 = cnt;

		LPUART_WriteBlocking(LPUART1, img_start, 2);
		LPUART_WriteBlocking(LPUART1, Uart_Data, 188*120);
		LPUART_WriteBlocking(LPUART1, img_end, 2);

//是否使用LCD
#if defined(USE_LCD) && (USE_LCD == 1U)
		for (int i = 0; i < 120; i++)
		{
			memcpy(s_frameBuffer[curLcdBufferIdx][i + 20] + 40, Pix_Data + i * 188, 188);
			memcpy(s_frameBuffer[curLcdBufferIdx][i + 20] + 300, Lab_Data + i * 188, 188);
			memcpy(s_frameBuffer[curLcdBufferIdx][i + 20] + 550, (uint8_t *)inactiveFrameAddr + i * 188, 188);
		}
		//memcpy(s_frameBuffer[!curLcdBufferIdx],s_frameBuffer[curLcdBufferIdx],38400);
		/* 等待直至中断完成 */
		ELCDIF_SetNextBufferAddr(LCDIF, (uint32_t)s_frameBuffer[curLcdBufferIdx]);
		s_frameDone = false;
		//wallner_new((uint8_t *)inactiveFrameAddr, Lab_Data);
		//wallner_new((uint8_t *)inactiveFrameAddr, Pix_Data);
		//bwlabel(Pix_Data, 8, Lab_Data);
		while (!s_frameDone)
		{
		}
		sprintf(dispBuff, "FPS = %02d MID_NUM = %03d tmp_img= %03d EFF=%d", FPS, mid_NUM, temp_img,Effective_line);
		sprintf(dispBuff1, "Runtime = %d HD=%d", tmp11,HD_first_flag);
		sprintf(dispBuff2, "L_corss = %d R_corss = %d L_UP=%d R_UP=%d", temp_img1,temp_img2,temp_img3,temp_img4);
		// sprintf(dispBuff3, "L_L=%03d L_R=%03d L_U=%03d %03d L_D = %03d %03d",tmp_BP[0][0][0],tmp_BP[0][0][1],tmp_BP[0][0][2],tmp_BP[0][1][2],tmp_BP[0][0][3],tmp_BP[0][1][3]);
		// sprintf(dispBuff4, "R_L=%03d R_R=%03d %03d R_U=%03d %03d R_D = %03d %03d",tmp_BP[1][0][0],tmp_BP[1][0][1],tmp_BP[1][1][1],tmp_BP[1][0][2],tmp_BP[1][1][2],tmp_BP[1][0][3],tmp_BP[1][1][3]);
		// sprintf(dispBuff5, "L_L=%03d L_R=%03d L_U=%03d %03d L_D = %03d %03d",tmp_BP[2][0][0],tmp_BP[2][0][1],tmp_BP[2][0][2],tmp_BP[2][1][2],tmp_BP[2][0][3],tmp_BP[2][1][3]);
		// sprintf(dispBuff6, "R_L=%03d R_R=%03d R_U=%03d %03d R_D = %03d %03d",tmp_BP[3][0][0],tmp_BP[3][0][1],tmp_BP[3][0][2],tmp_BP[3][1][2],tmp_BP[3][0][3],tmp_BP[3][1][3]);
		//sprintf(dispBuff7, "HD = %d ", HD_first_flag);
		LCD_ClearLine(LINE(6));
		LCD_ClearLine(LINE(7));
		// LCD_ClearLine(LINE(8));
		// LCD_ClearLine(LINE(9));
		// LCD_ClearLine(LINE(10));
		// LCD_ClearLine(LINE(11));
		LCD_ClearLine(LINE(14));
		//		/*然后显示该字符串即可，其它变量也是这样处理*/
		LCD_DisplayStringLine(LINE(6), (uint8_t *)dispBuff);
		LCD_DisplayStringLine(LINE(14), (uint8_t *)dispBuff1);
		LCD_DisplayStringLine(LINE(7), (uint8_t *)dispBuff2);
		// LCD_DisplayStringLine(LINE(8), (uint8_t *)dispBuff3);
		// LCD_DisplayStringLine(LINE(9), (uint8_t *)dispBuff4);
		// LCD_DisplayStringLine(LINE(10), (uint8_t *)dispBuff5);
		// LCD_DisplayStringLine(LINE(11), (uint8_t *)dispBuff6);
#endif
		CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, inactiveFrameAddr);
	}
}
