#define NRZI3_DATA_SIZE 101   

int16 mk_CRC(unsigned int8 *data, unsigned int8 data_number)
{
   unsigned int32 crcReg = 0xFFFF;   
   unsigned int32 calc = 0x8408;     
   unsigned int8 w;            
   int32 i;                    
   int32 k;                     
   unsigned int8 cal_data[NRZI3_DATA_SIZE];
   
   for(k = 0; k < data_number; k++)
   {
      cal_data[k] = data[k];   

      for(i = 0; i < 8; i++)
      {
         w = (crcReg ^ cal_data[k]) & 0x0001;
         crcReg = crcReg >> 1;

         if(w == 1)
         {
            crcReg = crcReg ^ calc;
         }

         cal_data[k] = cal_data[k] >> 1;
      }
   }

   crcReg = crcReg ^ 0xFFFF;   
   return crcReg;
}
//___________________________________________________________________________<<



//___________________________________________________________________________>>
//_________________Receive data packets________________________________________

//int ib = 0; //input buffer increment

unsigned int8 CMD_pckt[14]={0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0};
                            
unsigned int ib = 0;




//______________________This function make rx buffer zero______________________________<<
void RX_BFR_ZERO()
{
   for(int p = 0; p<14; p++)
   {
    CMD_pckt[p]=0x00;
   }
  
  ib = 0;
   return;
}
//_____________________________________________________________________________________<<





//_____________________________________________________________________________________<<
//_________________Receive data dump buffer______________________________________________
//_________________This function make temporaly buffer zero______________________________
unsigned int8 buffer[17];
void buffer_ZERO()
{
   for(int p = 0; p<17; p++)
   {
    buffer[p]=0x00;
   }
  
   return;
}

//_____________________________________________________________________________________<<






//_____________________________________________________________________________________>>
//_________________Acknoledgement Success packet_________________________________________

unsigned int8 Ack_pckt[32];

Void SEND_SUCCESS_ACK()
{
//    Ground stationCall sign---------------[ 0-----5 ]
      Ack_pckt[0] = 0x4a;     // J
      Ack_pckt[1] = 0x47;     // G               
      Ack_pckt[2] = 0x36;     // 6
      Ack_pckt[3] = 0x59;     // Y
      Ack_pckt[4] = 0x42;     // B
      Ack_pckt[5] = 0x57;     // W
      
//    Dest SSID-----------------------------[ 6 ]
      Ack_pckt[6] = 0x30;     //char 0

//    Satellite Call Sign-------------------[ 7-----12 ]
      Ack_pckt[7] = 0x4A;     // J
      Ack_pckt[8] = 0x47;     // G
      Ack_pckt[9] = 0x36;     // 6
      Ack_pckt[10] = 0x59;    // Y
      Ack_pckt[11] = 0x4D;    // M
      Ack_pckt[12] = 0x58;    // X
      
//    Source SSID---------------------------[ 13 ]
      Ack_pckt[13] = 0x30;    //char 0
      
//    Control-------------------------------[ 14 ]
      Ack_pckt[14] = 0x3E;    //
      
//    PID-----------------------------------[ 15 ]
      Ack_pckt[15] = 0xF0;    //
      
//    Header--------------------------------[ 16 ]
      Ack_pckt[16] = 0xAA;    //

//    SAT Header----------------------------[ 17]
      Ack_pckt[17] = 0x03;    //   
      
//    Pckt sequence number higher byte------[ 18 ]
      Ack_pckt[18] = CMD_pckt[1];    // 

//    Pckt sequence number lower byte-------[ 19 ]
      Ack_pckt[19] = CMD_pckt[2];    // 
      
//    CMD back------------------------------[ 20 - 28 ]
      Ack_pckt[20] = CMD_pckt[3];
      Ack_pckt[21] = CMD_pckt[4];
      Ack_pckt[22] = CMD_pckt[5];
      Ack_pckt[23] = CMD_pckt[6];
      Ack_pckt[24] = CMD_pckt[7];
      Ack_pckt[25] = CMD_pckt[8];
      Ack_pckt[26] = CMD_pckt[9];
      Ack_pckt[27] = CMD_pckt[10];
      Ack_pckt[28] = CMD_pckt[11];

//    Footer--------------------------------[ 29 ]
      Ack_pckt[29] = 0xAA;    //
      
//    CRC Calculation-----------------------[ 30 - 31 ]
      int16 outdata = mk_CRC(Ack_pckt, 30);
      
      Ack_pckt[30] = outdata ;
      Ack_pckt[31] = outdata >> 8;
      
//    Send pckt------------------------------
      OUTPUT_high(pin_d1);                                                       //TX mode
      Delay_ms(2000);
        for(int ih=0;ih<32;ih++)                                                 //buffer print
        {
           fputc(Ack_pckt[ih],TR_CP); 
        }
      Delay_ms(500);
      OUTPUT_low(pin_d1);                                                        //RX mode
      
      return;     
}
//___________________________________________________________________________<<



