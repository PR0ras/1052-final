#include "bsp_lpuart.h"
#include "fsl_lpuart.h"
#include "bsp_Timer.h"
#include "bsp_FlexPwm.h"
#include "board.h"
  extern int16_t err;
  extern int16_t l_err;
  extern int16_t ll_err;
  extern int16_t actual_speed; 
  extern int16_t now_PWM;

  extern int16_t aims_speed;
  extern float Kp;
  extern float Ki;
  extern float Kd;
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart_edma.h"
#if defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT
#include "fsl_dmamux.h"
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ECHO_BUFFER_LENGTH 8
/*******************************************************************************
 * Variables
 ******************************************************************************/
const int ParNum=8;
uint8_t camaddress=0,rxflag = 0;
uint16_t camdata=0;

lpuart_transfer_t sendXfer;
lpuart_edma_handle_t g_lpuartEdmaHandle;
edma_handle_t g_lpuartTxEdmaHandle;
edma_handle_t g_lpuartRxEdmaHandle;
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_tipString[]) =
    "LPUART EDMA example\r\nSend back received data\r\nEcho every 8 characters\r\n";
// AT_NONCACHEABLE_SECTION_INIT(uint8_t g_txBuffer[ECHO_BUFFER_LENGTH]) = {0};
// AT_NONCACHEABLE_SECTION_INIT(uint8_t g_rxBuffer[ECHO_BUFFER_LENGTH]) = {0};

volatile bool txBufferFull = false;
volatile bool txOnGoing = false;

extern uint8_t img_start[2],img_end[2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* LPUART user callback */
void LPUART_UserCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Code
 ******************************************************************************/
/* LPUART user callback */
void LPUART_UserCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_LPUART_TxIdle == status)
    {
        txBufferFull = false;
        txOnGoing = false;
		LPUART_WriteBlocking(LPUART1, img_end, 2);
    }

}

void uart_Init(void)
{
	lpuart_config_t config;
	LPUART_GetDefaultConfig(&config);
	config.baudRate_Bps = 1382400U;
	config.enableTx = true;
	config.enableRx = true;
	config.txFifoWatermark = 0;
	config.rxFifoWatermark = 0;
	LPUART_Init(LPUART1, &config, BOARD_DEBUG_UART_CLK_FREQ);
	LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable);
	//LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable);
	EnableIRQ(LPUART1_IRQn);


	edma_config_t edmaconfig;
    //lpuart_transfer_t xfer;
    /* Init DMAMUX */
    DMAMUX_Init(DMAMUX);
    /* Set channel for LPUART */
    DMAMUX_SetSource(DMAMUX, 0U, kDmaRequestMuxLPUART1Tx);
    DMAMUX_EnableChannel(DMAMUX, 0U);
    /* Init the EDMA module */
    EDMA_GetDefaultConfig(&edmaconfig);
    EDMA_Init(DMA0, &edmaconfig);
    EDMA_CreateHandle(&g_lpuartTxEdmaHandle, DMA0, 0U);
    /* Create LPUART DMA handle. */
    LPUART_TransferCreateHandleEDMA(LPUART1, &g_lpuartEdmaHandle, LPUART_UserCallback, NULL, &g_lpuartTxEdmaHandle,NULL);
    /* Send g_tipString out. */
    //xfer.data = g_tipString;
    //xfer.dataSize = sizeof(g_tipString) - 1;
    //txOnGoing = true;
    //LPUART_SendEDMA(LPUART1, &g_lpuartEdmaHandle, &xfer);

}

void uartcontrol(uint8_t *tmp)
{
	static uint8_t startcontrol[2]={0x0d,0x0a};
	static uint8_t endcontrol[2]={0x0a,0x0d};
	
	LPUART_WriteBlocking(LPUART1, startcontrol, 2);
	LPUART_WriteBlocking(LPUART1, tmp, 16);
	LPUART_WriteBlocking(LPUART1, endcontrol, 2);
}

void uartsend(uint16_t *rxuartdata)
{
	// static union
    //     {
    //         uint8_t Serialdata[ParNum];
    //         int16_t realdata[ParNum];
    //     }int8to16;
	// 	int8to16.realdata[0]=DisCNT();
	// 	int8to16.realdata[1]=DisCNT();
	// 	int8to16.realdata[2]=DisCNT();
	// 	int8to16.realdata[3]=DisCNT();
	// 	int8to16.realdata[4]=DisCNT();
	// 	int8to16.realdata[5]=DisCNT();
	// 	int8to16.realdata[6]=DisCNT();
	// 	int8to16.realdata[7]=DisCNT();
	// 	uartcontrol(int8to16.Serialdata);
}

