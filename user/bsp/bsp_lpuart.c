#include "bsp_lpuart.h"
#include "fsl_lpuart.h"
#include "bsp_EnCoder.h"
#include "fsl_debug_console.h"

const int ParNum=8;
uint8_t camaddress=0,rxflag = 0;
uint16_t camdata=0;

void uart_Init(void)
{
	// lpuart_config_t config;
	// LPUART_GetDefaultConfig(&config);
	// config.baudRate_Bps = 115200U;
	// config.enableTx = true;
	// config.enableRx = true;
	// config.txFifoWatermark = 0;
	// config.rxFifoWatermark = 0;
	// LPUART_Init(LPUART5, &config, BOARD_DEBUG_UART_CLK_FREQ);
	LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable);
	//LPUART_EnableInterrupts(LPUART1, kLPUART_RxDataRegFullInterruptEnable);
	EnableIRQ(LPUART1_IRQn);
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
            uint8_t Serialdata[2];
            int16_t realdata[1];
        }Speednumdata;
	static uint8_t startcontrol[2]={0x0d,0x0a};
	static uint8_t endcontrol[2]={0x0a,0x0d};
	//Speednumdata.realdata[0]=DisCNT();
	LPUART_WriteBlocking(LPUART1, startcontrol, 2);
	LPUART_WriteBlocking(LPUART1, Speednumdata.Serialdata, 2);
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
				rxindex += 2;
				break;
			case 3:
				/* code */
				rxindex += 2;
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
					/* code */
					camaddress = posture.data[2];
					//camdata=posture.data[0];
					camdata = (posture.data[0] << 8) | posture.data[1];
					//PRINTF("camaddress: %x\r\n", camaddress);
					//PRINTF("camdata: %x\r\n", camdata);
					break;
				}
				rxflag = 1;
				// digitalLo(GPIO1,9);
			}
			cmd_num = 0;
			rxindex = 0;
			break;
		}
	}
}
