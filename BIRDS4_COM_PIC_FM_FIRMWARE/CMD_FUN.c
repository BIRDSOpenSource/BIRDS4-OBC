//from here the mission________________________________________________________

//_______________________PAcket_download_______________________________________
Void PACKET_DOWNLOAD()
{
    if(cmd_Pckt[3]==0x10 || cmd_Pckt[3]==0x11 )                                  //packets download
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 pckt_num     = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    TX_ON();
    Delay_ms(1000);
       if(cmd_Pckt[3]==0x10)                                                     //download from COM flash
       {
       Send_Data_Packets(start_adress,0,pckt_num,1);                             //generates the array data_pckt[105], expects address, data size, packets, x (x==1 -> CF )
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x11)                                                     //download from shared flash
       {
       Send_Data_Packets(start_adress,0,pckt_num,0);                             //generates the array data_pckt[105], expects address, data size, packets, x (x==0 -> SF )
       RX_BFR_ZERO();
       }
    Delay_ms(500);    
    RX_ON();
   }
    
   return;
}


//____________________Lenth_Download___________________________________________
Void LENGTH_DOWNLOAD()
{
   if(cmd_Pckt[3]==0x13 || cmd_Pckt[3]==0x14 )                                   // length download
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 length       = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    TX_ON();
    Delay_ms(1000);
    
       if(cmd_Pckt[3]==0x13)
       {
       Send_Data_Packets(start_adress,length,0,1);
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x14)
       {
       Send_Data_Packets(start_adress,length,0,0);
       RX_BFR_ZERO();
       }
       
    Delay_ms(500);    
    RX_ON();
   }  
   
   
   return;

}

//____________________One_packt_download_______________________________________

Void ONE_PCKT_DOWNLOAD()
{
   if(cmd_Pckt[3]==0x15 || cmd_Pckt[3]==0x16)                                    // one packt download
   {
    TX_ON();
    Delay_ms(1000);
    
       if(cmd_Pckt[3]==0x15)
       {
       Send_a_Packet(1);
       RX_BFR_ZERO();
       }
       
       if(cmd_Pckt[3]==0x16)
       {
       Send_a_Packet(0);
       RX_BFR_ZERO();
       }   
       
    Delay_ms(500);    
    RX_ON();
   } 
   
   return;
}

//__________________________SET of packet download________________________________________
Void SET_OF_PACKETS_DOWNLOAD()
{
   if(cmd_Pckt[3]== 0x40 || cmd_Pckt[3] == 0x41 )                                //packets download
   {
    unsigned int32 start_adress  = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
     
    TX_ON();
    Delay_ms(1000);
       if(cmd_Pckt[3]==0x40)
       {
        PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],1);
        RX_BFR_ZERO();
       }
       
       else if(cmd_Pckt[3]==0x41)
       {
        PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],0);
        RX_BFR_ZERO();
       }
       
    Delay_ms(500);    
    RX_ON();  
   }
   
   return;
}

//set of acess downpppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp
Void SET_OF_PACKETS_DOWNLOAD_WITH_ACCESS()
{
   if(cmd_Pckt[3]== 0x44)                                                        //packets download
   {
    unsigned int32 start_adress  = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);                                    //getting accees sending time
    
    TX_ON();
    Delay_ms(1000);
       
    PCKT_SET_SEND(start_adress,cmd_Pckt[11],cmd_Pckt[10],0);                     //initial_address, packet_set_size, packet_set_num, FM_SELECT
    RX_BFR_ZERO();
   
       
    Delay_ms(500);    
    RX_ON();  
   }
   
   return;
}



//________________________FM_data_copy_________________________________________
Void FM_COPY()
{
   if(cmd_Pckt[3]==0x17)                                                         // copy
   {
    SEND_SUCCESS_ACK();                                                          // send success ACK using Ack_pckt[32];
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 length       = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
    FLASH_COPY(start_adress,length);                                             //borra un sector de la COM flash, lee de Shared flash y escribe en COM flash
    
    SEND_SUCCESS_ACK();                                                          // send success ACK using Ack_pckt[32];
    RX_BFR_ZERO();                                                               //make CMD_pckt[] zero
    
    RX_ON();
   }
    
   return;
}

