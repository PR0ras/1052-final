/*******************************************************************************
【平    台】龙邱K66FX智能车VD母板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2017年12月22日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】IAR7.80.4及以上
【Target  】K66FX1M0VLQ18
【Crystal 】 50.000Mhz
【busclock】180.000MHz
【pllclock】180.000MHz
******************************************************************************/
#ifndef __LQMT9V034M_H__
#define __LQMT9V034M_H__

#include "fsl_common.h"
#include "fsl_sccb.h"
#include "fsl_camera_device.h"

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【函数功能】龙邱MT9V034模块分辨率大小设置
【作  者】CHIUSIR
【参数值】
【软件版本】V1.0
【最后更新】2018年1月8日
【函数名】void LQMT9V034M_Set_Resolution(uint16_t width,uint16_t height)
【返回值】无
【参数值】
uint16_t width，自定义摄像头采集数据的横向列数
uint16_t height，自定义摄像头采集数据的纵向行数
【实例】 LQMT9V034M_Set_Resolution(320,240);//自定义图像大小为320*240
【提示】 重新定义摄像头分辨率后，需要重新定义每行采集像素的数量为width，行数为height。                                             
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

//#define IMAGEH  480 //行 HEIGHT 待采集摄像头图像高度行数
#define IMAGEW  376  //列 WIDTH  待采集摄像头图像宽度列数
#define IMAGEH  60 //行 HEIGHT 待采集摄像头图像高度行数   
//#define IMAGEW  752  //列 WIDTH  待采集摄像头图像宽度列数
typedef struct _LQMT9V034_resource
{
    sccb_i2c_t sccbI2C;                     //!< I2C for SCCB transfer. 
    void (*pullResetPin)(bool pullUp);     /*!< Function to pull reset pin high or low. */
    void (*pullPowerDownPin)(bool pullUp); /*!< Function to pull the power down pin high or low. */
    uint32_t inputClockFreq_Hz;             //!< Input clock frequency. 
} LQMT9V034_resource_t;

void delayms(uint32_t ms);
//LQMT9V034 operation functions. 
extern const camera_device_operations_t LQMT9V034_ops;
unsigned char MTV_IICWriteReg16(unsigned char address, uint16_t Data);
unsigned char MTV_IICReadReg16(unsigned char address, uint16_t *Data);
status_t  LQMT9V034_Init(camera_device_handle_t *handle, const camera_config_t *config);
extern void MT9V034_Reset(void);
extern void MT9V034_SetReservedReg(void);
extern void MT9V034_SetFrameRate(uint8_t frameRate);
void MT9V034_SetFrameResolution(uint16_t height,uint16_t width, uint8_t fps);
void imgremote(uint8_t address,uint16_t data);
extern void MT9V034_SetAutoExposure(bool enable);
void LPI2C1_Init(uint32_t baudrate);
void MT9V034_daily(void);
void MT9V034_night(void);

#define BINNING_ROW_A	        4
#define BINNING_COLUMN_A	4
#define BINNING_ROW_B	        2
#define BINNING_COLUMN_B	2
#define MINIMUM_HORIZONTAL_BLANKING	91 // see datasheet
#define MAX_IMAGE_HEIGHT	480
#define MAX_IMAGE_WIDTH	        752
#define MINIMUM_COLUMN_START	1
#define MINIMUM_ROW_START	4

/* Camera I2C registers */
#define MTV_I2C_ADDR                0x5C
#define MTV_DEVICE_WRITE_ADDRESS    0xB8
#define MTV_DEVICE_READ_ADDRESS     0xB9

#define MT9V034_I2C_ADDR	            0xB8 //(0xB8 >> 1)=0x5C
#define MT9V034_PIXEL_ARRAY_HEIGHT	            492
#define MT9V034_PIXEL_ARRAY_WIDTH	            782
#define MT9V034_CHIP_VERSION			    0x00
#define MT9V034_CHIP_ID	                    0x1324

#define MT9V034_COLUMN_START	                    0x01
#define MT9V034_COLUMN_START_MIN	            1
#define MT9V034_COLUMN_START_DEF	            1
#define MT9V034_COLUMN_START_MAX	            752

