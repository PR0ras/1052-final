
/*******************************************************************************
【平    台】龙邱K66FX智能车VD母板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2017年12月22日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
******************************************************************************/
#include "LQ_MT9V034M.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
status_t LQMT9V034_Init(camera_device_handle_t *handle, const camera_config_t *config);
status_t LQMT9V034_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig);
status_t LQMT9V034_Deinit(camera_device_handle_t *handle);
status_t LQMT9V034_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg);
status_t LQMT9V034_Start(camera_device_handle_t *handle);
status_t LQMT9V034_Stop(camera_device_handle_t *handle);
/*******************************************************************************
 * Variables
 ******************************************************************************/
const camera_device_operations_t LQMT9V034_ops = {
    .init = LQMT9V034_Init,
    .deinit = LQMT9V034_Deinit,
    .start = LQMT9V034_Start,
    .stop = LQMT9V034_Stop,
    .control = LQMT9V034_Control,
    .init_ext = LQMT9V034_InitExt,
};
//I2C 写16位寄存器函数  address：要写寄存器   Data：要写的值
static unsigned char MTV_IICWriteReg16(unsigned char address, uint16_t Data) //I2C 写16位寄存器函数
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(Data >> 8);
    buf[1] = (uint8_t)(Data);
    unsigned char result = SCCB_WriteMultiRegs(LPI2C1, MTV_I2C_ADDR, kSCCB_RegAddr8Bit, address, buf, 2);
    return result;
}
//I2C 写16位寄存器函数 Device：器件地址  address：要写寄存器   Data：要写的值
static void SCCB_RegWrite(uint8_t Device, uint8_t Address, uint16_t Data)
{
    MTV_IICWriteReg16(Address, Data);
}
//I2C 读16位寄存器函数  address：要读寄存器   Data：读出值的缓冲区
static unsigned char MTV_IICReadReg16(unsigned char address, uint16_t *Data)
{
    uint8_t buf[2];
    unsigned char result = SCCB_ReadMultiRegs(LPI2C1, MTV_I2C_ADDR, kSCCB_RegAddr8Bit, address, buf, 2);
    *Data = ((buf[0] << 8) | buf[1]);
    return result;
}

status_t LQMT9V034_Deinit(camera_device_handle_t *handle)
{
    ((LQMT9V034_resource_t *)(handle->resource))->pullPowerDownPin(true);
    return kStatus_Success;
}

status_t LQMT9V034_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg)
{
    return kStatus_InvalidArgument;
}

status_t LQMT9V034_Start(camera_device_handle_t *handle)
{
    return kStatus_Success;
}

status_t LQMT9V034_Stop(camera_device_handle_t *handle)
{
    return kStatus_Success;
}