//________________________Copy With Access_________________________________________________
Void FM_COPY_WITH_ACCESS()
{
   if(cmd_Pckt[3]==0x22)                                                         // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);             //as 256byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);                                    //getting accees
    WAIT_FOR_MAIN_PIC_RESPONSE();
    
       if(MAIN_TO_COM[12] == 0x66)
       {
        SEND_SUCCESS_ACK();                                                      // success of access
        delay_ms(1000);
        FLASH_COPY(start_adress,packet*256);                                     //erase 1 sector of COM Flash, reads Shared flash and writes COM flash
        SEND_SUCCESS_ACK();                                                      // copy success
       }
       
       else SEND_NOT_SUCCESS_ACK();
 
    RX_BFR_ZERO();                                                               //make CMD_packt[] zero
    RX_ON();
   }
   
   return;
}



//________________________Copy With Access_________________________________________________
Void FM_COPY_AND_DATA_DOWN_FROM_CFM()
{
   if(cmd_Pckt[3]==0x27)                                                         // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);             //as 81byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);                                    //getting accees for 1 min
    WAIT_FOR_MAIN_PIC_RESPONSE();
    
    
       if(MAIN_TO_COM[12] == 0x66)
       {
        FLASH_COPY(start_adress,packet*81);                                      //reads Shared flash and writes COM flash in start_address
        
        TX_ON();
        Delay_ms(1000);
        Send_Data_Packets(start_adress,0,packet,1);                              //start_address, data_size, packets num, x=1 COM flash)
        Delay_ms(500);
        RX_ON();
       }
       
       else
       { 
        TX_ON();
        Delay_ms(1000);
        SEND_NOT_SUCCESS_ACK();
        Delay_ms(500);
        RX_ON();
       }
 
    RX_BFR_ZERO();
   }
   
   return;
}

//999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
//________________________Copy With Access_________________________________________________
Void Data_down_from_SFM_with_access()
{
   if(cmd_Pckt[3]==0x35)                                                         // copy
   {
    unsigned int32 start_adress = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int16 packet       = make16(cmd_Pckt[10],cmd_Pckt[11]);             //as 81 byte packets
    
    COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[8]);                                    //command to get acces, fill the array COM_TO_MAIN[]
    WAIT_FOR_MAIN_PIC_RESPONSE();                                                //wait for main PIC response and load MAIN_TO_COM[] array with mainPIC answer
    
    Delay_ms(2000);
    
       if(MAIN_TO_COM[12] == 0x66)
       {    
        
        TX_ON();                                                                 //TX mode ON
        Delay_ms(1000);
        Send_Data_Packets(start_adress,0,packet,0);                              //generates the array data_pckt[105], using address, data size, packets, x (x==1 -> CF; x==0 -> SF )
        Delay_ms(500);                                                           //and sends to GS
        RX_ON();                                                                 //RX mode ON
       }
 
    RX_BFR_ZERO();                                                               //clear buffer CMD_pckt[] to write GS data
   }
   
   return;
}


void sync()
{
 unsigned int8 delay = CMD_PCKT[11];
 
   if(delay <= 120)                                                              //max delay 10 min
   {
    SEND_SUCCESS_ACK();                                                          //send the Acknoledgement Success array packet Ack_pckt[32]
      for(int i = 0; i <delay ; i++)                                             //wait delay time
      {
         delay_ms(5000);
         restart_wdt();
      }
   }
   
   else {SEND_NOT_SUCCESS_ACK();}                                                // send Acknoledgement NO Success packet array Ack_pckt[14]
   
 RX_BFR_ZERO();                                                                  // make CMD_pckt[] zero
 return;
}