#define MT9V034_ROW_START	                    0x02
#define MT9V034_ROW_START_MIN	            4
#define MT9V034_ROW_START_DEF	            4
#define MT9V034_ROW_START_MAX	            482

#define MT9V034_WINDOW_HEIGHT	            0x03
#define MT9V034_WINDOW_HEIGHT_MIN	            1
#define MT9V034_WINDOW_HEIGHT_DEF	            64
#define MT9V034_WINDOW_HEIGHT_MAX	            480

#define MT9V034_WINDOW_WIDTH	                    0x04
#define MT9V034_WINDOW_WIDTH_MIN	            1
#define MT9V034_WINDOW_WIDTH_DEF	            64
#define MT9V034_WINDOW_WIDTH_MAX	            752

#define MINIMUM_HORIZONTAL_BLANKING	    91 // see datasheet

#define MT9V034_HORIZONTAL_BLANKING	            0x05
#define MT9V034_HORIZONTAL_BLANKING_MIN	    43
#define MT9V034_HORIZONTAL_BLANKING_MAX	    1023

#define MT9V034_VERTICAL_BLANKING	            0x06
#define MT9V034_VERTICAL_BLANKING_MIN	    4
#define MT9V034_VERTICAL_BLANKING_MAX	    3000

#define MT9V034_CHIP_CONTROL	                    0x07
#define MT9V034_CHIP_CONTROL_MASTER_MODE         (1 << 3)
#define MT9V034_CHIP_CONTROL_DOUT_ENABLE         (1 << 7)
#define MT9V034_CHIP_CONTROL_SEQUENTIAL	    (1 << 8)

#define MT9V034_SHUTTER_WIDTH1	            0x08
#define MT9V034_SHUTTER_WIDTH2	            0x09
#define MT9V034_SHUTTER_WIDTH_CONTROL	    0x0A
#define MT9V034_TOTAL_SHUTTER_WIDTH	    0x0B
#define MT9V034_TOTAL_SHUTTER_WIDTH_MIN	    1
#define MT9V034_TOTAL_SHUTTER_WIDTH_DEF	    480
#define MT9V034_TOTAL_SHUTTER_WIDTH_MAX	    32767

#define MT9V034_RESET	                    0x0C

#define MT9V034_READ_MODE	                    0x0D
#define MT9V034_READ_MODE_ROW_BIN_MASK	    (3 << 0)
#define MT9V034_READ_MODE_ROW_BIN_SHIFT	    0
#define MT9V034_READ_MODE_COLUMN_BIN_MASK        (3 << 2)
#define MT9V034_READ_MODE_COLUMN_BIN_SHIFT       2
#define MT9V034_READ_MODE_ROW_BIN_2         (1<<0)
#define MT9V034_READ_MODE_ROW_BIN_4         (1<<1)
#define MT9V034_READ_MODE_COL_BIN_2         (1<<2)
#define MT9V034_READ_MODE_COL_BIN_4         (1<<3)
#define MT9V034_READ_MODE_ROW_FLIP	            (1 << 4)
#define MT9V034_READ_MODE_COLUMN_FLIP	    (1 << 5)
#define MT9V034_READ_MODE_DARK_COLUMNS	    (1 << 6)
#define MT9V034_READ_MODE_DARK_ROWS	            (1 << 7)

#define MT9V034_PIXEL_OPERATION_MODE	            0x0F
#define MT9V034_PIXEL_OPERATION_MODE_COLOR       (1 << 2)
#define MT9V034_PIXEL_OPERATION_MODE_HDR         (1 << 6)

#define MT9V034_V1_CTRL_REG_A	        0x31
#define MT9V034_V2_CTRL_REG_A	        0x32
#define MT9V034_V3_CTRL_REG_A	        0x33
#define MT9V034_V4_CTRL_REG_A	        0x34

#define MT9V034_ANALOG_GAIN	                    0x35
#define MT9V034_ANALOG_GAIN_MIN	            16
#define MT9V034_ANALOG_GAIN_DEF	            16
#define MT9V034_ANALOG_GAIN_MAX	            64

