/** @file nRF_SPI.h
 *
 * @author Wassim FILALI taken over from  Runar Kjellhaug out of nRF24L01P-EK
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */

#include "nRF_SPI.h"
#include "spi_stm8x.h"
#include "clock_led.h"

const unsigned char nRF24L01pRegNumber = 0x18;

const char* nRF24L01pRegNames[] = { "CONFIG",
                                    "EN_AA",
                                    "EN_RXADDR",
                                    "SETUP_AW",
                                    "SETUP_RETR",
                                    "RF_CH",
                                    "RF_SETUP",
                                    "STATUS",
                                    "OBSERVE_TX",
                                    "CD",
                                    "RX_ADDR_P0",
                                    "RX_ADDR_P1",
                                    "RX_ADDR_P2",
                                    "RX_ADDR_P3",
                                    "RX_ADDR_P4",
                                    "RX_ADDR_P5",
                                    "TX_ADDR ",
                                    "RX_PW_P0",
                                    "RX_PW_P1",
                                    "RX_PW_P2",
                                    "RX_PW_P3",
                                    "RX_PW_P4",
                                    "RX_PW_P5",
                                    "FIFO_STATUS"};

//these registers are not in sequence but at addresses 0x1C 0x1D
//                                    "DYNPD",
//                                    "FEATURE"};

//**********************************************************//
void SPI_Init()
{
  
  
	SPI_Init_ChipSelect_Pin();//Starts Disabled
	
	SPI_Init_ChipEnable_Pin();//Starts Disabled
	
        SPI_Init_SCK_MOSI_MISO_Pins();
        
	SPI_Init_Peripheral();
	
}


BYTE SPI_RW(BYTE byte)
{
	BYTE res = SPI_WriteRead(byte);
	
	delay_100us();//must be after the transaction is started TODO debug the timing to check with which values commands are correctly handled
    return(res);
}


BYTE SPI_Read_Register(BYTE reg)
{
	BYTE reg_val;

	CSN_Pin_LowSelect();
	SPI_RW(reg);
	reg_val = SPI_RW(0);    
	CSN_Pin_HighDisable();  

	return(reg_val);        
}

BYTE SPI_Cmd_ReadRxPayloadWidth()
{
	BYTE payload_Width;

	CSN_Pin_LowSelect();
	SPI_RW(R_RX_PL_WID);
	payload_Width = SPI_RW(0);    
	CSN_Pin_HighDisable();  

	return(payload_Width);
}

void SPI_Cmd_FlushRx()
{
	CSN_Pin_LowSelect();
	SPI_RW(FLUSH_RX);
	CSN_Pin_HighDisable();  
}

BYTE SPI_Write_Register(BYTE reg, BYTE value)
{
	BYTE status;
	BYTE last_state = CE_Pin_getstate();
	CE_Pin_LowDisable();

	CSN_Pin_LowSelect();	
	status = SPI_RW(WRITE_REG | reg);
	SPI_RW(value);        
	CSN_Pin_HighDisable();

	if(last_state)
	{
		CE_Pin_HighEnable();
	}

	return(status);       
}

BYTE SPI_Command(BYTE reg)
{
	BYTE status;
	BYTE last_state = CE_Pin_getstate();
	CE_Pin_LowDisable();

	CSN_Pin_LowSelect();
	status = SPI_RW( reg);
	CSN_Pin_HighDisable();

	if(last_state)
	{
		CE_Pin_HighEnable();
	}

	return(status);       
}

BYTE SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE size)
{
	BYTE status,byte_ctr;
	BYTE last_state = CE_Pin_getstate();
	CE_Pin_LowDisable();

	CSN_Pin_LowSelect();                           
	status = SPI_RW(reg);                          

	SPI_RW(size);								//protocol with size as first Byte
	for(byte_ctr=0; byte_ctr<size; byte_ctr++)	//will send size+1 bytes
		SPI_RW(*pBuf++);

	CSN_Pin_HighDisable();
	if(last_state)
	{
		CE_Pin_HighEnable();
	}

	return(status);                                
}


BYTE SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes)
{
BYTE status,byte_ctr;

	//Reading in Standby Mode, why ?
	BYTE last_state = CE_Pin_getstate();
	CE_Pin_LowDisable();

	CSN_Pin_LowSelect();                         
	status = SPI_RW(reg);                        

	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
	pBuf[byte_ctr] = SPI_RW(0);                

	CSN_Pin_HighDisable();                       

	if(last_state)
	{
		CE_Pin_HighEnable();
	}
	
	return(status);                              
}