//_____________________________________________________________________________________>>
//_________________Acknoledgement NO Success packet_________________________________________

Void SEND_NOT_SUCCESS_ACK()
{
//    Ground stationCall sign---------------[ 0-----5 ]
      Ack_pckt[0] = 0x4a;     // J
      Ack_pckt[1] = 0x47;     // G               
      Ack_pckt[2] = 0x36;     // 6
      Ack_pckt[3] = 0x59;     // Y
      Ack_pckt[4] = 0x42;     // B
      Ack_pckt[5] = 0x57;     // W
      
//    Dest SSID-----------------------------[ 6 ]
      Ack_pckt[6] = 0x30;     //char 0

//    Satellite Call Sign-------------------[ 7-----12 ]
      Ack_pckt[7] = 0x4A;     // J
      Ack_pckt[8] = 0x47;     // G
      Ack_pckt[9] = 0x36;     // 6
      Ack_pckt[10] = 0x59;    // Y
      Ack_pckt[11] = 0x4D;    // M
      Ack_pckt[12] = 0x58;    // X
      
//    Source SSID---------------------------[ 13 ]
      Ack_pckt[13] = 0x30;    //char 0
      
//    Control-------------------------------[ 14 ]
      Ack_pckt[14] = 0x3E;    //
      
//    PID-----------------------------------[ 15 ]
      Ack_pckt[15] = 0xF0;    //
      
//    Header--------------------------------[ 16 ]
      Ack_pckt[16] = 0xAA;    //

//    SAT Header----------------------------[ 17]
      Ack_pckt[17] = 0x03;    //   
      
//    Pckt sequence number higher byte------[ 18 ]
      Ack_pckt[18] = CMD_pckt[1];    // 

//    Pckt sequence number lower byte-------[ 19 ]
      Ack_pckt[19] = CMD_pckt[2];    // 
      
//    CMD back------------------------------[ 20 - 28 ]
      Ack_pckt[20] = 0x69;
      Ack_pckt[21] = 0x69;
      Ack_pckt[22] = 0x69;
      Ack_pckt[23] = 0x69;
      Ack_pckt[24] = 0x69;
      Ack_pckt[25] = 0x69;
      Ack_pckt[26] = 0x69;
      Ack_pckt[27] = 0x69;
      Ack_pckt[28] = 0x69;

//    Footer--------------------------------[ 29 ]
      Ack_pckt[29] = 0xAA;    //
      
//    CRC Calculation-----------------------[ 30 - 31 ]
      int16 outdata = mk_CRC(Ack_pckt, 30);
      
      Ack_pckt[30] = outdata ;
      Ack_pckt[31] = outdata >> 8;
      
//    Send pckt------------------------------
      OUTPUT_high(pin_d1);
      Delay_ms(2000);
        for(int ih=0;ih<32;ih++)                //buffer print
        {
           fputc(Ack_pckt[ih],TR_CP); 
        }
      Delay_ms(500);
      OUTPUT_low(pin_d1);
      
      return;     
}
//_____________________________________________________________________________________<<









//_____________________________________________________________________________________>>
//_________________Output data packets___________________________________________________

unsigned int8 Data_pckt[105]  ;