// ___________________CMD to main PIC___________________________________________________
Void CMD_TO_MPIC()
{
   if(CMD_PCKT[3]==0xA0)                                                         // main pic comands
    {      
       COMUNICATRE_WITH_MAIN_PIC(0,0);                                           //from pos 4 to 11 do COM_TO_MAIN[] = CMD_PCKT[] and send to MAIN PIC   
       WAIT_FOR_MAIN_PIC_RESPONSE();
       
       if(MAIN_TO_COM[12] == 0x66) SEND_SUCCESS_ACK();
       
       if(MAIN_TO_COM[12] == 0x00) SEND_NOT_SUCCESS_ACK();
       
       RX_BFR_ZERO();
    }
    
    return;
}

//_________________________GET_FM_ACCESS_________________________________________________
Void GET_SFM_ACCESS_GIVEN_TIME()
{
   if(CMD_PCKT[3]==0x42 && ((CMD_PCKT[11]==0x01) || (CMD_PCKT[11]==0x05) || (CMD_PCKT[11]==0x0A) ))    // main pic comands
    {   
       
       
       COMUNICATRE_WITH_MAIN_PIC(2,cmd_Pckt[11]);                                //se le pasa el cmd para get access and time value
       
       
       WAIT_FOR_MAIN_PIC_RESPONSE();
       
       if(MAIN_TO_COM[12] == 0x66) SEND_SUCCESS_ACK();
       
       if(MAIN_TO_COM[12] == 0x00) SEND_NOT_SUCCESS_ACK();
       
       
       RX_BFR_ZERO();                                                            //make CMD_pckt[] zero
    }
    
    return;
}


//__________________________SPECIAL______________________________________________________
Void Delay_for_data_downlink()
{
 SEND_SUCCESS_ACK();                                                             // send the Acknoledgement Success array packet Ack_pckt[32]
    
      for(int i = 0; i <90; i++)                                                 //30min delay
      {
         delay_ms(20000);
         restart_wdt();           
      }
      
 RX_BFR_ZERO();                                                                  //make CMD_pckt[] array zero
 return;
}




//__________________force CW send________________________________________________________
Void CW_SEND_BGSC()
{

   if(CMD_PCKT[3]==0x21)                                                         // cw send
    {
       CW_ON();                                                                  // CW mode
       
       CW_PCKT();                                                                // generates the array with the format [BIRDS4]__[CALL SIGN]__[CW_MSN]__ and transmite it
       delay_ms(500);
       
       RX_ON();
       RX_BFR_ZERO();
    }
    
    return;
}

//__________________CW update mission____________________________________________________
void CW_UPDATE_MISSION()
{
   if(CMD_PCKT[3]==0x20)                                                         // cw update
   {
       CF_SECTOR_ERASE(0x00000000);                                              //erase 1 sector
       
       CF_BYTE_WRITE(0x00000000,CMD_PCKT[10]);                                   //should be ff
       
       CF_BYTE_WRITE(0x00000001,CMD_PCKT[4]);
       CF_BYTE_WRITE(0x00000002,CMD_PCKT[5]);
       CF_BYTE_WRITE(0x00000003,CMD_PCKT[6]);
       CF_BYTE_WRITE(0x00000004,CMD_PCKT[7]);
       CF_BYTE_WRITE(0x00000005,CMD_PCKT[8]);
       CF_BYTE_WRITE(0x00000006,CMD_PCKT[9]);
       
       
       
       SEND_SUCCESS_ACK();
       RX_BFR_ZERO();
    }
 
 return;
}


//_______________________________SEND temp rssi__________________________________________
Void SEND_COM_TEMP_RSSI_TO_GS()
{
   if(CMD_PCKT[3]==0x30)    
    {      
       SEND_COM_DATA();                                                          //send the array Ack_pckt[32] with temp and rssi data
       RX_BFR_ZERO();                                                            //make CMD_pckt[14] zero
    }
    
    return;
}


//_______________________________CW_send_________________________________________________
Void SEND_CW()
{
   CW_ON();                                                                      //set CW mode   
   CW_PCKT();                                                                    // make the format [BIRDS4]__[CALL SIGN]__[CW_MSN]__                                                                 
   RX_ON();                                                                      // set RX mode
   RX_BFR_ZERO();                                                                // This function make rx buffer zero
   return;
}