status_t LQMT9V034_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig)
{
    return LQMT9V034_Init(handle, config);
}
void LPI2C1_Init(uint32_t baudrate)
{
    uint32_t sourceClock = 0;

    //设置I2C的时钟源为USB1(PLL3)=480/8Mhz=60Mhz
    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0); //LPI2C时钟源为PLL3=60Mhz
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 5); //分频设置，分频值为5+1，所以LPI2C CLK=10Mhz

    /* LPI2C clock is OSC clock. */
    sourceClock = CLOCK_GetOscFreq();

    lpi2c_master_config_t lpi2c1_config; //配置结构体
    //配置I2C1作为I2C主机
    LPI2C_MasterGetDefaultConfig(&lpi2c1_config);          //先配置为默认配置
    lpi2c1_config.baudRate_Hz = baudrate;                  //设置I2C速率
    LPI2C_MasterInit(LPI2C1, &lpi2c1_config, sourceClock); //初始化I2C1
}
void MT9V034_Default_Settings(void);
// MT9V034 Port Init
status_t LQMT9V034_Init(camera_device_handle_t *handle, const camera_config_t *config)
{
    uint16_t data = 0;
    LPI2C1_Init(400000);                           //// I2C 初始化
    MTV_IICReadReg16(MT9V034_CHIP_VERSION, &data); //读取ID
                                                   //PRINTF("tmp: %x \r\n",tmp);
                                                   //PRINTF("kStatus_Success: %x \r\n",kStatus_Success);
                                                   //LCD_P6x8Str(100,1,"Ok");
                                                   //PRINTF("神眼初始化SUCCESS\r\n");
    if (data != MT9V034_CHIP_ID)                   //ID不正确
    {
        //LCD_P6x8Str(100, 1, "Fall");
        PRINTF("神眼初始化失败！: %x \r\n", data);
    }
    /*下面的和K66 K60例程一样，可以改变摄像头的输出图像大小，修改后CSI  和 PXP 都需要自行修改*/

    MT9V034_Default_Settings(); //最小曝光时间配置  注意神眼摄像头的寄存器掉电不会丢失，修改前最好记得备份当前配置
    /*下面的和K66 K60例程一样，可以改变摄像头的输出图像大小，修改后CSI  和 PXP 都需要自行修改*/
    MT9V034_SetFrameResolution(IMAGEH, IMAGEW, config->framePerSec); //设置摄像头图像分频输出,BIT4,5镜像设置:上下左右镜像

    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2C, 0x0004);                                     //参考电压设置   1.4v
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_ANALOG_CTRL, MT9V034_ANTI_ECLIPSE_ENABLE); //反向腐蚀

    //SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_MAX_GAIN_REG, 30); //0xAB  最大模拟增益     64

    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_AGC_AEC_PIXEL_COUNT_REG, 188 * 120); //0xB0  用于AEC/AGC直方图像素数目,最大44000   IMAGEH*IMAGEW
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_ADC_RES_CTRL_REG, 0x0303);           //0x1C  here is the way to regulate darkness :)
    ////
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x13, 0x2D2E); //We also recommended using R0x13 = 0x2D2E with this setting for better column FPN.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x20, 0x01c7); //0x01C7对比度差，发白；0x03C7对比度提高 Recommended by design to improve performance in HDR mode and when frame rate is low.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x24, 0x0010); //Corrects pixel negative dark offset when global reset in R0x20[9] is enabled.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2B, 0x0003); //Improves column FPN.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2F, 0x0003); //Improves FPN at near-saturation.

    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_SHUTTER_WIDTH_CONTROL, 0x0164); //0x0A Coarse Shutter IMAGEW Control
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_V2_CTRL_REG_A, 0x001A);         //0x32   0x001A
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_HDR_ENABLE_REG, 0x0100);        //0x0F High Dynamic Range enable,bit is set (R0x0F[1]=1), the sensor uses black level correction values from one green plane, which are applied to all colors.
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_AGC_AEC_DESIRED_BIN_REG, 40);   //0xA5  图像亮度  60  1-64
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_ANALOG_GAIN, 0x8010);
    //MT9V034_SetReservedReg();
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_RESET, 0x03); //0x0c  复位

    //    delayms(1000);

    return kStatus_Success;
}
void MT9V034_SetFrameResolution(uint16_t height, uint16_t width, uint8_t fps)
{
   // MTV_IICWriteReg16(0x08, 0x01BB);   //COARSE_SHUTTER_WIDTH_1_CONTEXTA 0x01BB
   // MTV_IICWriteReg16(0x09, 0x01D9);   //COARSE_SHUTTER_WIDTH_2_CONTEXTA 0x01D9
    MTV_IICWriteReg16(0x0A, 0x0164);   //SHUTTER_WIDTH_CONTROL_CONTEXTA 0x0164
    MTV_IICWriteReg16(0x0B, 0x01E0);   //COARSE_SHUTTER_WIDTH_TOTAL_CONTEXTA 0x0000
    MTV_IICWriteReg16(0x31, 0x0030);   //V1_CONTROL_CONTEXTA 0x0027
    MTV_IICWriteReg16(0x32, 0x001A);   //V2_CONTROL_CONTEXTA 0x001A
    MTV_IICWriteReg16(0x33, 0x0010);   //V3_CONTROL_CONTEXTA 0x0005
    MTV_IICWriteReg16(0x34, 0x0003);   //V4_CONTROL_CONTEXTA 0x0003

    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x13, 0x2D2E); //We also recommended using R0x13 = 0x2D2E with this setting for better column FPN.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x20, 0x01C7); //0x01C7对比度差，发白；0x03C7对比度提高 Recommended by design to improve performance in HDR mode and when frame rate is low.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x24, 0x001B); //Corrects pixel negative dark offset when global reset in R0x20[9] is enabled.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2B, 0x0003); //Improves column FPN.
    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2F, 0x0003); //Improves FPN at near-saturation.

    SCCB_RegWrite(MT9V034_I2C_ADDR, 0x2C, 0x0004);                                     //参考电压设置   1.4v
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_ANALOG_CTRL, MT9V034_ANTI_ECLIPSE_ENABLE); //反向腐蚀
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_MAX_GAIN_REG, 40); //0xAB  最大模拟增益     64
    //SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_AGC_AEC_PIXEL_COUNT_REG, 188 * 120); //0xB0  用于AEC/AGC直方图像素数目,最大44000   IMAGEH*IMAGEW
    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_ADC_RES_CTRL_REG, 0x0303);           //0x1C  here is the way to regulate darkness :)

    SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_HDR_ENABLE_REG, 0x0101);

    MTV_IICWriteReg16(MT9V034_READ_MODE, 0x003A); //写寄存器，配置行分频
    MTV_IICWriteReg16(MT9V034_WINDOW_WIDTH, width);   //读取图像的列数  改变此处也可改变图像输出大小，不过会丢失视角
    MTV_IICWriteReg16(MT9V034_WINDOW_HEIGHT, height); //读取图像的行数  改变此处也可改变图像输出大小，不过会丢失视角
    MTV_IICWriteReg16(MT9V034_COLUMN_START, MT9V034_COLUMN_START_MIN); //列开始
    MTV_IICWriteReg16(MT9V034_ROW_START, MT9V034_ROW_START_MIN);       //行开始
    MTV_IICWriteReg16(MT9V034_AEC_AGC_ENABLE, 0x0000);                 //AEC

    //MTV_IICWriteReg16(MT9V034_MAX_GAIN_REG, 50);
    //SCCB_RegWrite(MT9V034_I2C_ADDR, MT9V034_AGC_AEC_DESIRED_BIN_REG, 40); //0xA5  图像亮度  60  1-64
    //SCCB_RegWrite(MT9V034_I2C_ADDR,MT9V034_ANALOG_GAIN,0x8010); 
    
}

