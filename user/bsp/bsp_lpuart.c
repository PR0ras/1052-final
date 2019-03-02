#include "bsp_lpuart.h"
#include "fsl_lpuart.h"
#include "bsp_EnCoder.h"

const int ParNum=8;

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
	static union
        {
            uint8_t Serialdata[ParNum];
            int16_t realdata[ParNum];
        }int8to16;
		int8to16.realdata[0]=DisCNT();
		int8to16.realdata[1]=DisCNT();
		int8to16.realdata[2]=DisCNT();
		int8to16.realdata[3]=DisCNT();
		int8to16.realdata[4]=DisCNT();
		int8to16.realdata[5]=DisCNT();
		int8to16.realdata[6]=DisCNT();
		int8to16.realdata[7]=DisCNT();
		uartcontrol(int8to16.Serialdata);
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
	Speednumdata.realdata[0]=DisCNT();
	LPUART_WriteBlocking(LPUART1, startcontrol, 2);
	LPUART_WriteBlocking(LPUART1, Speednumdata.Serialdata, 2);
	LPUART_WriteBlocking(LPUART1, endcontrol, 2);
		
}
