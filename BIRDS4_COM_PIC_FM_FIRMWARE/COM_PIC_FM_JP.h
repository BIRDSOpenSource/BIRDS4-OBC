/* This file contains everything related to compic functions and compic settings
   ,compic comuniction ports
*/

//_______________________________________________________________________________________________________________
//#use rs232(baud=9600, parity=N, xmit=PIN_B7, rcv=PIN_B6, bits=8, stream=PORT1)    //PC reading port
//_______________________________________________________________________________________________________________
#use rs232(uart1,baud=115200, parity=N, bits=8, stream=TR_CP)                      //UART________TRX 

#use rs232(baud=4800, parity=N, xmit=PIN_B3, rcv=PIN_B4, bits=8, stream=RP_CP, ERRORS)    //UART________RESET-PIC
#use rs232(baud=9600, parity=N, xmit=PIN_B1, rcv=PIN_B2, bits=8, stream=MP_CP, ERRORS)    //UART________MAIN-PIC   
//_______________________________________________________________________________________________________________
#use spi(MASTER, CLK=PIN_D4, DI=PIN_D6, DO=PIN_D7, BAUD=400000, BITS=8, STREAM=CFM, MODE=0) //FLASH_M_______COM
#use spi(MASTER, CLK=PIN_C3, DI=PIN_C4, DO=PIN_C5, BAUD=400000, BITS=8, STREAM=SFM, MODE=0) //FLASH_M_______SHARED
//_______________________________________________________________________________________________________________

unsigned int16 RSSI_VALUE;

unsigned int32 auto_address = 0;
unsigned int32 Data_size = 0;
unsigned int8 rcv_byte = 0;
unsigned int32 packets_number = 0;
//unsigned int8 access = 0;
unsigned int8 pckt_size = 0x51;

#include <COMPIC_FUN.c>                                                          
#include <COM_FLASH.c>                                                           
#include <PACKET_CODE_JP.c>
#include <CW_JP.c>                                                              
#include <CMD_FUN.c> 
 


//_________________Tranceiver Control__________________________________________
   Void TX_ON();     //__Data Transmission Enable__ 
   Void RX_ON();     //__Commamd Receiving Enable__
   Void CW_ON();     //__CW Transmission Enable__
   
   Unsigned int16 TRX_TEMP_READ();    //__Tranceiver Temperature Read__
   unsigned int16 TRX_RSSI_READ();    //__Received Power Level__
//_____________________________________________________________________________


//_________________Input Command Buffer________________________________________
   Void RX_BFR_ZERO();   //Make receive buffer Zero__
//_____________________________________________________________________________


//_________________Flash memory functions______________________________________
   Void CF_WRITE_ENABLE();
   Void SF_WRITE_ENABLE();
   
   Void CF_SECTOR_ERASE(unsigned int32 sector_address);
   Void SF_SECTOR_ERASE(unsigned int32 sector_address);
   
   Void CF_BYTE_WRITE(unsigned int32 byte_address, int8 data);
   Void SF_BYTE_WRITE(unsigned int32 byte_address, int8 data);
   
   int8 CF_BYTE_READ(unsigned INT32 ADDRESS);
   int8 SF_BYTE_READ(unsigned INT32 ADDRESS);
   
   Void FLASH_COPY(unsigned int32 start_address,unsigned int32 data_length);
//_____________________________________________________________________________