Void Send_Data_Packets(unsigned int32 start_address, unsigned int32 data_size, unsigned int32 packets,int1 x)
{


 unsigned int32 num_of_pckt;
 
    if(packets == 0)                                                             
    {
     num_of_pckt = (data_size/81)+1;
    }
 
    else if(data_size == 0)
    {
     num_of_pckt = packets ;
    }
 
 
for(unsigned int32 pckt_seq_num = 1; pckt_seq_num <= num_of_pckt; pckt_seq_num ++)
{ 
   //=================Ground stationCall sign part=============================
   //========================= 0 --- 5 ========================================
      data_pckt[0] = 0x4a;     // J                                             
      data_pckt[1] = 0x47;     // G               
      data_pckt[2] = 0x36;     // 6
      data_pckt[3] = 0x59;     // Y
      data_pckt[4] = 0x42;     // B
      data_pckt[5] = 0x57;     // W
   //=========================Dest SSID========================================
   //=========================   6     ========================================
      data_pckt[6] = 0x30;     //char 0
   //===================Satellite Call Sign====================================
   //========================= 7 --- 12  ======================================
      data_pckt[7] = 0x4A;     // J
      data_pckt[8] = 0x47;     // G
      data_pckt[9] = 0x36;     // 6
      data_pckt[10] = 0x59;    // Y 
      data_pckt[11] = 0x4D;    // M
      data_pckt[12] = 0x58;    // X
   //========================Source SSID=======================================
   //=========================   13   =========================================
      data_pckt[13] = 0x30;    //char 0
   //==========================Control=========================================
   //=========================   14   =========================================
      data_pckt[14] = 0x3E;//
   //============================PID===========================================
   //=========================   15   =========================================
      data_pckt[15] = 0xF0;//
   //=====================Packet_specified_data================================
   //========================= 16 ---- 17 =====================================
      data_pckt[16] = 0xFF;
      data_pckt[17] = 0xF0;
      data_pckt[18] = 0xFf;
      data_pckt[19] = pckt_seq_num >> 16;
      data_pckt[20] = pckt_seq_num >> 8;
      data_pckt[21] = pckt_seq_num;
   //===========================Pay_Load=======================================
   //========================= 22 ---- 102 ====================================
     if(x==1)
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)                      
        {      
         data_pckt[ele_num] = CF_BYTE_READ(start_address + 81*(pckt_seq_num-1) + ele_num-22);
        }
     }
     
     if(x==0)
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)
        {      
         data_pckt[ele_num] = SF_BYTE_READ(start_address + 81*(pckt_seq_num-1) + ele_num-22);
        }
     }
     
     
   //===========================CRC Value======================================
   //======================== 103 ---- 104 ====================================
      int16 outdata = mk_CRC(data_pckt, 103);
      
      data_pckt[103] = outdata ;
      data_pckt[104] = outdata >> 8;
      
   //__________________________________________________________________________
   
      for(int8 ia = 0; ia<=104; ia++)
      {
       fputc(data_pckt[ia],tr_cp);
      } 
 
      delay_ms(220);
      
 } 
 
 return;
} 

//___________________________________________________________________________<<

