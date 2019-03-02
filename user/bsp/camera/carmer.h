#ifndef __CARMER_H
#define __CARMER_H
#include "fsl_common.h"
#include "fsl_csi.h"
#include "fsl_csi_camera_adapter.h"
#include "fsl_camera.h"
#include "fsl_camera_receiver.h"
#include "fsl_camera_device.h"
#include "LQ_MT9V034M.h"

#define FRAME_BUFFER_ALIGN 64
#define CAMERA_WIDTH  IMAGEW
#define CAMERA_HEIGHT IMAGEH
#define CAMERA_BPP					(1)		// Bytes Per Pixel
#define CAMERA_SIZE					(CAMERA_WIDTH*CAMERA_HEIGHT*CAMERA_BPP)
//#define CAMERA_CONTROL_FLAGS 		(kCAMERA_HrefActiveHigh|kCAMERA_DataLatchOnRisingEdge)kCAMERA_DataLatchOnFallingEdge,kCAMERA_DataLatchOnRisingEdge
#define CAMERA_CONTROL_FLAGS 		(kCAMERA_VsyncActiveHigh|kCAMERA_HrefActiveHigh|kCAMERA_DataLatchOnRisingEdge)
#define CAMERA_FRAME_BUFFER_COUNT	(4)

void CAMCSI_Init(void);
void Campin_Init(void);
void test(void);
void CAM_DIS(void);
void testSend(void);

//AT_NONCACHEABLE_SECTION_ALIGN(static uint16_t s_frameBuffer[4][CAMERA_HEIGHT*CAMERA_WIDTH*CAMERA_BPP/sizeof(uint16_t)],
//                              FRAME_BUFFER_ALIGN);CAMERA_HEIGHT 
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t csiFrameBuf[CAMERA_FRAME_BUFFER_COUNT][CAMERA_HEIGHT*CAMERA_WIDTH],FRAME_BUFFER_ALIGN); //定义摄像头数据缓存区

#endif /* __CARMER_H */
