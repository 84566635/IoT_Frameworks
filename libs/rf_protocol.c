/** @file rf_protocol.c
 *
 * @author Wassim FILALI  STM8L
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation
 * $Revision:
 *
*/

#include "rf_protocol.h"

//for printf
#include "uart.h"



void rf_get_tx_alive_3B(BYTE NodeId, BYTE* tx_data)
{
      tx_data[0]= rf_pid_0x75_alive;
      tx_data[1]= NodeId;
      tx_data[2]= tx_data[0] ^ NodeId;
}

//Rx 3 Bytes
void rx_alive(BYTE *rxData,BYTE rx_DataSize)
{
	BYTE crc = rxData[0] ^ rxData[1];
	if(crc == rxData[2])
	{
		printf("NodeId:");
		UARTPrintf_uint(rxData[1]);
		printf_ln(",is:Alive");
	}
	else
	{
		printf_ln("Protocol Id: 0x75, CRC Fail");
	}
}

//Rx 5 Bytes
void rx_light(BYTE *rxData,BYTE rx_DataSize)
{
	unsigned int SensorVal;
	BYTE crc = rxData[0] ^ rxData[1];
	if(crc == rxData[4])
	{
	  printf("NodeId:");
	  printf_uint(rxData[1]);
	  printf(",Light: ");
	  SensorVal = rxData[2];
	  SensorVal <<= 4;//shift to make place for the 4 LSB
	  SensorVal = SensorVal + (0x0F & rxData[3]);
	  printf_uint(SensorVal);
	  printf_eol();
	}
	else
	{
	  printf("Protocol Id: 0x3B, CRC Fail\n");
	}
}

//Rx 4 Bytes
void rx_magnet(BYTE *rxData,BYTE rx_DataSize)
{
	BYTE crc = rxData[0] ^ rxData[1] ^ rxData[2];
	if(crc == rxData[3])
	{
		printf("NodeId:");
		printf_uint(rxData[1]);
		printf(",is:");
		if(rxData[2] == 0)
		{
			printf_ln("Low");
		}
		else
		{
			printf_ln("High");
		}
	}
    else
    {
      printf_ln("Protocol Id: 0xC5, CRC Fail");
    }
}