static void MT9V034_SetAutoExposure(bool enable)
{
    uint16_t reg = 0;
    MTV_IICReadReg16(MT9V034_AEC_AGC_ENABLE, &reg);
    if (true == enable)
    {
        MTV_IICWriteReg16(MT9V034_AEC_AGC_ENABLE, reg | MT9V034_AEC_ENABLE | MT9V034_AGC_ENABLE);
    }
    else
    {
        MTV_IICWriteReg16(MT9V034_AEC_AGC_ENABLE, reg & ~(MT9V034_AEC_ENABLE | MT9V034_AGC_ENABLE));
    }
}
void MT9V034_SetFrameRate(uint8_t frameRate)
{
}

static void MT9V034_Reset(void)
{
    //Reset MT9V034, but register config will not change.
    MTV_IICWriteReg16(MT9V034_RESET, 0x0001);
    delayms(10);

    //Unlock MT9V034, allowing user to initiate register settings and readout
    MTV_IICWriteReg16(MT9V034_CHIP_CONTROL, 0x0188);

    //Reset Again.
    MTV_IICWriteReg16(MT9V034_RESET, 0x0001);
    delayms(10);
}

void MT9V034_SetReservedReg(void)
{
    //Here we write some reserved registers as recommendations from Rev.G datasheet, Table.8
    MTV_IICWriteReg16(0x13, 0x2D2E);
    MTV_IICWriteReg16(0x20, 0x03C7);
    MTV_IICWriteReg16(0x24, 0x001B);
    MTV_IICWriteReg16(0x2B, 0x0003);
    MTV_IICWriteReg16(0x2F, 0x0003);
}

void delayms(uint32_t ms)
{
    uint32_t i, j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 200000; j++) //528M--1ms
            __NOP();                 //asm("NOP");
    }
    return;
}