Void PCKT_SET_SEND(unsigned int32 initial_address,unsigned int8 packet_set_size, unsigned int8 packet_set_num ,int1 FM_SELECT)
{


unsigned int32 next_size = packet_set_size * (packet_set_num - 1);               

unsigned int32 start_address = initial_address + 81*next_size;

 
for(unsigned int32 pckt_seq_num = 1;   pckt_seq_num <= packet_set_size;      pckt_seq_num ++)
{ 
   //=================Ground stationCall sign part=============================
   //========================= 0 --- 5 ========================================
      data_pckt[0] = 0x4a;     // J                                             
      data_pckt[1] = 0x47;     // G               
      data_pckt[2] = 0x36;     // 6
      data_pckt[3] = 0x59;     // Y
      data_pckt[4] = 0x42;     // B
      data_pckt[5] = 0x57;     // W
   //=========================Dest SSID========================================
   //=========================   6     ========================================
      data_pckt[6] = 0x30;     //char 0
   //===================Satellite Call Sign====================================
   //========================= 7 --- 12  ======================================
      data_pckt[7] = 0x4A;     // J
      data_pckt[8] = 0x47;     // G
      data_pckt[9] = 0x36;     // 6
      data_pckt[10] = 0x59;    // Y 
      data_pckt[11] = 0x4D;    // M
      data_pckt[12] = 0x58;    // X
   //========================Source SSID=======================================
   //=========================   13   =========================================
      data_pckt[13] = 0x30;    //char 0
   //==========================Control=========================================
   //=========================   14   =========================================
      data_pckt[14] = 0x3E;//
   //============================PID===========================================
   //=========================   15   =========================================
      data_pckt[15] = 0xF0;//
   //=====================Packet_specified_data================================
   //========================= 16 ---- 17 =====================================
      data_pckt[16] = 0xFF;
      data_pckt[17] = 0xF0;
      data_pckt[18] = 0xFf;
      data_pckt[19] = pckt_seq_num >> 16;
      data_pckt[20] = pckt_seq_num >> 8;
      data_pckt[21] = pckt_seq_num;
   //===========================Pay_Load=======================================
   //========================= 22 ---- 102 ====================================
     if(FM_SELECT==1)                                                            //com flash
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)
        {      
         data_pckt[ele_num] = CF_BYTE_READ(start_address + 81*(pckt_seq_num-1) + ele_num-22);
        }
     }
     
     else if(FM_SELECT==0)                                                       //shared flash
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)
        {      
         data_pckt[ele_num] = SF_BYTE_READ(start_address + 81*(pckt_seq_num-1) + ele_num-22);
        }
     }
     
     
   //===========================CRC Value======================================
   //======================== 103 ---- 104 ====================================
      int16 outdata = mk_CRC(data_pckt, 103);
      
      data_pckt[103] = outdata ;
      data_pckt[104] = outdata >> 8;
      
   //__________________________________________________________________________
   
      for(int8 ia = 0; ia<=104; ia++)
      {
       fputc(data_pckt[ia],tr_cp);
      } 
      delay_ms(200);
      
      
 } 
 
 return;
} 


//_____________________________________________________________________________________>>
//_________________Output a packet_______________________________________________________

Void Send_a_Packet(int1 x)
{
    unsigned int32 start_address = make32(cmd_Pckt[4],cmd_Pckt[5],cmd_Pckt[6],cmd_Pckt[7]);
    unsigned int32 pckt          = make32(cmd_Pckt[8],cmd_Pckt[9],cmd_Pckt[10],cmd_Pckt[11]);
    
   //=================Ground stationCall sign part=============================
   //========================= 0 --- 5 ========================================
      data_pckt[0] = 0x4a;     // J                                             
      data_pckt[1] = 0x47;     // G               
      data_pckt[2] = 0x36;     // 6
      data_pckt[3] = 0x59;     // Y
      data_pckt[4] = 0x42;     // B
      data_pckt[5] = 0x57;     // W
   //=========================Dest SSID========================================
   //=========================   6     ========================================
      data_pckt[6] = 0x30;     //char 0
   //===================Satellite Call Sign====================================
   //========================= 7 --- 12  ======================================
      data_pckt[7] = 0x4A;     // J
      data_pckt[8] = 0x47;     // G
      data_pckt[9] = 0x36;     // 6
      data_pckt[10] = 0x59;    // Y 
      data_pckt[11] = 0x4D;    // M
      data_pckt[12] = 0x58;    // X
   //========================Source SSID=======================================
   //=========================   13   =========================================
      data_pckt[13] = 0x30;    //char 0
   //==========================Control=========================================
   //=========================   14   =========================================
      data_pckt[14] = 0x3E;//
   //============================PID===========================================
   //=========================   15   =========================================
      data_pckt[15] = 0xF0;//
   //=====================Packet_specified_data================================
   //========================= 16 ---- 17 =====================================
      data_pckt[16] = 0xFF;
      data_pckt[17] = 0xF0;
      data_pckt[18] = 0xFf;
      data_pckt[19] = pckt >> 16;
      data_pckt[20] = pckt >> 8;
      data_pckt[21] = pckt;
   //===========================Pay_Load=======================================
   //========================= 22 ---- 102 ====================================     
     if(x==1)
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)
        {      
         data_pckt[ele_num] = CF_BYTE_READ(start_address + 81*(pckt-1) + ele_num-22);
        }
     }
     
     if(x==0)
     {
        for (int32 ele_num = 22; ele_num <= 102; ele_num++)
        {      
         data_pckt[ele_num] = SF_BYTE_READ(start_address + 81*(pckt-1) + ele_num-22);
        }
     }
     
   //===========================CRC Value======================================
   //======================== 103 ---- 104 ====================================
      int16 outdata = mk_CRC(data_pckt, 103);
      
      data_pckt[103] = outdata ;
      data_pckt[104] = outdata >> 8;
      
   //__________________________________________________________________________
   
      for(int8 ia = 0; ia<=104; ia++)
      {
       fputc(data_pckt[ia],tr_cp);
      } 
      delay_ms(200);
      
  return;    
 } 
