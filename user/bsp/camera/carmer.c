#include "carmer.h"
//#include "oled2.h"
#include "./lcd/bsp_lcd.h"
#include "./font/fonts.h"
#include "bwLabel.h"
#include "bsp_pit.h"
#include "1052_NVIC.h"

#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "fsl_pxp.h"
#include "fsl_elcdif.h"

uint8_t Pix_Data[22560] = {0};
uint8_t Lab_Data[22560] = {0};
//uint8_t Buffer_Data[600]={0};
//uint8_t cmos[60][80]={0,0};
//uint8_t oledcmos[60][80]={0};

uint8_t camover = 0, FPStmp = 0, FPS = 0;
uint32_t activeFrameAddr = (uint32_t)&csiFrameBuf[0];
uint32_t inactiveFrameAddr = (uint32_t)&csiFrameBuf[0];
uint8_t img_start[2] = {0xaa, 0x55};
uint8_t img_end[2] = {0x55, 0xaa};
char dispBuff[100];

extern pxp_ps_buffer_config_t psBufferConfig;
extern pxp_output_buffer_config_t outputBufferConfig;
extern volatile bool s_frameDone;
extern void MT9V034_SetFrameResolution(uint16_t height, uint16_t width, uint8_t fps);
extern uint32_t cnt;
extern bool txOnGoing;
extern lpuart_transfer_t sendXfer;
AT_NONCACHEABLE_SECTION_ALIGN( uint8_t uartimg[94][60], FRAME_BUFFER_ALIGN);

static csi_private_data_t csiPrivateData;

static csi_resource_t csiResource = {
	.csiBase = CSI,
};

camera_receiver_handle_t cameraReceiver = {
	.resource = &csiResource,
	.ops = &csi_ops,
	.privateData = &csiPrivateData,
};

static LQMT9V034_resource_t LQMT9V034Resource = {
	//摄像头初始化结构体
	.sccbI2C = LPI2C1,
	.inputClockFreq_Hz = 27000000,
};

camera_device_handle_t cameraDevice = {
	.resource = &LQMT9V034Resource,
	.ops = &LQMT9V034_ops,
};

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
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_12_CSI_PIXCLK, 0U);   /* GPIO_AD_B1_04 is configured as CSI_PIXCLK */
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_06_CSI_VSYNC, 0U); /* GPIO_AD_B1_06 is configured as CSI_VSYNC */
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_07_CSI_HSYNC, 0U);
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
	BOARD_InitCameraResource();
	memset(csiFrameBuf, 0, sizeof(csiFrameBuf));
	CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, NULL, NULL);
	//CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);
	MT9V034_SetFrameResolution(480, 752, 60);
	RT1052_NVIC_SetPriority(CSI_IRQn,3,0);
	for (uint32_t i = 0; i < 4; i++)
	{
		CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(csiFrameBuf[i]));
	}

	outputBufferConfig.buffer0Addr = (uint32_t)uartimg;
	PXP_SetOutputBufferConfig(PXP, &outputBufferConfig);
	//PRINTF("Camera Start\r\n");//摄像头启动成功
}

void CAM_DIS(void)
{
	
 	// sendXfer.data = (uint8_t *)uartimg;
    // sendXfer.dataSize = sizeof(uartimg);
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
	LCDIF->CUR_BUF=activeFrameAddr;
	ELCDIF_SetNextBufferAddr(LCDIF, inactiveFrameAddr);
	ELCDIF_RgbModeStart(LCDIF);
	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);
	CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, inactiveFrameAddr);
	PRINTF("CAMERA Start Success\r\n");
//	cnt=0;
//	wallner(csiFrameBuf[0], Pix_Data);
//	PRINTF("wallner Time:%d \r\n",cnt);
//	cnt=0;
//	wallner_new(csiFrameBuf[0], Lab_Data);
//	PRINTF("wallner_new Time:%d \r\n",cnt);
	cnt=0;
	edge_dect(csiFrameBuf[0]);
	// bwlabel(Pix_Data,8,Lab_Data);
//	//testSend();
	PRINTF("bwlabel Time :%d \r\n",cnt);
	
	while (1)
	{
		FPStmp++;
		curLcdBufferIdx ^= 1U;
		// Return the camera buffer to camera queue.
//		CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);
//		activeFrameAddr = inactiveFrameAddr;

		// Wait for the new set LCD frame buffer active.
		CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, activeFrameAddr);
		activeFrameAddr = inactiveFrameAddr;
		
		// psBufferConfig.bufferAddr = inactiveFrameAddr;    //设置PXP转换源地址
        // PXP_SetProcessSurfaceBufferConfig(PXP, &psBufferConfig); // 对PXP输入进行配置
		// PXP_Start(PXP);

//		for (int i = 0; i < 120; i++)
//		{
//			memcpy(s_frameBuffer[curLcdBufferIdx][i], (uint8_t *)inactiveFrameAddr + i * 188, 188);
//		}

		//		wallner((uint8_t *)inactiveFrameAddr,Pix_Data);
		//		testSend();

		while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &inactiveFrameAddr))
		{
		}
		
		sendXfer.data = (uint8_t *)inactiveFrameAddr;
		// edge_dect((uint8_t *)inactiveFrameAddr);
		// wallner((uint8_t *)inactiveFrameAddr, Pix_Data);
		edge_bw((uint8_t *)inactiveFrameAddr, Pix_Data);
		for (int i = 0; i < 120; i++)
		{
			memcpy(s_frameBuffer[curLcdBufferIdx][i+20]+40,Pix_Data + i * 188, 188);
			//memcpy(s_frameBuffer[curLcdBufferIdx][i+20]+300,Lab_Data + i * 188, 188);
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
		
		sprintf(dispBuff, "FPS = %d ", FPS);
		LCD_ClearLine(LINE(6));
//		/*然后显示该字符串即可，其它变量也是这样处理*/
		LCD_DisplayStringLine(LINE(6), (uint8_t *)dispBuff);

		// while (!(kPXP_CompleteFlag & PXP_GetStatusFlags(PXP)))  //等待pXP转换完成
        //     {
        //     }
        // PXP_ClearStatusFlags(PXP, kPXP_CompleteFlag);   //清除标志位
		// if(txOnGoing==false)
		// {	
		// 	txOnGoing = true;
		// 	LPUART_SendEDMA(LPUART1, &g_lpuartEdmaHandle, &xfer);
		// }
		
		
	}
}