void MT9V034_Default_Settings(void) //神眼摄像头默认配置
{
    MTV_IICWriteReg16(0x01, 0x0001); //COL_WINDOW_START_CONTEXTA_REG
    MTV_IICWriteReg16(0x02, 0x0004); //ROW_WINDOW_START_CONTEXTA_REG
    MTV_IICWriteReg16(0x03, 0x01E0); //ROW_WINDOW_SIZE_CONTEXTA_REG
    MTV_IICWriteReg16(0x04, 0x02F0); //COL_WINDOW_SIZE_CONTEXTA_REG
    MTV_IICWriteReg16(0x05, 0x005E); //HORZ_BLANK_CONTEXTA_REG
    MTV_IICWriteReg16(0x06, 0x002D); //VERT_BLANK_CONTEXTA_REG
    MTV_IICWriteReg16(0x07, 0x0188); //CONTROL_MODE_REG
    MTV_IICWriteReg16(0x08, 0x01BB); //COARSE_SHUTTER_WIDTH_1_CONTEXTA
    MTV_IICWriteReg16(0x09, 0x01D9); //COARSE_SHUTTER_WIDTH_2_CONTEXTA
    MTV_IICWriteReg16(0x0A, 0x0164); //SHUTTER_WIDTH_CONTROL_CONTEXTA
    MTV_IICWriteReg16(0x0B, 0x0000); //COARSE_SHUTTER_WIDTH_TOTAL_CONTEXTA
    MTV_IICWriteReg16(0x0C, 0x0000); //RESET_REG
    MTV_IICWriteReg16(0x0D, 0x0300); //READ_MODE_REG
    MTV_IICWriteReg16(0x0E, 0x0000); //READ_MODE2_REG
    MTV_IICWriteReg16(0x0F, 0x0000); //PIXEL_OPERATION_MODE
    MTV_IICWriteReg16(0x10, 0x0040); //RAMP_START_DELAY
    MTV_IICWriteReg16(0x11, 0x8042); //OFFSET_CONTROL
    MTV_IICWriteReg16(0x12, 0x0022); //AMP_RESET_BAR_CONTROL
    MTV_IICWriteReg16(0x13, 0x2D2E); //5T_PIXEL_RESET_CONTROL
    MTV_IICWriteReg16(0x14, 0x0E02); //4T_PIXEL_RESET_CONTROL
    MTV_IICWriteReg16(0x15, 0x0E32); //TX_CONTROL
    MTV_IICWriteReg16(0x16, 0x2802); //5T_PIXEL_SHS_CONTROL
    MTV_IICWriteReg16(0x17, 0x3E38); //4T_PIXEL_SHS_CONTROL
    MTV_IICWriteReg16(0x18, 0x3E38); //5T_PIXEL_SHR_CONTROL
    MTV_IICWriteReg16(0x19, 0x2802); //4T_PIXEL_SHR_CONTROL
    MTV_IICWriteReg16(0x1A, 0x0428); //COMPARATOR_RESET_CONTROL
    MTV_IICWriteReg16(0x1B, 0x0000); //LED_OUT_CONTROL
    MTV_IICWriteReg16(0x1C, 0x0302); //DATA_COMPRESSION
    MTV_IICWriteReg16(0x1D, 0x0040); //ANALOG_TEST_CONTROL
    MTV_IICWriteReg16(0x1E, 0x0000); //SRAM_TEST_DATA_ODD
    MTV_IICWriteReg16(0x1F, 0x0000); //SRAM_TEST_DATA_EVEN
    MTV_IICWriteReg16(0x20, 0x03C7); //BOOST_ROW_EN
    MTV_IICWriteReg16(0x21, 0x0020); //I_VLN_CONTROL
    MTV_IICWriteReg16(0x22, 0x0020); //I_VLN_AMP_CONTROL
    MTV_IICWriteReg16(0x23, 0x0010); //I_VLN_CMP_CONTROL
    MTV_IICWriteReg16(0x24, 0x001B); //I_OFFSET_CONTROL
                                     //    MTV_IICWriteReg1G=0x25, 0x0000); // I_BANDGAP_CONTROL - TRIMMED PER DIE
    MTV_IICWriteReg16(0x26, 0x0004); //I_VLN_VREF_ADC_CONTROL
    MTV_IICWriteReg16(0x27, 0x000C); //I_VLN_STEP_CONTROL
    MTV_IICWriteReg16(0x28, 0x0010); //I_VLN_BUF_CONTROL
    MTV_IICWriteReg16(0x29, 0x0010); //I_MASTER_CONTROL
    MTV_IICWriteReg16(0x2A, 0x0020); //I_VLN_AMP_60MHZ_CONTROL
    MTV_IICWriteReg16(0x2B, 0x0004); //VREF_AMP_CONTROL
    MTV_IICWriteReg16(0x2C, 0x0004); //VREF_ADC_CONTROL
    MTV_IICWriteReg16(0x2D, 0x0004); //VBOOST_CONTROL
    MTV_IICWriteReg16(0x2E, 0x0007); //V_HI_CONTROL
    MTV_IICWriteReg16(0x2F, 0x0003); //V_LO_CONTROL
    MTV_IICWriteReg16(0x30, 0x0003); //V_AMP_CAS_CONTROL
    MTV_IICWriteReg16(0x31, 0x0027); //V1_CONTROL_CONTEXTA
    MTV_IICWriteReg16(0x32, 0x001A); //V2_CONTROL_CONTEXTA
    MTV_IICWriteReg16(0x33, 0x0005); //V3_CONTROL_CONTEXTA
    MTV_IICWriteReg16(0x34, 0x0003); //V4_CONTROL_CONTEXTA
    MTV_IICWriteReg16(0x35, 0x0010); //GLOBAL_GAIN_CONTEXTA_REG
    MTV_IICWriteReg16(0x36, 0x8010); //GLOBAL_GAIN_CONTEXTB_REG
    MTV_IICWriteReg16(0x37, 0x0000); //VOLTAGE_CONTROL
    MTV_IICWriteReg16(0x38, 0x0000); //IDAC_VOLTAGE_MONITOR
    MTV_IICWriteReg16(0x39, 0x0027); //V1_CONTROL_CONTEXTB
    MTV_IICWriteReg16(0x3A, 0x0026); //V2_CONTROL_CONTEXTB
    MTV_IICWriteReg16(0x3B, 0x0005); //V3_CONTROL_CONTEXTB
    MTV_IICWriteReg16(0x3C, 0x0003); //V4_CONTROL_CONTEXTB
    MTV_IICWriteReg16(0x40, 0x0080); //DARK_AVG_THRESHOLDS
    MTV_IICWriteReg16(0x46, 0x231D); //CALIB_CONTROL_REG (AUTO)
    MTV_IICWriteReg16(0x47, 0x0080); //STEP_SIZE_AVG_MODE
    MTV_IICWriteReg16(0x48, 0x0020); //ROW_NOISE_CONTROL
    MTV_IICWriteReg16(0x4C, 0x0002); //NOISE_CONSTANT
    MTV_IICWriteReg16(0x60, 0x0000); //PIXCLK_CONTROL
    MTV_IICWriteReg16(0x67, 0x0000); //TEST_DATA
    MTV_IICWriteReg16(0x6C, 0x0000); //TILE_X0_Y0
    MTV_IICWriteReg16(0x70, 0x0000); //TILE_X1_Y0
    MTV_IICWriteReg16(0x71, 0x002A); //TILE_X2_Y0
    MTV_IICWriteReg16(0x72, 0x0000); //TILE_X3_Y0
    MTV_IICWriteReg16(0x7F, 0x0000); //TILE_X4_Y0
    MTV_IICWriteReg16(0x99, 0x0000); //TILE_X0_Y1
    MTV_IICWriteReg16(0x9A, 0x0096); //TILE_X1_Y1
    MTV_IICWriteReg16(0x9B, 0x012C); //TILE_X2_Y1
    MTV_IICWriteReg16(0x9C, 0x01C2); //TILE_X3_Y1
    MTV_IICWriteReg16(0x9D, 0x0258); //TILE_X4_Y1
    MTV_IICWriteReg16(0x9E, 0x02F0); //TILE_X0_Y2
    MTV_IICWriteReg16(0x9F, 0x0000); //TILE_X1_Y2
    MTV_IICWriteReg16(0xA0, 0x0060); //TILE_X2_Y2
    MTV_IICWriteReg16(0xA1, 0x00C0); //TILE_X3_Y2
    MTV_IICWriteReg16(0xA2, 0x0120); //TILE_X4_Y2
    MTV_IICWriteReg16(0xA3, 0x0180); //TILE_X0_Y3
    MTV_IICWriteReg16(0xA4, 0x01E0); //TILE_X1_Y3
    MTV_IICWriteReg16(0xA5, 0x003A); //TILE_X2_Y3
    MTV_IICWriteReg16(0xA6, 0x0002); //TILE_X3_Y3
    MTV_IICWriteReg16(0xA8, 0x0000); //TILE_X4_Y3
    MTV_IICWriteReg16(0xA9, 0x0002); //TILE_X0_Y4
    MTV_IICWriteReg16(0xAA, 0x0002); //TILE_X1_Y4
    MTV_IICWriteReg16(0xAB, 0x0040); //TILE_X2_Y4
    MTV_IICWriteReg16(0xAC, 0x0001); //TILE_X3_Y4
    MTV_IICWriteReg16(0xAD, 0x01E0); //TILE_X4_Y4
    MTV_IICWriteReg16(0xAE, 0x0014); //X0_SLASH5
    MTV_IICWriteReg16(0xAF, 0x0000); //X1_SLASH5
    MTV_IICWriteReg16(0xB0, 0xABE0); //X2_SLASH5
    MTV_IICWriteReg16(0xB1, 0x0002); //X3_SLASH5
    MTV_IICWriteReg16(0xB2, 0x0010); //X4_SLASH5
    MTV_IICWriteReg16(0xB3, 0x0010); //X5_SLASH5
    MTV_IICWriteReg16(0xB4, 0x0000); //Y0_SLASH5
    MTV_IICWriteReg16(0xB5, 0x0000); //Y1_SLASH5
    MTV_IICWriteReg16(0xB6, 0x0000); //Y2_SLASH5
    MTV_IICWriteReg16(0xB7, 0x0000); //Y3_SLASH5
    MTV_IICWriteReg16(0xBF, 0x0016); //Y4_SLASH5
    MTV_IICWriteReg16(0xC0, 0x000A); //Y5_SLASH5
    MTV_IICWriteReg16(0xC2, 0x18D0); //DESIRED_BIN
    MTV_IICWriteReg16(0xC3, 0x007F); //EXP_SKIP_FRM_H
    MTV_IICWriteReg16(0xC4, 0x007F); //EXP_LPF
    MTV_IICWriteReg16(0xC5, 0x007F); //GAIN_SKIP_FRM
    MTV_IICWriteReg16(0xC6, 0x0000); //GAIN_LPF_H
    MTV_IICWriteReg16(0xC7, 0x4416); //MAX_GAIN
    MTV_IICWriteReg16(0xC8, 0x4421); //MIN_COARSE_EXPOSURE
    MTV_IICWriteReg16(0xC9, 0x0001); //MAX_COARSE_EXPOSURE
    MTV_IICWriteReg16(0xCA, 0x0004); //BIN_DIFF_THRESHOLD
    MTV_IICWriteReg16(0xCB, 0x01E0); //AUTO_BLOCK_CONTROL
    MTV_IICWriteReg16(0xCC, 0x02F0); //PIXEL_COUNT
    MTV_IICWriteReg16(0xCD, 0x005E); //LVDS_MASTER_CONTROL
    MTV_IICWriteReg16(0xCE, 0x002D); //LVDS_SHFT_CLK_CONTROL
    MTV_IICWriteReg16(0xCF, 0x01DE); //LVDS_DATA_CONTROL
    MTV_IICWriteReg16(0xD0, 0x01DF); //LVDS_DATA_STREAM_LATENCY
    MTV_IICWriteReg16(0xD1, 0x0164); //LVDS_INTERNAL_SYNC
    MTV_IICWriteReg16(0xD2, 0x0001); //LVDS_USE_10BIT_PIXELS
    MTV_IICWriteReg16(0xD3, 0x0000); //STEREO_ERROR_CONTROL
    MTV_IICWriteReg16(0xD4, 0x0000); //INTERLACE_FIELD_VBLANK
    MTV_IICWriteReg16(0xD5, 0x0104); //IMAGE_CAPTURE_NUM
    MTV_IICWriteReg16(0xD6, 0x0000); //ANALOG_CONTROLS
    MTV_IICWriteReg16(0xD7, 0x0000); //AB_PULSE_WIDTH_REG
    MTV_IICWriteReg16(0xD8, 0x0000); //TX_PULLUP_PULSE_WIDTH_REG
    MTV_IICWriteReg16(0xD9, 0x0000); //RST_PULLUP_PULSE_WIDTH_REG
    MTV_IICWriteReg16(0xF0, 0x0000); //NTSC_FV_CONTROL
    MTV_IICWriteReg16(0xFE, 0xBEEF); //NTSC_HBLANK
}
 void imgremote(uint8_t address,uint16_t data)
 {
    MTV_IICWriteReg16(address, data);
    PRINTF("%x %x\r\n", address,data);
	//PRINTF("camdata: %x\r\n", data);
 }