//___________________________________________________________________________<<



//_____________________________________________________________________________________<<
//_________________________This function correct the receiving buffer____________________

unsigned int16 cr,pk;                                                            // this is the crc check variable                         
unsigned int8 sat_id;
void buffer_corection()
{
    
     if(buffer[0]==0x42)
     {
        for(int ih=0;ih<14;ih++)
        {
         CMD_pckt[ih] = buffer[ih]; 
        }
     next_in=0;
     buffer_ZERO();
     }
     
     if(buffer[1]==0x42)
     {
        for(int ih=0;ih<14;ih++)
        {
         CMD_pckt[ih] = buffer[ih+1]; 
        }
     next_in=0;
     buffer_ZERO();
     }
     
     if(buffer[2]==0x42)
     {
        for(int ih=0;ih<14;ih++)
        {
         CMD_pckt[ih] = buffer[ih+2]; 
        }
     next_in=0;
     buffer_ZERO();
     }
     
     if(buffer[3]==0x42)
     {
        for(int ih=0;ih<14;ih++)
        {
         CMD_pckt[ih] = buffer[ih+3]; 
        }
     next_in=0;
     buffer_ZERO();
     }
     
     if(buffer[4]==0x42)
     {
        for(int ih=0;ih<14;ih++)
        {
         CMD_pckt[ih] = buffer[ih+4]; 
        }
     next_in=0;
     buffer_ZERO();
     }
     
    cr = mk_crc(CMD_pckt,12);
    pk = make16(CMD_pckt[13],CMD_pckt[12]);
    sat_id = CMD_pckt[1]; 
    return;
}
//___________________________________________________________________________<<

 
 

//_____________________________________________________________________________________<<
//_________________CMD to obc____________________________________________________________

unsigned int8 COM_TO_MAIN[16] = {0xAA,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0xBB};

unsigned int8 MAIN_TO_COM[24] = {0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0};

int8 MBI = 0;
unsigned int32 num = 0;

void WAIT_FOR_MAIN_PIC_RESPONSE()
{
   for( ; num < 100000; num++)
   {
      if(kbhit(MP_CP))
      {
       MAIN_TO_COM[MBI] = fgetc(MP_CP);
       MBI++;
      }
   }MBI = 0; num = 0;
              
   return;
}

void MAKE_MAIN_PIC_BUFFR_ZERO()
{
   for(int p = 0; p<24; p++)
   {
      MAIN_TO_COM[p]=0x00;
   }
   
   return;
}

void COMUNICATRE_WITH_MAIN_PIC(int8 p , unsigned int8 time)
{
  
   if(p == 0)                                                                    //for com to main commands
   {
    COM_TO_MAIN[4]  = CMD_PCKT[4];
    COM_TO_MAIN[5]  = CMD_PCKT[5];
    COM_TO_MAIN[6]  = CMD_PCKT[6];
    COM_TO_MAIN[7]  = CMD_PCKT[7];
    COM_TO_MAIN[8]  = CMD_PCKT[8];
    COM_TO_MAIN[9]  = CMD_PCKT[9];
    COM_TO_MAIN[10] = CMD_PCKT[10];
    COM_TO_MAIN[11] = CMD_PCKT[11];
   }
   
   else if(p == 1)                                                               // CW ASK
   {
    COM_TO_MAIN[4]  = 0x50;
    COM_TO_MAIN[5]  = 0x00;
    COM_TO_MAIN[6]  = 0x00;
    COM_TO_MAIN[7]  = 0x00;
    COM_TO_MAIN[8]  = 0x00;
    COM_TO_MAIN[9]  = 0x00;
    COM_TO_MAIN[10] = 0x00;
    COM_TO_MAIN[11] = 0x00;
   }
   
   else if(p == 2)                                                               // get access
   {
    COM_TO_MAIN[4]  = 0x59;
    COM_TO_MAIN[5]  = 0x00;
    COM_TO_MAIN[6]  = time;
    COM_TO_MAIN[7]  = 0x00;
    COM_TO_MAIN[8]  = 0x00;
    COM_TO_MAIN[9]  = 0x00;
    COM_TO_MAIN[10] = 0x00;
    COM_TO_MAIN[11] = 0x00;
   }
   
   
   
   for(int8 ia = 0; ia<16; ia++)
   {
      fputc(COM_TO_MAIN[ia],MP_CP);
   }

   return;

}
//___________________________________________________________________________<<