void Speedupdate(void)
{
	static union
        {
            uint8_t Serialdata[12];
            int16_t realdata[6];
        }Speednumdata;
	static uint8_t startcontrol[2]={0x0d,0x0a};
	static uint8_t endcontrol[2]={0x0a,0x0d};
	Speednumdata.realdata[0]=DisCNT();
    Speednumdata.realdata[1]=err;
	Speednumdata.realdata[2]=l_err;
	Speednumdata.realdata[3]=ll_err;
	Speednumdata.realdata[4]=actual_speed;
	Speednumdata.realdata[5]=now_PWM;
	LPUART_WriteBlocking(LPUART1, startcontrol, 2);
	LPUART_WriteBlocking(LPUART1, Speednumdata.Serialdata, 12);
	LPUART_WriteBlocking(LPUART1, endcontrol, 2);
		
}

void LPUART1_IRQHandler(void)
{
	/* If new data arrived. */
	static uint8_t cmd_num = 0, datainx = 0, rxindex = 0;
	uint8_t data = 0;
	static union {
		uint8_t data[24];
		uint16_t data16[12];
		float ActVal[6];
	} posture;

	if ((kLPUART_RxDataRegFullFlag) && LPUART_GetStatusFlags(LPUART1))
	{
		data = LPUART_ReadByte(LPUART1);
		//LPUART_WriteByte(LPUART1,rxindex);
		//LPUART_WriteByte(LPUART1,data);
		//digitalToggle(GPIO1,9);
		switch (rxindex)
		{
		case 0:
			if (data == 0x0d)
				rxindex++;
			else
			{
				rxindex = 0;
				cmd_num = 0;
			}
			break;
		case 1:
			if (data == 0x0a)
			{
				datainx = 0; //数组索引
				rxindex++;   //数据索引
			}
			else if (data == 0x0d)
				rxindex = 1;
			else
			{
				rxindex = 0;
				cmd_num = 0;
			}
			break;
		case 2:
			cmd_num = data;
			switch (cmd_num)
			{
			case 0:
				/* code */
				rxindex++;
				break;
			case 1:
				/* code */
				rxindex++;
				break;
			case 2:
				/* code */
				rxindex++;
				break;
			case 3:
				/* code */
				rxindex++;
				break;
			default:
				rxindex = 0;
				break;
			}
			break;
		case 3:
			switch (cmd_num)
			{
			case 0:
				/* code */
				rxindex++;
				break;
			case 1:
				/* code */
				posture.data[datainx++] = data;
				if (datainx >= 3)
				{
					rxindex++;
				}
				break;
			case 2:
				posture.data[datainx++] = data;
				if (datainx >= 12)
				{
					rxindex++;
				}
				break;
			case 3:
				posture.data[datainx++] = data;
				if (datainx >= 2)
				{
					rxindex++;
				}
				break;
			default:
				rxindex = 0;
				break;
			}
			break;
		case 4:
			if (data == 0x0a)
				rxindex++;
			else
			{
				rxindex = 0;
				cmd_num = 0;
			}
			break;
		case 5:
			if (data == 0x0d)
			{
				switch (cmd_num)
				{
				case 0:
					/* code */
					break;
				case 1:
					camaddress = posture.data[2];
					camdata = (posture.data[0] << 8) | posture.data[1];
					break;
				case 2:
					Kp = posture.ActVal[0];
					Ki = posture.ActVal[1];
					Kd = posture.ActVal[2];
					break;
				case 3:
					aims_speed = posture.data16[0];
					DJ_PWM_Reload(aims_speed); //舵机测试
					break;
				}
				rxflag = 1;
			}
			cmd_num = 0;
			rxindex = 0;
			break;
		}
	}
}

void testSend(void)
{
	if(!txOnGoing)
	{
	LPUART_WriteBlocking(LPUART1, img_start, 2);
	LPUART_SendEDMA(LPUART1, &g_lpuartEdmaHandle, &sendXfer);
	txOnGoing=true;
	}
}
