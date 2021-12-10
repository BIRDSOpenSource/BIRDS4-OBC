#include <16F1789.h>
BYTE next_in = 0;
int t;
#device adc=16
#fuses NOMCLR,WDT,NOPUT,NOBROWNOUT
#use delay(crystal=16mhz)
#include <COMPICREG.h>                                                           // com pic registers header
#include <COM_PIC_FM_JP.h>                                                     // com main header file

int auto_flag1 = 1;
int counti = 0;

#define CHECK_CMD (cr==pk) && ((sat_id == 0xAA)||(sat_id == 0xBB))               // (cr == pk) checking crc, AA is sat id
                                                                                 // BB is the common id for all sat

#int_rda
void serial_isr()
{
   buffer[next_in]=fgetc(tr_cp);
   t=next_in;
   next_in=(next_in+1)% 17 ;   
} 


void main(void)
{
settings();                                                                      // Every initial settings belongs to com pic is included in this function
Setup_wdt(wdt_256s);                                                             // Wdt setup

delay_ms(5000);                                                                  // Just delay before start
/*delay_ms(30000);
delay_ms(30000);*/

RX_ON();                                                                         // Reception mode activated

    While(true)
    { 

      //______________(1)______CW_RELATED_TASKS__________________________________________<<
       RSSI_VALUE = RSSI_READ();                                                 // Reading satellite noise level (Received signal strength indication AN4)
       
       COMUNICATRE_WITH_MAIN_PIC(1,0);                                           //Send 0x50 to Main PIC Asking CW Data
       WAIT_FOR_MAIN_PIC_RESPONSE();  
    
       SEND_CW();
       restart_wdt();                  

   
  
   //______________(2)____AUTOMATIC_PACKET_TRANSMITION_RELATED_TASKS__________________<<
     if((MAIN_TO_COM[14] == 0x77) && (counti == 0))
      {       
         rcv_byte = 0x77; 
         auto_flag1 = 1;
         counti = 1;  
      }
       
      if(MAIN_TO_COM[14] == 0x00)
      {       
         rcv_byte = 0x00; 
         auto_flag1 = 1;
         counti = 0;
         auto_address = 0;
      }
      
      if(MAIN_TO_COM[14] == 0x35)                                                //RESERVATION DATA DOWNLOAD
      {       
         rcv_byte = 0x35; 
         auto_address = 0;
      }
    //-----------------------------
      if(rcv_byte == 0x77)
      {  
         if(auto_flag1 == 1)
         {
            auto_address = make32(MAIN_TO_COM[15],MAIN_TO_COM[16],MAIN_TO_COM[17],MAIN_TO_COM[18]);
            Data_size    = make32(MAIN_TO_COM[19],MAIN_TO_COM[20],MAIN_TO_COM[21],MAIN_TO_COM[22]);
            
            COMUNICATRE_WITH_MAIN_PIC(2,1);                                      //getting SFM acces 
            delay_ms(3000);
            FLASH_COPY(auto_address,Data_size);                                  //erase 1 sector COM flash, reads Shared flash and writes COM flash
            auto_flag1 = 0;
         }
      
       TX_ON();                                                                  //TX mode ON
       delay_ms(1000);
       Send_Data_Packets(auto_address,0,5,1);                                    //start_address, data_size, packets, x
       delay_ms(500);
       RX_BFR_ZERO();                                                            //make CMD_pckt[] zero
       RX_ON();
      } 
    //___________________________________________________________________________________
    if(rcv_byte == 0x35)
      {  

            auto_address = make32(MAIN_TO_COM[15],MAIN_TO_COM[16],MAIN_TO_COM[17],MAIN_TO_COM[18]);
            packets_number = make32(MAIN_TO_COM[19],MAIN_TO_COM[20],MAIN_TO_COM[21],MAIN_TO_COM[22]);
          //access = MAIN_TO_COM[13];
            Data_size = packets_number * pckt_size;                              //pckt_size -> hex 0x51 = dec 81
            
            COMUNICATRE_WITH_MAIN_PIC(2,1);                                      //getting SFM acces 
            delay_ms(3000);
            FLASH_COPY(auto_address,Data_size);                                  //erase 1 sector COM flash, reads Shared flash and writes COM flash
     
       TX_ON();                                                                  //TX mode ON
       delay_ms(1000);
       Send_Data_Packets(auto_address,0,packets_number,1);                       //start_address, data_size, packets, x=1 ->COM FLASH
       delay_ms(500);
       RX_BFR_ZERO();                                                            //make CMD_pckt[] zero
       RX_ON();
       auto_address = 0;
       packets_number = 0;
       Data_size = 0;
      }
    
    //___________________________________________________________________________________
     restart_wdt();            
     RX_ON();
    //___________________________________________________________________________________
    
    
    //______________(3)____RECEIVING_MODE_START_HERE___________________________________<<   
     for (int i = 0; i < 40; i++)
     {
      BUFFER_CORECTION();                                                        // This function correct the receiving buffer CMD_pckt[] = buffer[]
       
       if(((sat_id == 0xAA)||(sat_id == 0xBB)) && (CMD_PCKT[2] == 0x90) )        // Satellite silent ON
       {
        Delay_for_data_downlink();                                               // send the Acknoledgement Success array packet Ack_pckt[32] and wait 10 min
       }
       
       if(CHECK_CMD)                                                             //(cr==pk) && ((sat_id == 0xAA)||(sat_id == 0xBB))
       {
        if(CMD_PCKT[2] == 0xAA) { Sync(); }                                      // send the Acknoledgement Success or NO success array packet Ack_pckt[32] and wait a delay
                                                                                 // indicated in the CMD_PCKT[11] position
        PACKET_DOWNLOAD();                                                       // cmd 0x10 , cmd 0x11      packets download from COM or shared flash
        LENGTH_DOWNLOAD();                                                       // cmd 0x13 , cmd 0x14      packets download (using size) from COM or shared flash
        ONE_PCKT_DOWNLOAD();                                                     // cmd 0x15 , cmd 0x16      one packet download from COM or shared flash
        Data_down_from_SFM_with_access();                                        // cmd 0x35
        SET_OF_PACKETS_DOWNLOAD_WITH_ACCESS();                                   // cmd 0x44               set of packets download from shared flash
        
        SET_OF_PACKETS_DOWNLOAD();                                               // cmd 0x40, cmd 0x41           40 00 00 00 00    00 00 setsize setnumber      
        
        SEND_COM_TEMP_RSSI_TO_GS();                                              // cmd 0x30       30 00 00 00 00    00 00 00 00 (send the array Ack_pckt[32] with temp and rssi)
        FM_COPY_AND_DATA_DOWN_FROM_CFM();                                        // cdm 0x27       25 00 00 00 00    00 00 packet size
        
        
        FM_COPY();                                                               // cmd 0x17    copy from SF to CF without getting accees
        FM_COPY_WITH_ACCESS();                                                   // cmd 22      copy from SF to CF with access
        GET_SFM_ACCESS_GIVEN_TIME();                                             // cmd 0x42
        
        CMD_TO_MPIC();                                                           // cmd 0xA0    from pos 4 to 11 do COM_TO_MAIN[] = CMD_PCKT[] and send to MAIN PIC
        CW_SEND_BGSC();                                                          // cmd 0x21    force CW send
        CW_UPDATE_MISSION();                                                     // cmd 0x20 
       
       }
       
      next_in=0;
      RX_BFR_ZERO();
      delay_ms(2000);
      restart_wdt(); 
     }                                                                           // for loop finish here

    
   }                                                                             //while loop finish here
}