#define MT9V034_MAX_ANALOG_GAIN	            0x36
#define MT9V034_MAX_ANALOG_GAIN_MAX	            127

#define MT9V034_FRAME_DARK_AVERAGE	            0x42
#define MT9V034_DARK_AVG_THRESH	            0x46
#define MT9V034_DARK_AVG_LOW_THRESH_MASK         (255 << 0)
#define MT9V034_DARK_AVG_LOW_THRESH_SHIFT        0
#define MT9V034_DARK_AVG_HIGH_THRESH_MASK	    (255 << 8)
#define MT9V034_DARK_AVG_HIGH_THRESH_SHIFT	    8

#define MT9V034_ROW_NOISE_CORR_CONTROL	    0x70
#define MT9V034_ROW_NOISE_CORR_ENABLE	    (1 << 5)
#define MT9V034_ROW_NOISE_CORR_USE_BLK_AVG	    (1 << 7)

#define MT9V034_PIXEL_CLOCK		            0x74
#define MT9V034_PIXEL_CLOCK_INV_LINE		    (1 << 0)
#define MT9V034_PIXEL_CLOCK_INV_FRAME	    (1 << 1)
#define MT9V034_PIXEL_CLOCK_XOR_LINE		    (1 << 2)
#define MT9V034_PIXEL_CLOCK_CONT_LINE	    (1 << 3)
#define MT9V034_PIXEL_CLOCK_INV_PXL_CLK	    (1 << 4)

#define MT9V034_TEST_PATTERN		            0x7f
#define MT9V034_TEST_PATTERN_DATA_MASK	    (1023 << 0)
#define MT9V034_TEST_PATTERN_DATA_SHIFT	    0
#define MT9V034_TEST_PATTERN_USE_DATA	    (1 << 10)
#define MT9V034_TEST_PATTERN_GRAY_MASK	    (3 << 11)
#define MT9V034_TEST_PATTERN_GRAY_NONE	    (0 << 11)
#define MT9V034_TEST_PATTERN_GRAY_VERTICAL	    (1 << 11)
#define MT9V034_TEST_PATTERN_GRAY_HORIZONTAL	    (2 << 11)
#define MT9V034_TEST_PATTERN_GRAY_DIAGONAL	    (3 << 11)
#define MT9V034_TEST_PATTERN_ENABLE		    (1 << 13)
#define MT9V034_TEST_PATTERN_FLIP		    (1 << 14)

#define MT9V034_AEC_AGC_ENABLE		    0xAF
#define MT9V034_AEC_ENABLE		            (1 << 0)
#define MT9V034_AGC_ENABLE		            (1 << 1)
#define MT9V034_THERMAL_INFO		            0xc1
#define MT9V034_ANALOG_CTRL                     (0xC2)
#define MT9V034_ANTI_ECLIPSE_ENABLE                 (1<<7)
#define MT9V034_MAX_GAIN                        (0xAB)
#define MT9V034_FINE_SHUTTER_WIDTH_TOTAL_A      (0xD5)
#define MT9V034_HDR_ENABLE_REG	        0x0F
#define MT9V034_ADC_RES_CTRL_REG	        0x1C
#define MT9V034_ROW_NOISE_CORR_CTRL_REG	0x70
#define MT9V034_TEST_PATTERN_REG       	0x7F
#define MT9V034_TILED_DIGITAL_GAIN_REG	0x80
#define MT9V034_AGC_AEC_DESIRED_BIN_REG	0xA5
#define MT9V034_MAX_GAIN_REG        	0xAB
#define MT9V034_MIN_EXPOSURE_REG       	0xAC  // datasheet min coarse shutter width
#define MT9V034_MAX_EXPOSURE_REG       	0xAD  // datasheet max coarse shutter width
#define MT9V034_AEC_AGC_ENABLE_REG	0xAF
#define MT9V034_AGC_AEC_PIXEL_COUNT_REG	0xB0

#endif // __MT9V034_H__