//___________________________________________________________________________<<







//____________________This function create packet of rssi and temp of trx________________
Void SEND_COM_DATA()
{
//    Ground stationCall sign---------------[ 0-----5 ]
      Ack_pckt[0] = 0x4a;     // J
      Ack_pckt[1] = 0x47;     // G               
      Ack_pckt[2] = 0x36;     // 6
      Ack_pckt[3] = 0x59;     // Y
      Ack_pckt[4] = 0x42;     // B
      Ack_pckt[5] = 0x57;     // W
      
//    Dest SSID-----------------------------[ 6 ]
      Ack_pckt[6] = 0x30;     //char 0

//    Satellite Call Sign-------------------[ 7-----12 ]
      Ack_pckt[7] = 0x4A;     // J
      Ack_pckt[8] = 0x47;     // G
      Ack_pckt[9] = 0x36;     // 6
      Ack_pckt[10] = 0x59;    // Y
      Ack_pckt[11] = 0x4D;    // M
      Ack_pckt[12] = 0x58;    // X
      
//    Source SSID---------------------------[ 13 ]
      Ack_pckt[13] = 0x30;    //char 0
      
//    Control-------------------------------[ 14 ]
      Ack_pckt[14] = 0x3E;    //
      
//    PID-----------------------------------[ 15 ]
      Ack_pckt[15] = 0xF0;    //
      
//    Header--------------------------------[ 16 ]
      Ack_pckt[16] = 0xAA;    //

//    SAT Header----------------------------[ 17]
      Ack_pckt[17] = 0x03;    //   
      
//    Pckt sequence number higher byte------[ 18 ]
      Ack_pckt[18] = CMD_pckt[1];    // 

//    Pckt sequence number lower byte-------[ 19 ]
      Ack_pckt[19] = CMD_pckt[2];    // 
      
//    CMD back------------------------------[ 20 - 28 ]
      int16 tempr = TRX_TEMP_READ();                                             //read ADC channel 5
      
      unsigned int32 rssi64 = 0;
      
      for(int lk = 0; lk<100; lk++) 
      {
       rssi64  = rssi64 + TRX_RSSI_READ();                                       //read ADC channel 4
       delay_us(100);
      }
      
      int16 rssi = rssi64/100 ;

      Ack_pckt[20] = CMD_pckt[3];
      Ack_pckt[21] = 0xDD;
      Ack_pckt[22] = rssi>>8;
      Ack_pckt[23] = rssi;
      Ack_pckt[24] = 0xDD;
      Ack_pckt[25] = 0xDD;
      Ack_pckt[26] = tempr>>8;
      Ack_pckt[27] = tempr;
      Ack_pckt[28] = 0xDD;

//    Footer--------------------------------[ 29 ]
      Ack_pckt[29] = 0xAA;    //
      
//    CRC Calculation-----------------------[ 30 - 31 ]
      int16 outdata = mk_CRC(Ack_pckt, 30);
      
      Ack_pckt[30] = outdata ;
      Ack_pckt[31] = outdata >> 8;
      
//    Send pckt------------------------------
      OUTPUT_high(pin_d1);
      Delay_ms(2000);
        for(int ih=0;ih<32;ih++)                                                 //buffer print
        {
           fputc(Ack_pckt[ih],TR_CP); 
        }
      Delay_ms(500);
      OUTPUT_low(pin_d1);
      
      return;     
}


unsigned int16 RSSI_READ()                                                       //Reading satellite noise level (Received signal strength indication AN4)
{
 unsigned int32 RSSI = 0;

   for(int lk = 0; lk<20; lk++) 
      {
       RSSI  = RSSI + TRX_RSSI_READ();                                           //lectura analogica de AN9
       delay_us(10);
      }
      
  unsigned int16 RSSI1 = RSSI/20;                                                //averaging for better value
  
  return RSSI1 ;
}
