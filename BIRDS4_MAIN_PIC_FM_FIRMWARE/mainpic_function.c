//caution
//port of kbhit in CAMTurn_OFF_BC Mission is PC--
//TRANSFER_DATA_NBYTE_TOPC_SCF(65536,65536); is comment out--
//adcs address test make32 function--
//
//reset battery voltage 0x87, over 0x87 is nomal mode
//reset 
int8 MISSION_STATUS = 0;                                                         //MISSION STATUS FLAGS
static int16 currenttime = 0;
int16 a,n;
int16 safe;

void GIVE_ACCESS_SCF_Nsec(int8 min)
{
   fprintf(PC,"giving access to COM for %d min\r\n", min);
   fputc(0x35,COM);
   output_high(PIN_C4);
   if(min > 5)                                                                   //for accidentaly send the long time delay, maximum should be 5 min
   {
      min = 5;
   }
   for(int16 i = 0; i < min * 60; i++)
   {
      delay_ms(1000);                                                            //wait 1 sec
      if(in_bffr_main[4] == 0xab || CMD_FROM_PC[0] == 0xab)
      {
         CMD_FROM_PC[0] = 0;
         CMD_FROM_PC[1] = 0;
         PC_DATA = 0;
         break;
      }
   }
   fprintf(PC,"\r\nBack to Nomal Operation\r\n");
   return;
}

//--------MAIN PIC Buffer------------------------------------------------------
/*
void SerialDataReceive()                                                         //get buffer data one by one
{
   int num = 0;
   for(num = 0 ;num < 16; num++)
   {
    in_bffr_main[num] = fgetc(com);
   }     
   return;
}
*/
void DELETE_CMD_FROM_PC()
{
   for(int num = 0; num < 8; num++)
   {
      CMD_FROM_PC[num] = 0;
   }
   return;
}

void Delete_Buffer()                                                             //delete com command buffer
{
   int num = 0;
   for(num = 0;num < 16; num++)
   {
    in_bffr_main[num] = 0x00;
   }
   COM_DATA = 0;
   return;
}
/*
void Transfer_Buffer(int PORT_NUM)                                               //get buffer data one by one                                                                                
{                                                                                //1:PC 2:COM 3:RESET
   int num = 0;
   switch(PORT_NUM)
   {
      case 1:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],PC);
         }
         break;
      case 2:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],COM);
         }
         break;
      case 3:
         for(;num < 16; num++)
         {
         fputc(in_bffr_main[num],RESET);
         }
         break;
   }
   return;
}

void TRANSFER_Buffer_OF(unsigned INT32 TO_ADRESS)                                //for testing
{
   for (int i=0 ; i < 16 ; i++)
   {
      WRITE_DATA_BYTE_OF(TO_ADRESS, in_bffr_main[i]);
      TO_ADRESS++;
   }
   
   return;
}
*/
//--------BC Function----------------------------------------------------------

//unsigned int8 BC_temp_data[2] = {};
unsigned int16 BC_temp_data_h = 0;
unsigned int16 BC_temp_data_l = 0;
unsigned int16 BC_TEMP = 0;
float  temp = 0;
float initial_temp = 0;
float MAXTEMP = 0;
int16 UNLEG2 = 0;



void BC_SETUP()                                                                  //Analog read configuration (AN9)
{
   ANCON2= 0x01;                                                                 // PIN RC2 Analog enable
   ADCON1L = 0x00;                                                               // SAMP bit must be cleared by software to start conversion (ends sampling and starts converting)
   ADCHS0L = 0x09;                                                               // Connect AN9 as S/H+ input
                                                                                 // AN9 is the input
   ADCON2H = 0x00;                                                               //A/D control register 2H
   ADRC = 1;                                                                     //RC Clock source
   ADCSS0L = 0;                                                                  //Sample select register
   ADCON3L = 0x02;                                                               // Conversion clock select bits, Tad = 3.2/Fosc
   //setup_adc(ADC_CLOCK_DIV_32);                                                //VSS_VDD
   ADCON2L = 0;                                                                  //A/D control register 2L
   return;
}
/*
void BC_READ_TO_PC()
{
   ADON = 1;
   SAMP = 1;
   delay_ms(100);
   
   SAMP = 0; // start converting
   while (!DONE){};
   delay_ms(100);
   //BC_temp_data[0] = ADCBUF0H;//read_adc();
   //BC_temp_data[1] = ADCBUF0L;
   
   BC_temp_data_h = ADCBUF0H;
   BC_temp_data_l = ADCBUF0L;
   //temp = ((data_h << 8 ) | data_l);
   fprintf(PC,"data_l:%x \r\n", BC_temp_data_l);
   fprintf(PC,"data_h:%x \r\n", BC_temp_data_h);
   BC_TEMP = BC_temp_data_h | BC_temp_data_l;
   //fprintf(PC,"temp:%f\r\n",temp);
   temp = BC_TEMP/1024*3.31*100-50;    //VDD: 3.31V(MEASURED)
   //fprintf(PC,"%1.1f \r\n", temp);
   return;
}
*/
void CHECK_BC_TEMP()                                                             //reads BC and saves in MAXTEMP
{
   BC_SETUP();                                                                   //Analog read configuration (AN9)
   ADON = 1;
   SAMP = 1;
   delay_ms(10);
   
   SAMP = 0;                                                                     // start converting
   while (!DONE){};
   delay_ms(10);
   BC_temp_data_h = ADCBUF0H;                                                    //read_adc();
   BC_temp_data_l = ADCBUF0L;
   //fprintf(PC,"%x%x\r\n",ADCBUF0H,BC_temp_data_l);
   BC_temp = BC_temp_data_h<<8 | BC_temp_data_l;                                 //converts to int16
   //fprintf(PC,"HEX : %x%x\r\n",BC_temp>>8,BC_temp);

   temp = BC_temp;                                                               //converts to float
   //temp = temp/1024*3.25*100-50;
   temp = (temp*3.3/1023);
   temp = temp*100-50;
   fprintf(PC,"Current Temp: %1.1f\r\n",temp);
   
   if(MAXTEMP < temp)
   {
      MAXTEMP = temp;
   }
   fprintf(PC,"%f\r\n",temp);
   return;
}

void MEASURE_BC_TEMP()
{
   BC_SETUP();                                                                   //analog reading setup
   ADON = 1;
   SAMP = 1;
   delay_ms(1);
   
   SAMP = 0;                                                                     // start converting
   while (!DONE){};
   delay_ms(1);
   BC_temp_data_h = ADCBUF0H;                                                    //read_adc();
   BC_temp_data_l = ADCBUF0L;

   return;
}

void Turn_ON_BC()
{
   output_high(PIN_D5);                                                          //BC switch ON, RD5=1
   return;
}

void Turn_OFF_BC()
{
   output_low(PIN_D5);                                                           //BC switch OFF, RD5=0
   return;
}

void BC_OPERATION()                                                              //Turn ON BC and read temperature
{
   CHECK_BC_TEMP();                                                              //read BC temperature and save It in float MAXTEMP
   initial_temp = MAXTEMP;                                                       //save temperature in initial_temp before turn_ON BC
   //fprintf(PC,"INITIAL TEMPERATURE is %1.1f\r\n\r\n",initial_temp);
   Turn_ON_BC();                                                                 //BC switch ON, RD5=1
   fprintf(PC,"Turned ON BC\r\n");
   currenttime = 0;
   int8 counter = 0;   
   while(currenttime < 30)                                                       //turn ON BC for 30 sec
   {
      for(int i = 0; i < 10; i++)
      {
         COLLECT_RESET_DATA();                                                   //sends cmd asking RESET DATA
         if(reset_bffr[0] == 0x8e)
         {
            for(i = 0; i < 10; i++);                                             //shows received array
            {
               fprintf(PC,"%x,",reset_bffr[i]);
            }
            fprintf(PC,"%x\r\n",reset_bffr[10]);
            break;
         }
      }
      CHECK_BC_TEMP();                                                           //read BC temperature and save It in float MAXTEMP
      UNLEG2 = UNLEG2 + RESET_bffr[10];                                          //add the unleg2 current every second
      counter++;
      delay_ms(900);
   }
      
   Turn_OFF_BC();                                                                //BC switch OFF, RD5=0
   fprintf(PC,"Turned OFF BC\r\n");
   delay_ms(500);
   CHECK_BC_TEMP();                                                              //reads BC temperature and saves in float MAXTEMP
   if(MAXTEMP-initial_temp > 5)                                                  //if temperature increase, flag in HIGH
   {
      ANT_DEP_STATUS = 1;
   }
   fprintf(PC,"MAXIMUM TEMPERATURE is %1.1f\r\n\r\n",MAXTEMP);
   MAXTEMP = 0;
   initial_temp = 0;
   return;
}

void CLEAR_BC_FLAG()
{
   BC_ATTEMPT_FLAG = 0;
   fprintf(PC,"\r\nBC Attempt Flag clear done\r\n");
   return;
}

void MAKE_BC_FLAG_1()
{
   BC_ATTEMPT_FLAG = 1;
   fprintf(PC,"\r\nBC Attempt Flag:1\r\n");
   return;
}

void MAKE_BC_FLAG_2()
{
   BC_ATTEMPT_FLAG = 2;
   fprintf(PC,"\r\nBC Attempt Flag:2\r\n");
   return;
}

void MAKE_BC_FLAG_3()
{
   BC_ATTEMPT_FLAG = 3;
   fprintf(PC,"\r\nBC Attempt Flag:3\r\n");
   return;
}

void MAKE_BC_FLAG_4()
{
   BC_ATTEMPT_FLAG = 4;
   fprintf(PC,"\r\nBC Attempt Flag:4\r\n");
   return;
}

void Antenna_Deploy()
{
   fprintf(PC,"Ant Dep Attempt No: %x\r\n",BC_ATTEMPT_FLAG);
   
   if(BC_ATTEMPT_FLAG < 4 && BC_ATTEMPT_FLAG != 0)                               //repeats three times, independently the success or not
   {
      //SAVE_SAT_LOG(0xBC,BC_ATTEMPT_FLAG,BC_ATTEMPT_FLAG);
      fprintf(PC,"BC command sent to RESET PIC\r\n");
      for(int num = 0; num < 50; num++)
      {
         fputc(0xBC,reset);
         delay_ms(10);
      }
      delay_ms(1000);
      if(reset_bffr[0] == 0xCB)                                                  //check the headear of reset respond
      {
         RESET_DATA = 0;
         BC_OPERATION();                                                         //Turn ON BC for 30s and read temperature
         BC_ATTEMPT_FLAG++;                                                      //increase attempt Flag
         STORE_FLAG_INFO();                                                      //save flag data to flash memory
         WRITE_FLAG_to_EEPROM();                                                 //saves flags in EEPROM from address 0x18000 (75%)
         STORE_ADRESS_DATA_TO_FLASH();                                           //saves address data in a new sector if maximun number of R/W is achieved
         reset_bffr[0] = 0;
         CMD_FROM_PC[1] = 0;
         RESET_DATA = 0;
         delay_ms(1000);
         delay_ms(20000);                                                        //wait until RESET goes back to nomal loop
         SAVE_SAT_LOG(0xBC,BC_ATTEMPT_FLAG,BC_ATTEMPT_FLAG);
      }
   }
   return;
}


//--------FAB HK collection----------------------------------------------------
#define HK_size 124                                                              //HK FORMAT ARRAY SIZE
#define CW_size 5                                                                //CW FORMAT ARRAY SIZE
#define HIGH_SAMP_HK_size 124                                                    //High Sampling HK FORMAT ARRAY SIZE
#define FAB_SENSOR_size 45                                                       //HK FAB Part

static unsigned int8 CW_FORMAT[CW_size] = {};
unsigned int8 in_HK[FAB_SENSOR_size] = {};
unsigned int8 HKDATA[HK_size] ={};
unsigned int8 ACK_for_COM[24] = {0xAA,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0xBB};
//int8 in_High_HK[HIGH_SAMP_HK_size] = {};
BYTE FAB_DATA = 0;
//int t;
static int8 FAB_MEASUERING_FLAG= 0;
static int8 HIGH_SAMP_FAB_MEASUERING_FLAG = 0;
int32 FAB_FLAG = 0;
int8 CHECK_FAB_RESPONSE = 0;

#define buffer_from_FAB  (in_bffr_main[0]==0x33)

void REFLESH_CW_ACK_for_COM()                                                    //clear the array ACK_for_COM[7]
{
   for(int i = 1; i < 7; i++)
   {
      ACK_for_COM[i] = 0x00;
   }
   return;
}

void REFLESH_MSN_ACK_for_COM()                                                   //clear array ACK_for_COM[i] from position 12 to 22
{
   for(int i = 12; i < 23; i++)
   {
      ACK_for_COM[i] = 0x00;
   }
   return;
}

void CW_RESPOND()                                                                //writes ACK_for_COM[] with CW_FORMAT[] data and sends to COM PIC
{
   REFLESH_CW_ACK_for_COM();                                                     //writes 0x00 in array ACK_for_COM[] from pos 1 to 6
   delay_ms(100);
   ACK_for_COM[0] = 0xAA;
   ACK_for_COM[1] = 0x50;
   ACK_for_COM[2] = CW_FORMAT[0];
   ACK_for_COM[3] = CW_FORMAT[1];
   ACK_for_COM[4] = CW_FORMAT[2];
   ACK_for_COM[5] = CW_FORMAT[3];
   ACK_for_COM[6] = CW_FORMAT[4];
   ACK_for_COM[23] = 0xBB;
   for(int a = 0; a < 24; a++)                                                   //send the latest CW data to COM PIC
   {
      fputc(ACK_for_COM[a],COM);
   }
   MBP_SYNC();                                                                   //Send time data to MBP for syncronization
   fprintf(PC,"\r\nCW ANS:");
   for(a = 0; a < 23; a++)                                                       //send the latest CW data to PC
   {
      fprintf(PC,"%x,",ACK_for_COM[a]);
   }
   fprintf(PC,"%x\r\n",ACK_for_COM[23]);
   
//!   for(int k = 14; k < 23; k++)                                               //clear RSV part
//!   {
//!      ACK_for_COM[k] = 0x00;
//!   }
   return;
}

void CHECK_50_and_CW_RESPOND()                                                   //check 0x50 from COM PIC and sends CW to COM
{
   if(in_bffr_main[4] == 0x50)
   {
      CW_RESPOND();                                                              //writes ACK_for_COM[] with CW_FORMAT[] data and sends to COM PIC
      Delete_Buffer();                                                           //delete in_bffr_main[] (com command buffer)
      COM_DATA = 0;                                                              //flag to cero
   }
   return;
}

//--------ADCS MISSION---------------------------------------------------------
//////////////////////////////////////////////////
//ADCS MISSION ID: 0x45                         //
//ADCS MISSION DATA COLLECT: 55                 //
//ADCS SENSOR SAMPLING: 0x34                    //  
//ADCS ACK: 0x55                                //
//ADCS Finish Storing: 0x67                     //
//////////////////////////////////////////////////
#define TLE_size 35                                                              //TLE FORMAT ARRAY SIZE
#define GAIN_size 25                                                              //GAIN ARRAY SIZE
#define UTC_size 12                                                              //UTC time plus reset time
#define ADCScheck_size  72                                                        //ADCS data check
unsigned int8 TLE[TLE_size] = {0};                                               //TLE data array
unsigned int8 GAIN_data[GAIN_size] = {0};                               
unsigned int8 UTC_data[UTC_size] = {0};

void Turn_On_ADCS()
{
   output_high(PIN_D6);                                                          //ADCS switch ON, RD6=1
   return;
}

void Turn_Off_ADCS()
{
   output_low(PIN_D6);                                                           //ADCS switch OFF, RD6=0
   return;
}
/*
void Send_Command_to_ADCS(int8 data)                                             //testing for EM integration
{
   fputc(data,ADCS);
   return;
}
*/
void DELETE_ADCS_SENSOR()
{
   for(int a = 0; a < ADCS_SENSOR_SIZE; a++)
   {
      ADCS_SENSOR_DATA[a] = 0;
   }
   return;
}

void ADCS_SETTINGS()                                                             //initiallize
{
   output_high(PIN_C4);                                                          //flash COM side
   Delete_Buffer();                                                              //delete in_bffr_main[] (com command buffer)
   DELETE_ADCS_SENSOR();                                                         //delet ADCS_SENSOR_DATA[] array
   return;
}

void CHECK_ACK_FROM_ADCS_10k()                                                     //Wait ACK from DC for 10s
{
   for(int32 num = 0; num < 1000000; num++) 
   {
      if(kbhit(ADCS))                                                             
      {
         ADCS_ACK = fgetc(ADCS);                                                    //PC   
         break;
      }
      //delay_ms(1);
   }
   return;
}
/*
void DISPLAY_ADCS_SENSOR_TO_PC()//for testing
{
   for(int b = 0; b < ADCS_SENSOR_SIZE; b++)//for checking
   {
      fputc(ADCS_SENSOR_DATA[b],PC);
   }
   return;
}

void Get_ADCS_MISSION_Data()
{
   
   Delete_ADCS_SENSOR(); //initialize
   CHECK_50_and_CW_RESPOND();
   fputc(0x55,ADCS);
   fputc(0x55,PC);
   int i = 0;
   for(int32 num = 0; num < 1000000 ;num++)
   {
      if(kbhit(ADCS) && fgetc(ADCS) == 0x67)
      {
         for(num = 0; num < 10000; num++)
         {
            if(Kbhit(ADCS))
            {
               ADCS_SENSOR_DATA[i] = fgetc(ADCS);
               i++;
               if(i == 60)
               {
                  break;
               }
            }
         }
         ADCS_ACK = 0x67;
         break;
      }
   }
   if(num == 1000000)
   {
      fprintf(PC,"NO DATA\r\n");
   }
   CHECK_50_and_CW_RESPOND();
   return;
}

int8 compare_before_after(int32 beginning, int32 end)
{
   int8 data[12] = {};
   for(int i = 0; i < 6; i++)
   {
      data[i] = READ_DATA_BYTE_OF(beginning+222+10+i);
   }
   for(i = 0; i < 6; i++)
   {
      data[i+6] = READ_DATA_BYTE_OF(end-101+i);
   }
   signed int16 Bx = make16(data[0],data[1]);
   signed int16 By = make16(data[2],data[3]);
   signed int16 Bz = make16(data[4],data[5]);
   
   signed int16 Ax = make16(data[6],data[7]);
   signed int16 Ay = make16(data[8],data[9]);
   signed int16 Az = make16(data[10],data[11]);
   
   signed int16 Total_B = Bx + By + Bz;
   signed int16 Total_A = Ax + Ay + Az;
   Total_B = abs(Total_B);
   Total_A = abs(Total_A);
   fprintf(PC,"%lx,%lx,%lx\r\n",Bx,By,Bz);
   fprintf(PC,"%lx,%lx,%lx\r\n",Ax,Ay,Az);
   fprintf(PC,"%lud,%lud\r\n",Total_B,Total_A);
   if(Total_B >= Total_A)
   {
      return 1;//declease --> continue
   }else{
      return 0;//inclease --> stop
   }
}
*/
void GET_ADCS_SENSOR_DATA()                                                      //after that, method will changed (ADCS make format and just send to MAIN PIC)
{
   CHECK_50_and_CW_RESPOND();
   fputc(0x34,adcs);                                                             //send command (just collect sensor)
   //delay_ms(700);
   ADCS_ACK = 0;
   for(int32 num = 0; num < 1000000; num++)
   {
      if(kbhit(ADCS))
      {
         ADCS_ACK = fgetc(ADCS);
         break;
      }
      
   }
   int8 counter = 0;
   if(ADCS_ACK == 0x55)                                                          //acknowledge
   {
     fprintf(PC,"\r\nADCS ACK received\r\n");
     for(int a = 0; a < 5; a++)                                                  //send time data to ADCS
      {       
         delay_ms(100);
         fputc(HKDATA[a + 2],adcs);
         fprintf(PC,",%x",HKDATA[a + 2]);                                         //debug
      } 
      fprintf(PC,"\r\nTime data sent to ADCS\r\n");
      for(int32 num = 0; num < 1000000; num++)
      {
         if(kbhit(ADCS))
         {
            ADCS_SENSOR_DATA[counter] = fgetc(ADCS);
            counter++;
            if(counter == 61)
            {
               break;
            }
         }
      }
   }
   CHECK_50_and_CW_RESPOND();
   if(ADCS_SENSOR_DATA[0] == 0x57)
   {
      fprintf(PC,"\r\nADCS DATA received\r\n");   
   }else{
      fprintf(PC,"\r\nADCS DATA not received\r\n");
      ADCS_ACK_COMMING = 0;
      ADCS_ACK = 0;
   }
   return;
}

/////////TLE JOIN PART/////////////
void ADCS_TLE_JOIN(int8 seq,int8 data1,int8 data2,int8 data3,int8 data4,int8 data5)  //join and save TLE data
{
int pos=0;   
   switch(seq)
   {
      case 0x01:
         pos=0; 
         break;
      case 0x02:
         pos=5; 
         break;
      case 0x03:
         pos=10; 
         break;
      case 0x04:
         pos=15; 
         break;
      case 0x05:
         pos=20; 
         break;
      case 0x06:
         pos=25; 
         break;
      case 0x07:
         pos=30; 
         break;
   }

   output_low(PIN_C4);
   output_low(PIN_A5);
   delay_ms(100);
   for(int8 d = 0; d < TLE_size; d++)                                            //clear TLE array to receive data
   {
      TLE[d] = 0;
   }
   for(int c = 0; c < TLE_size; c++)                                             // Read TLE data from flash and put in TLE[] array
      {
         TLE[c]=READ_DATA_BYTE_OF(0x06650000+c);
      }
      SUBSECTOR_4KB_ERASE_OF(0x06650000);                                        // Clear flash memory TO SAVE NEW DATA
      SUBSECTOR_4KB_ERASE_SCF(0x06650000);
      SUBSECTOR_4KB_ERASE_SMF(0x06650000);
      delay_ms(100);
      TLE[pos] = data1;                                                          //update TLE[] array data
      TLE[pos+1] = data2;                                                      
      TLE[pos+2] = data3;                                                      
      TLE[pos+3] = data4;                                                      
      TLE[pos+4] = data5;                                                   
      
   for(int i = 0; i < TLE_size; i++)                                             // save everything to flash memory
      {
         WRITE_DATA_BYTE_OF(0x06650000 + i,TLE[i]);
         WRITE_DATA_BYTE_SCF(0x06650000 + i,TLE[i]);
         WRITE_DATA_BYTE_SMF(0x06650000 + i,TLE[i]);
      }

   fprintf(PC,"\r\nTLE:\r\n");
   for(int p = 0; p < TLE_size; p++)                                             // read from flash and print TLE data
      {
         fprintf(PC,"%x,",READ_DATA_BYTE_SMF(0x06650000+p));
      }
      fprintf(PC,"\r\n");
   
   output_high(PIN_C4);
   return;

}
/////////GAIN JOIN PART/////////////
void ADCS_GAIN_JOIN(int8 seq,int8 data1,int8 data2,int8 data3,int8 data4,int8 data5)  //join and save GAIN data
{
int pos=0;   
   switch(seq)
   {
      case 0x01:
         pos=0; 
         break;
      case 0x02:
         pos=5; 
         break;
      case 0x03:
         pos=10; 
         break;
      case 0x04:
         pos=15; 
         break;
      case 0x05:
         pos=20; 
         break;
   }

   output_low(PIN_C4);
   output_low(PIN_A5);
   delay_ms(100);
   for(int8 d = 0; d < GAIN_size; d++)                                            //clear GAIN array to receive data
   {
      GAIN_data[d] = 0;
   }
   for(int c = 0; c < GAIN_size; c++)                                             // Read GAIN data from flash and put in GAIN_data[] array
      {
         GAIN_data[c]=READ_DATA_BYTE_OF(0x06652000+c);
      }
      SUBSECTOR_4KB_ERASE_OF(0x06652000);                                        // Clear flash memory TO SAVE NEW DATA
      SUBSECTOR_4KB_ERASE_SCF(0x06652000);
      SUBSECTOR_4KB_ERASE_SMF(0x06652000);
      delay_ms(100);
      GAIN_data[pos] = data1;                                                          //update GAIN_data[] array data
      GAIN_data[pos+1] = data2;                                                      
      GAIN_data[pos+2] = data3;                                                      
      GAIN_data[pos+3] = data4;                                                      
      GAIN_data[pos+4] = data5;                                                   
      
   for(int i = 0; i < GAIN_size; i++)                                             // save everything to flash memory
      {
         WRITE_DATA_BYTE_OF(0x06652000 + i,GAIN_data[i]);
         WRITE_DATA_BYTE_SCF(0x06652000 + i,GAIN_data[i]);
         WRITE_DATA_BYTE_SMF(0x06652000 + i,GAIN_data[i]);
      }

   fprintf(PC,"\r\nGAIN:\r\n");
   for(int p = 0; p < GAIN_size; p++)                                             // read from flash and print GAIN data
      {
         fprintf(PC,"%x,",READ_DATA_BYTE_SMF(0x06652000+p));
      }
      fprintf(PC,"\r\n");
   
   output_high(PIN_C4);
   return;

}


void Delete_in_HK()                                                              //Borra el array in_HK[118]
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      in_HK[num] = 0;
   }
   return;
}
//////////////////////////////////////
void Delete_HKDATA()                                                             //erases HKDATA[118]
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      HKDATA[num] = 0;
   }
   return;
}

void Delete_CWFORMAT()                                                           //erases CW_FORMAT[5]
{
   for(int num = 0; num < CW_size; num++)
   {
      CW_FORMAT[num] = 0;
   }
   return;
}

int8 CHECK_SUNSHINE(int8 current)
{
   if(current > EX_PANEL_THRESHHOLD)
   {
      return 1;
   }else{
      return 0;
   }
}

int8 CONVERT_16bit_GYRO_to_8bit_X(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for X
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.11853;
   /*fprintf(PC,"ang:%.4f\r\n",ang_v);*/
   return (int8)ang_v; 
}

int8 CONVERT_16bit_GYRO_to_8bit_Y(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for Y
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.24552;
   /*fprintf(PC,"ang:%.4f\r\n",ang_v);*/
   return (int8)ang_v; 
}

int8 CONVERT_16bit_GYRO_to_8bit_Z(int8 HIGH_8bit, int8 LOW_8bit)//caribrated for Z
{
   signed int16 row_data = make16(HIGH_8bit, LOW_8bit);
   float ang_v = row_data*0.00875-0.40486;
   /*fprintf(PC,"ang:%.4f\r\n",ang_v);*/
   return (int8)ang_v; 
}

int8 OPERATION_MODE_VALUE(int8 Bat_Vol)
{//0x94HKDATA[116] 
   if(Bat_Vol > 0x94){//more than 3.8
      OPERATION_MODE = 0x11;
      return 96;
   }else if((Bat_Vol > 0x88) && (Bat_Vol <= 0x94)){//3.5 to 3.8
      OPERATION_MODE = 0x10;
      return 64;
   }else if(Bat_Vol <= 0x88){//less than 3.5
      OPERATION_MODE = 0x00;
      return 16;
   }
}

void MAKE_CW1_FORMAT()                                                           
{
   Delete_CWFORMAT();                                                            //erases CW_FORMAT[5]
   
   CW_FORMAT[0] = HKDATA[44];                                                    //battery voltage
   CW_FORMAT[1] = HKDATA[45]<<4|HKDATA[46]>>4;                                   //battery current
   CW_FORMAT[2] = HKDATA[47];                                                    //battery temp
   
//   CW_FORMAT[3] = CW_FORMAT[3];//0:CW1
//   CW_FORMAT[3] = CW_FORMAT[3] + RESERVE_CHECK * 64;
   CW_FORMAT[3] = CW_FORMAT[3] + OPERATION_MODE_VALUE(HKDATA[116]);              //96(=64+32-->0x01100000):nomal, 64(0x01000000):low, 16(0x00100000):safe
   KILL_FLAG_FAB = (HKDATA[49] & 0x10)>>4;
   KILL_FLAG_MAIN = HKDATA[49] & 0x01;
   CW_FORMAT[3] = CW_FORMAT[3] + KILL_FLAG_MAIN * 16;
   CW_FORMAT[3] = CW_FORMAT[3] + KILL_FLAG_FAB * 8;
   CW_FORMAT[3] = CW_FORMAT[3] + ANT_DEP_STATUS * 4;
   CW_FORMAT[3] = CW_FORMAT[3] + CHECK_SUNSHINE(HKDATA[34]) * 2;                 //+Y Panel (new axis definition)
   CW_FORMAT[3] = CW_FORMAT[3] + CHECK_SUNSHINE(HKDATA[35]) * 1;                 //+X Panel (new axis definition)
   
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[36]) * 128;               //-Z Panel (new axis definition)
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[37]) * 64;                //-X Panel (new axis definition)
   CW_FORMAT[4] = CW_FORMAT[4] + CHECK_SUNSHINE(HKDATA[38]) * 32;                //+Z Panel (new axis definition)
   CW_FORMAT[4] = CW_FORMAT[4] + (HKDATA[4] & 0b00011111);                       //time data
   
   CW_IDENTIFIER = 0;
   CHECK_50_and_CW_RESPOND();
   ACK_for_COM[0] = 0xAA;                                                        //for safety (this byte should be always 0)
   ACK_for_COM[23] = 0xBB;                                                       //for safety (this byte should be always 0)
   return;
}
void MAKE_CW2_FORMAT()                                                           
{
   Delete_CWFORMAT();
   //CHECK_50_and_CW_RESPOND();
   CW_FORMAT[0] = CONVERT_16bit_GYRO_to_8bit_X(HKDATA[59], HKDATA[60]);          //GYRO X axis
   CW_FORMAT[1] = CONVERT_16bit_GYRO_to_8bit_Y(HKDATA[61], HKDATA[62]);          //GYRO Y axis
   CW_FORMAT[2] = CONVERT_16bit_GYRO_to_8bit_Z(HKDATA[63], HKDATA[64]);          //GYRO Z axis
   
   CW_FORMAT[3] = CW_FORMAT[3] + 128;                                            //1:CW2
   CW_FORMAT[3] = CW_FORMAT[3] + FIRST_HSSC_DONE * 64;                           //High Sampling Sensor Collection Flag 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_CAM_DONE * 32;                             //AUTO CAM MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_MBP_DONE * 16;                             //AUTO MBP MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + AUTO_ADCS_DONE * 8;                             //AUTO ADCS MISSION DONE 0:not done, 1:done
   CW_FORMAT[3] = CW_FORMAT[3] + HKDATA[48] * 4;                                 //Heater 0:OFF, 1:ON
   CW_FORMAT[3] = CW_FORMAT[3] + RESERVE_CHECK * 2;                              //RSV Flag
   CW_FORMAT[3] = CW_FORMAT[3] + UPLINK_SUCCESS;                                 //UPLINK SUCCESS
   
   CW_FORMAT[4] = MISSION_STATUS;                                                //MISSION STATUS FLAGS

   CW_IDENTIFIER = 1;
   CHECK_50_and_CW_RESPOND();
   ACK_for_COM[0] = 0xAA;                                                        //for safety (this byte should be always 0)
   ACK_for_COM[23] = 0xBB;                                                       //for safety (this byte should be always 0)
   
   return;
}

void MAKE_CW_FORMAT()
{
   if(CW_IDENTIFIER)
   {
      MAKE_CW1_FORMAT();
   }else{
      MAKE_CW2_FORMAT();
   }
}

void Send_COMMAND_TO_FAB(int8 cmd)                                               //sends cmnds to FAB
{
   fputc(cmd,FAB);                                                               
   return;
}

void COMMAND_TO_FAB(int32 delaytime)                                             //ask for data to FAB
{
   FAB_DATA = 0;
   enable_interrupts(INT_rda3);                                                  //FAB UART Interrupt enabled to receives data for in_HK[]
   Send_Command_to_FAB(0x61);                                                    //send command to fab
   /*fprintf(PC,"Command to FAB sent\r\n");*/ 
   int32 a = 0;
   while(FAB_DATA == 0){                                                         
      a++;                                                                       
      if(a > 50000)
      {
         break;
      }
   }
   waiting(delaytime);                                                           //funcion de espera
   disable_interrupts(INT_rda3);                                                 //disabled FAB UART interrupt
   return;
}
/*
int8 ACK_FROM_FAB()
{
   int8 ack;
   ack = fgetc(CAM);
   return ack;
}









*/
void SAVE_HKDATA_TO_SCF(unsigned int32 Memory_Adress)
{
   output_low(PIN_C4);
   for(int8 num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_SCF(Memory_Adress+num, HKDATA[num]);
   }
   output_high(PIN_C4);
   return;
}

void Send_HKDATA_to_SCF(int32 adress)
{
   Save_HKDATA_to_SCF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_HKDATA_TO_SMF(unsigned int32 Memory_Adress)
{
   output_low(PIN_A5);
   for(int8 num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_SMF(Memory_Adress+num, HKDATA[num]);
   }
   return;
}
void Send_HKDATA_to_SMF(int32 adress)
{
   Save_HKDATA_to_SMF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_HKDATA_TO_OF(unsigned int32 Memory_Adress)
{
   int num;
   for(num = 0; num < HK_size; num++)
   {
      WRITE_DATA_BYTE_OF(Memory_Adress+num, HKDATA[num]);
   }
   return;
}
void Send_HKDATA_to_OF(int32 adress)
{
   Save_HKDATA_to_OF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}








void SAVE_CWFORMAT_TO_SCF(unsigned int32 Memory_Adress)
{
   output_low(PIN_C4);
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_SCF(Memory_Adress+num, CW_FORMAT[num]);
   }
   output_high(PIN_C4);
   return;
}
void SEND_CWFORMAT_TO_SCF(int32 adress)
{
   SAVE_CWFORMAT_TO_SCF(adress);                                                 //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   return;
}

void SAVE_CWFORMAT_TO_SMF(unsigned int32 Memory_Adress)
{
   output_low(PIN_A5);
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_SMF(Memory_Adress+num, CW_FORMAT[num]);
   }
   return;
}
void SEND_CWFORMAT_TO_SMF(int32 adress)
{
   SAVE_CWFORMAT_TO_SMF(adress);  //save HK to COM PIC
   CHECK_50_and_CW_RESPOND();
   fprintf(PC,"\r\nCW SAVED\r\n");
   return;
}

void SAVE_CWFORMAT_TO_OF(unsigned int32 Memory_Adress)
{
   for(int8 num = 0; num < CW_size; num++)
   {
      WRITE_DATA_BYTE_OF(Memory_Adress+num, CW_FORMAT[num]);
   }
   return;
}
void SEND_CWFORMAT_TO_OF(int32 adress)
{
   SAVE_CWFORMAT_TO_SCF(adress);                                                 //save HK to COM PIC
   TRANSFER_DATA_NBYTE_TOPC_OF(adress,CW_size);                                  //for checking whether the data saved correctly
   CHECK_50_and_CW_RESPOND();
   return;
}








void CHECK_HKDATA(int8 in,int32 delaytime)                                       //writes HKDATA[] array
{
   fprintf(PC,"GET FAB SENSOR DATA\r\n");
   Delete_HKDATA();                                                              
   waiting(delaytime);                                                           
   CHECK_50_and_CW_RESPOND();
   for(int num = 1; num < 11 - in; num++)                                        //[FAB] +X,-Y,-Z,+Y,-Xtemp_high,low(array[10] to [17])
   {
      HKDATA[num + 5+4] = in_HK[num + 2 - in];                                   //places the data sent by the FAB in the HKDATA [] array from position 10 to 17
     /* fputc(HKDATA[num + 5+4],PC); */                                          
      fprintf(PC,"%x,",HKDATA[num + 5+4]);
   }
   
   MEASURE_BC_TEMP();                                                            //BC analog reading and BC_temp_data_h and BC_temp_data_l writing
   CHECK_50_and_CW_RESPOND();   
   HKDATA[14+4] = BC_temp_data_h;                                                //+X temp high[18]
   HKDATA[15+4] = BC_temp_data_l;                                                //+X temp low[19]
   
   for(num = 9; num < FAB_SENSOR_size - 2; num++)                                //[FAB] from BPB temp to Kill status(array[20] to [49])
   {
      HKDATA[num + 7+4] = in_HK[num + 2 - in];
      /*fputc(HKDATA[num + 7+4],PC);*/                                           
      fprintf(PC,"%x,",HKDATA[num + 7+4]);
   }
   FAB_DATA = 0;                                                                 //flag to zero
   return;
}

void VERIFY_FABDATA(int32 delaytime1,int32 delaytime2)
{
   for(int8 num = 0; num < 3; num++)
   {
      COMMAND_TO_FAB(delaytime1);                                                //sends cmd to FAB and writes in_HK[] array
      CHECK_50_and_CW_RESPOND();
      //FAB_DATA = 0;
      if(in_HK[0] == 0x33)                                                       //gather sensor data by interrupt
      {
         CHECK_HKDATA(2,delaytime2);                                             //writes the array HKDATA[118] 
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[1] == 0x33){
         //delay_ms(200);
         CHECK_HKDATA(1,delaytime2);
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[2] == 0x33){
         //delay_ms(200);
         CHECK_HKDATA(0,delaytime2);
         CHECK_50_and_CW_RESPOND();
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }
   }
   return;
}




void GET_RESET_DATA()                                                            //writes the array HKDATA with Reset PIC data
{
   RESET_DATA = 0;
   for(int i = 0; i < 6; i++)
   {
      COLLECT_RESET_DATA();
      if(reset_bffr[0] == 0x8e)
      {
         break;
      }
   }   
   CHECK_50_and_CW_RESPOND();
   if(RESET_bffr[0] == 0x8e)                                                     //check header
   {
      fprintf(PC,"\r\nRESET DATA OBTAINED\r\n");
      for(int num = 0; num < 5; num++)                                           //write HKDATA array with timedata positions 2 to 6
      {
         HKDATA[num + 2] = reset_bffr[num + 1];
         /*
         fputc(HKDATA[num + 2],PC);
         */
         fprintf(PC,"%x,",HKDATA[num + 2]);                                      
      }
   
      for(num = 0; num < 5; num++)                                               //writes HKDATA with reset sensor data in positions 110 to 114
      {
         HKDATA[num + 116] = reset_bffr[num + 6];                                
         /*
         fputc(HKDATA[num + 116],PC);
         */
         fprintf(PC,"%x,",HKDATA[num + 116]);                                    
      }
   }else{
      fprintf(PC,"\r\nRESET DATA NO OBTAINED\r\n");
   }
   //Delete_Reset();
   return;
}

void MAKE_ADCS_HKDATA()                                                          //carga en el array HKDATA[] los datos del ADCS en las posiciones 53 al 106
{
   GET_ADCS_SENSOR_DATA();                                                       //funcion que carga el array ADCS_SENSOR_DATA[] con los datos del ADCS
   CHECK_50_and_CW_RESPOND();   
   for(int num = 53; num < 113; num++)                                           //12byte(MAG6,GYRO6)+48byte(GPS) = 60
   {
      HKDATA[num] = ADCS_SENSOR_DATA[num - 52];                                  //ADCS[1] to ADCS[60]
   }
   for(int n = 1; n < 61; n++)                                                   // SHOW IN SERIAL MONITOR 12byte(MAG6,GYRO6)+48byte(GPS) = 60
   {
      fprintf(PC,"%x,",ADCS_SENSOR_DATA[n]);                                    //ADCS[1] to ADCS[60]
                                
   }
   
//!   for(num = 65; num < 107; num++)                                            //60byte(GPS)
//!   {
//!      HKDATA[num] = ADCS_SENSOR_DATA[num - 34];//HKDATA[54] = ADCS_SENSOR_DATA[21]-->ADCS[21] to ADCS[62]
//!   }
   return;
}

void DISPLAY_CW()                                                                //funcion que imprime el array CW_FORMAT[]
{
   fprintf(PC,"\r\nCW:\r\n");
   for(int8 i = 0; i < 5; i++)
   {
      fprintf(PC,"%x",CW_FORMAT[i]);
   }
   fprintf(PC,"\r\n");
   CHECK_50_and_CW_RESPOND();   
   return;
}

void SET_IDENTIFIER()
{
   CHECK_50_and_CW_RESPOND();
   HKDATA[0] = 0x33;
   HKDATA[1] = 0x33;
   HKDATA[7] = 0xAA;
   HKDATA[8] = 0xAA;
   HKDATA[9] = 0xAA;
   HKDATA[50] = 0xBB;
   HKDATA[51] = 0xBB;
   HKDATA[52] = 0xBB;
   HKDATA[113] = 0xCC;
   HKDATA[114] = 0xCC;
   HKDATA[115] = 0xCC;
   HKDATA[122] = 0x44;
   HKDATA[123] = 0x44;
   return;
}

void FAB_TEST_OPERATION()
{
   Delete_in_HK();                                                               //clear the array in_HK[124]
   LOOP_FAB_HK_ADDRESS();                                                        //Rotates the HK data saving positions if it reaches the end of the allocated space 
   LOOP_FAB_CW_ADDRESS();                                                        //Rotates the CW data saving positions if it reaches the end of the allocated space
   CHECK_50_and_CW_RESPOND();                                                    //check if command 0x50 arrived from COM PIC and send CW to COM
   LOOP_FAB_HK_ADDRESS();                                                        //Rotates the HK data saving positions if it reaches the end of the allocated space
   CHECK_50_and_CW_RESPOND();                                                    //check if command 0x50 arrived from COM PIC and send CW to COM
   LOOP_FAB_CW_ADDRESS();                                                        //Rotates the CW data saving positions if it reaches the end of the allocated space
   CHECK_50_and_CW_RESPOND();                                                    //check if command 0x50 arrived from COM PIC and send CW to COM
   Turn_ON_ADCS();                                                               //turn ON ADCS, RD6 = HIGH                                                               
   //fprintf(PC,"FAB communication start\r\n");
   waiting(200000);                                                              //wait function, about 200.000=1s
   FAB_MEASUERING_FLAG++;                                                        //count until 7(it means 10 min)
   FAB_DATA = 0;                                                                 //for initialize
   
//   fprintf(PC,"SENDING COMMAND TO FAB\r\n");
   CHECK_50_and_CW_RESPOND();                                                    //check if command 0x50 arrived from COM PIC and send CW to COM
   VERIFY_FABDATA(130000,2000);//delaytime1 and delaytime2                       //sends cmd to FAB and writes the array in_HK[] with FAB data
   CHECK_50_and_CW_RESPOND();
   
   GET_RESET_DATA();                                                             //sends cmd to reset PIC and writes the array HKDATA[] with Reset PIC data
   
   MAKE_ADCS_HKDATA();                                                           //writes the array HKDATA[] with ADCS data
   
// Turn_OFF_ADCS();                                                              //ADCS switch OFF, RD6=0
   CHECK_50_and_CW_RESPOND();
   MAKE_CW_FORMAT();                                                             //make CW format
   CHECK_50_and_CW_RESPOND();   
   SET_IDENTIFIER();                                                             //writes identifiers in HKDATA[] 
   if(CHECK_FAB_RESPONSE)                                                        
   {
      fprintf(PC,"\r\nCollecting HK and Making CWFormat Done\r\n");

      fprintf(PC,"\r\nHK data:\r\n");

      CHECK_50_and_CW_RESPOND();
      
      for(int num = 0; num < 65; num++)                                          //array[0] to [64](until gyro data)
      {
         fprintf(PC,"%x,",HKDATA[num]);
      }
      CHECK_50_and_CW_RESPOND();
      for(num = 65; num < 113; num++)                                            //GPS 60 byte should be shown as char type
      {
         //fputc(HKDATA[num],PC);
         fprintf(PC,"%x,",HKDATA[num]);                                          
      }
      CHECK_50_and_CW_RESPOND();
      for(num = 113; num < HK_Size; num++)                                       //array[113] to [124]
      {
         fprintf(PC,",%x",HKDATA[num]);
      }     
      CHECK_50_and_CW_RESPOND();
      
      DISPLAY_CW();                                                              //print CW_FORMAT[]
      CHECK_FAB_RESPONSE = 0;                                                    //flag FAB to zero     
      output_low(PIN_C4);                                                        //COM Flash memory Main side
      output_low(PIN_A5);
      //sector_erase_SCF(65536*50);
      //sector_erase_SCF(65536*51);
      SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);                                        //saves HKDATA[] in COM flash
      SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_OF(FAB_HK_ADDRESS);                                         //saves HKDATA[] in Main flash
      SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);                                      //saves CWFORMAT[] in COM flash
      SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);                                       //saves CWFORMAT[] in Main flash 
      FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;                                 //prepare for next storing address
      FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;                                 //prepare for next storing address
      fprintf(PC,"\r\nSENSORS DATA SAVED ON FLASH\r\n");
      output_high(PIN_C4);                                                       //COM Flash memory COM side
      CHECK_50_and_CW_RESPOND();
   }else{
      SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);                                        //saves HKDATA[] in COM flash 
      SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);
      SEND_HKDATA_to_OF(FAB_HK_ADDRESS);                                         //saves HKDATA[] in Main flash
      SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);                                      //saves CWFORMAT[] in COM flash
      SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
      SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);                                       //saves CWFORMAT[] in Main flash 
      FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;                                 //prepare for next storing address
      FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;                                 //prepare for next storing address
      fprintf(PC,"NO RESPONSE FROM FAB\r\n\r\n");  
      CHECK_50_and_CW_RESPOND();
      for(int num = 0; num < HK_size; num++)
      {
         fprintf(PC,"%x,",HKDATA[num]);
      }
      CHECK_50_and_CW_RESPOND();
   
   }
   
   return;
}

void GET_INITIAL_TIME()
{
   COLLECT_RESET_DATA();
   
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS,0xdd);                                 //header
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 1,0xdd);                             //header
   for(int i = 1; i < 6; i++){
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 7,0xdd);                             //footer
   WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 8,0xdd);                             //footer
   
   output_low(PIN_C4);                                                           //COM_MUX MAINSIDE
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS,0xdd);                                //header
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 1,0xdd);                            //header
   for(i = 1; i < 6; i++){
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 7,0xdd);                            //footer
   WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 8,0xdd);                            //footer
   output_high(PIN_C4);
   
   output_low(PIN_A5);                                                           //CAM_MUX MAINSIDE
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS,0xdd);                                //heade
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 1,0xdd);                            //header
   for(i = 1; i < 6; i++){
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+1+i,reset_bffr[i]);
   }
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 7,0xdd);                            //footer
   WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 8,0xdd);                            //footer
   
   ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + 9;                                //total 9byte for time info
                                                                                 //5 byte time info and 2 byte header and 2 byte footer
   return;
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//ADCS mission
int determination_flag = 0;
void GET_ADCS_MSN_DATA(int array_lenght)
{
   int c = 1;
   if(kbhit(ADCS)&&fgetc(ADCS) == 0x58)                                          //0x58 is the array header, [0]
   {
      for(int32 n= 0; n < 300000; n++)
      {
         if(kbhit(ADCS))
         {
            ADCS_SENSOR_DATA[c] = fgetc(ADCS);                                   //[1] to [array_lenght]
            fprintf(PC,"%x",ADCS_SENSOR_DATA[c]);                                //print everything I receive
            c++;                                                                 //after receive everything, c = array_lenght
         }
       if(c == array_lenght)
         {
          if(determination_flag == 1)                                            //only for determination mission
          {
          Send_Command_to_FAB(0x69);                                             //send command to fab to transmit data needed by ADCS
          }
            break;
         }
      }        
      ADCS_SENSOR_DATA[0] = 0x58;                                                //add the array header [0]
            
      int8 a = (int8)(currenttime >> 8);
      int8 b = (int8)(currenttime);
      output_low(PIN_C4);
      output_low(PIN_A5);
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS,0xAA);                              //header
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS,0xAA);                             //header
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS,0xAA);                             //header
            
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+1,a);                               //currenttime high
      WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS+2,b);                               //currenttime low
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+1,a);
      WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS+2,b);
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+1,a);
      WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+2,b);

      for(int i = 0; i < array_lenght; i++)                                      // save everything to flash memory
      {
         WRITE_DATA_BYTE_OF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
         WRITE_DATA_BYTE_SCF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
         WRITE_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS + 3 + i,ADCS_SENSOR_DATA[i]);
      }
      output_high(PIN_C4);
      fprintf(PC,"\r\n%ld,",currenttime);
      fprintf(PC,"%x,",READ_DATA_BYTE_SMF(ADCS_SENSOR_ADDRESS+3));                //ack
      fprintf(PC,"\r\n");
      ADCS_SENSOR_ADDRESS = ADCS_SENSOR_ADDRESS + array_lenght + 3;              //+3 is because 1 byte for header and 2 bytes for currenttime  
   }
   return;
}




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

int32 beginning_of_operation = 0;

//OPERATION_MODE
/*#define NOMAL_ADCS_MSN 0x22*/
#define NOMAL_ADCS_MSN 0x78                                                      //comando esperado por hisatsugu
#define AUTO_ADCS_MSN 0x27
void ADCS_TEST_OPERATION( int8 OPERATION_MODE, unsigned int16 duration, int8 EXTRA_DATA1,int8 EXTRA_DATA2, int8 EXTRA_DATA3)           //Integration Testing With ADCS
{
   int8 data_lenght = 0;
   int8 count = 0;
   LOOP_ADCS_SENSOR_ADDRESS();
   ADCS_SETTINGS();
   beginning_of_operation = ADCS_SENSOR_ADDRESS;
   
   GET_INITIAL_TIME();                                                           //save resetPIC time in OF, SCF, SMF at ADCS sensor address
   fprintf(PC,"Turn On ADCS \r\n");
   Turn_On_ADCS();                                                               //Turn on ADCS
   /*delay_ms(8000);*/
   
   fputc(0x21,ADCS);                                                             //ADCS hand shake

   CHECK_ACK_FROM_ADCS_10k();
   if(ADCS_ACK == 0x55)                                                            //Success ACK
   {
      fprintf(PC,"Received ACK From ADCS\r\n");
      delay_ms(100);
      ADCS_ACK = 0;
      fputc(OPERATION_MODE,adcs);                                                //for execute mission
      delay_ms(100);
      fputc(EXTRA_DATA1,adcs);
      delay_ms(100);
      fputc(EXTRA_DATA2,adcs);
      delay_ms(100);
      fputc(EXTRA_DATA3,adcs);
      fprintf(PC,"Sent OPERATION MODE AND EXTRA DATA to ADCS\r\n");
//ADCS MISSION MODES PART////
      switch(OPERATION_MODE)
         {
            case 0x84:                                                           //ADCS_RW_MISSION
               data_lenght = 30;
               fprintf(PC,"ADCS_RW_MISSION\r\n");
               break;
            case 0x81:                                                           //ADCS_BDOT_MISSION
               data_lenght = 35;
               fprintf(PC,"ADCS_BDOT_MISSION\r\n");
               break;
            case 0x82:                                                           //ADCS_DETERMINATION_MISSION
               data_lenght = 85;
               fprintf(PC,"ADCS_DETERMINATION_MISSION\r\n");
               determination_flag = 1;
               Send_Command_to_FAB(0x69);                                        //send command to fab to transmit data needed by ADCS
               break;
            case 0x78:                                                           //ADCS_MTQ_MISSION
               data_lenght = 0;                                                  //No data received from MTQ mission
               fprintf(PC,"ADCS_MTQ_MISSION\r\n");
               break;
            case 0x83:                                                           //ADCS_POINTING_MISSION
               data_lenght = 85;
               fprintf(PC,"ADCS_POINTING_MISSION\r\n");
               break;
            case 0x80:                                                           //ADCS_SGP4_MISSION
               data_lenght = 80;
               fprintf(PC,"ADCS_SGP4_MISSION\r\n");
               break;
         }
         
         CMD_FROM_PC[0] = 0;                                                           //prepare to get MAIN command
         PC_DATA = 0;                                                                  //prepare to get MAIN command
         currenttime = 0;                                                              //prepare to measure the operation time
         while(true)
         {
            MISSION_STATUS = (MISSION_STATUS | 0b10000000);                      // FLAG in HIGH
            for(int safe = 0; safe < 100; safe++)
            {
               GET_ADCS_MSN_DATA(data_lenght);
            }
            if(currenttime > duration)                                                 //600-->600 seconds (10min)
            {
               MISSION_STATUS = (MISSION_STATUS & 0b01111111);                   //FLAG to LOW
               fprintf(PC,"done\r\n");
               break;
            }
            for(safe = 0; safe < 100; safe++)
            {
               GET_ADCS_MSN_DATA(data_lenght);
            }
            if((currenttime%90)==0)
            {
               COLLECT_RESET_DATA();
            }
            
            for(safe = 0; safe < 100; safe++)
            {
               GET_ADCS_MSN_DATA(data_lenght);
            }
            
            if(CMD_FROM_PC[0] == 0xab)
            {
               CMD_FROM_PC[0] = 0;
               PC_DATA = 0;
               Turn_off_ADCS();
               MISSION_STATUS = (MISSION_STATUS & 0b01111111);                   //flag to low
               fprintf(PC,"Stop ADCS Mission\r\n");
               break;
            }
            for(safe = 0; safe < 100; safe++)
            {
               GET_ADCS_MSN_DATA(data_lenght);
            }
            RESET_SATELLITE();                                                         //if reset pic send command for resetting the satellite, OBC will response soon
         } 
   
   }else{
      ADCS_ACK = 0;
      fprintf(PC,"FAILURED GETTING ACK From ADCS\r\n");
   }

   determination_flag = 0;
   Turn_OFF_ADCS();                                                              //turn off ADCS OCP
   fprintf(PC,"\r\n\r\nTurned_OFF_ADCS\r\n\r\n\r\n");                                                       
   fprintf(PC,"%lx",beginning_of_operation);
   delay_ms(2000);
   Turn_On_ADCS();
   fprintf(PC,"\r\n\r\nTurned_ON_ADCS\r\n\r\n\r\n");
   delay_ms(5000);
   ADCS_TLE_TRANSFER();
   delay_ms(500);
   ADCS_GAIN_TRANSFER();
   delay_ms(500);
   ADCS_UTC_TRANSFER();
   delay_ms(500);
   RESET_SATELLITE();                                                            //if reset pic send command for resetting the satellite, OBC will response soon
   if(count > 89)
   {
      count = 0;
      COLLECT_RESET_DATA();
   }else{
      count++;
   } 
   return;
}

void ADCS_TLE_TRANSFER()                                                         //send TLE data to ADCS
{

   fprintf(PC,"Turn On ADCS \r\n");
   Turn_On_ADCS();                                                               //Turn on ADCS
   /*delay_ms(8000);*/
   for(int c = 0; c < TLE_size; c++)                                           // Read TLE data from flash and put in TLE[] array
      {
         TLE[c]=READ_DATA_BYTE_OF(ADCS_TLE_ADDRESS+c);
      }
   
   fputc(0x80,ADCS);                                                             //ADCS hand shake

   CHECK_ACK_FROM_ADCS_10k();
   if(ADCS_ACK == 0x55)                                                            //Success ACK
   {
      fprintf(PC,"Received ACK From ADCS\r\n");
      //delay_ms(1000);
      ADCS_ACK = 0;      
    for(int t = 0; t < TLE_size; t++)                                           // Read TLE data from flash and put in TLE[] array
      {
         fputc(TLE[t],ADCS);
      }
      fprintf(PC,"Sent TLE data to ADCS\r\n");
   }else{
      ADCS_ACK = 0;
      fprintf(PC,"FAILURED GETTING ACK From ADCS\r\n");
   }
   CMD_FROM_PC[0] = 0;                                                           //prepare to get MAIN command
   PC_DATA = 0;                                                                  //prepare to get MAIN command
   return;
}

void ADCS_GAIN_TRANSFER()
{

   fprintf(PC,"Turn On ADCS \r\n");
   Turn_On_ADCS();                                                               //Turn on ADCS
   /*delay_ms(8000);*/
   for(int c = 0; c < GAIN_size; c++)                                           // Read GAIN data from flash and put in GAIN_data[] array
      {
         GAIN_data[c]=READ_DATA_BYTE_OF(0x06652000+c);
      }
   
   fputc(0x81,ADCS);                                                             //ADCS gain transfer cmnd

   CHECK_ACK_FROM_ADCS_10k();
   if(ADCS_ACK == 0x55)                                                            //Success ACK
   {
      fprintf(PC,"Received ACK From ADCS\r\n");
      //delay_ms(1000);
      ADCS_ACK = 0;      
    for(int t = 0; t < GAIN_size; t++)                                           // Read GAIN data from flash and send to ADCS
      {
         fputc(GAIN_data[t],ADCS);
      }
      fprintf(PC,"Sent GAIN data to ADCS\r\n");
   }else{
      ADCS_ACK = 0;
      fprintf(PC,"FAILURED GETTING ACK From ADCS\r\n");
   }
   CMD_FROM_PC[0] = 0;                                                           //prepare to get MAIN command
   PC_DATA = 0;                                                                  //prepare to get MAIN command
   return;
}

void ADCS_UTC_TRANSFER()
{
fprintf(PC,"sending UTC data to ADCS\r\n");
   Turn_On_ADCS();                                                               
   fputc(0x83,ADCS);                                                             //ADCS hand shake
   CHECK_ACK_FROM_ADCS_10k();
   if(ADCS_ACK == 0x55)                                                          //Success ACK
   {

   fprintf(PC,"Received ACK From ADCS\r\n");
      //delay_ms(1000);
      ADCS_ACK = 0;
    for(int c = 0; c < UTC_size; c++)                                             // Read gain data from flash and put in UTC_data[] array
      {
         UTC_data[c]=READ_DATA_BYTE_OF(0x06653000+c);
      }
    fprintf(PC,"\r\nUTC:");
    for(int i=0 ; i<UTC_size ; i++)
      {
      fputc(UTC_data[i],ADCS);
      fprintf(PC,"%x,",UTC_data[i]);
      delay_ms(100);
      }
   fprintf(PC,"\r\nSent UTC DATA to ADCS\r\n");
   
   }else{
      ADCS_ACK = 0;
      fprintf(PC,"FAILURED GETTING ACK From ADCS\r\n");
   }
   CMD_FROM_PC[0] = 0;                                                           //prepare to get MAIN command
   PC_DATA = 0;                                                                  //prepare to get MAIN command
return;
}
void ADCS_UTC_UPLINK(int8 cmd1,int8 cmd2,int8 cmd3,int8 cmd4,int8 cmd5,int8 cmd6,int8 cmd7)                                                    //send array of commnands to MBP
{

      UTC_data[0]=0XAA;
      UTC_data[1]=cmd2;
      UTC_data[2]=cmd3;
      UTC_data[3]=cmd4;
      UTC_data[4]=cmd5;
      UTC_data[5]=cmd6;
      UTC_data[6]=cmd7;
      COLLECT_RESET_DATA();
      UTC_data[7]=reset_bffr[1];
      UTC_data[8]=reset_bffr[2];
      UTC_data[9]=reset_bffr[3];
      UTC_data[10]=reset_bffr[4];
      UTC_data[11]=reset_bffr[5];

      output_low(PIN_C4);                                                        //SCF main side
      output_low(PIN_A5);                                                        //SMF main side
      delay_ms(100);
      SUBSECTOR_4KB_ERASE_OF(0x06653000);                                        //clear 4KB OBC flash adcs part
      SUBSECTOR_4KB_ERASE_SCF(0x06653000);                                       //clear 4KB COM flash adcs part 
      SUBSECTOR_4KB_ERASE_SMF(0x06653000);                                       //clear 4KB COM flash adcs part
      for(int u=0 ; u<UTC_size ; u++)
         {
            WRITE_DATA_BYTE_OF(0x06653000+u,UTC_data[u]);                        //Save data on OF, SCF, SMF
            WRITE_DATA_BYTE_SCF(0x06653000+u,UTC_data[u]);
            WRITE_DATA_BYTE_SMF(0x06653000+u,UTC_data[u]);  
         }
      output_high(PIN_C4);                                                       //COM_MUX COM side
      ADCS_UTC_TRANSFER();                                                       //send UTC data to ADCS
 
 return;
}

void GET_ADCS_FLASH()
{
   int16 n = 0;
   int8 input = 0;
   int32 ADCS_FLASH_ADDRESS = 0x06651000;
   for(int32 i = 0; i < 2000000; i++)                                            //1000000
   {                                                                             
      if(kbhit(adcs))
      {
         input = fgetc(adcs);                                                    //get data from adcs
         fprintf(PC,"%x,",input);        
         WRITE_DATA_BYTE_OF(ADCS_FLASH_ADDRESS,input);                           //Save data on OF, SCF, SMF
         WRITE_DATA_BYTE_SCF(ADCS_FLASH_ADDRESS,input);
         WRITE_DATA_BYTE_SMF(ADCS_FLASH_ADDRESS,input);
         n++;
         ADCS_FLASH_ADDRESS = ADCS_FLASH_ADDRESS + 1;
         if(n==ADCScheck_size)                                                    //sale del for cuando se completa la cantidad de bytes
         {
         break;
         }
      }
   }
   fprintf(PC,"\r\n");
   fprintf(PC,"Transferred %ld Bytes",n);
   fprintf(PC,"\r\n");
/* for(int m = 0; m < ADCScheck_size; m++)                                      //Show data from ADCS on PC
   {
      fprintf(PC,"%x,",READ_DATA_BYTE_SMF(0x06651000 + m));
   }
   fprintf(PC,"\r\n");*/
   n=0;
   output_high(PIN_C4);                                                          //COM_MUX COM side
   return;
}

void ADCS_FLASH_CHECK(int8 cmd1,int8 cmd2,int8 cmd3,int8 cmd4,int8 cmd5,int8 cmd6,int8 cmd7)   //send array of commnands to ADCS
{


   fprintf(PC,"sending command ADCS FLASH check\r\n");
   Turn_On_ADCS();                                                               
   output_low(PIN_C4);                                                          //SCF main side
   output_low(PIN_A5);                                                          //SMF main side
   delay_ms(100);
   SUBSECTOR_4KB_ERASE_OF(0x06651000);                                           //clear 4KB OBC flash adcs part
   SUBSECTOR_4KB_ERASE_SCF(0x06651000);                                          //clear 4KB COM flash adcs part 
   SUBSECTOR_4KB_ERASE_SMF(0x06651000);                                          //clear 4KB COM flash adcs part
   delay_ms(100);
   fputc(cmd1,ADCS);                                                             //ADCS hand shake
   CHECK_ACK_FROM_ADCS_10k();
   if(ADCS_ACK == 0x55)                                                          //Success ACK
   {
   fprintf(PC,"Received ACK From ADCS\r\n");
   GET_ADCS_FLASH();                                                    //adcs data transfer
   }else{
      ADCS_ACK = 0;
      fprintf(PC,"FAILURED GETTING ACK From ADCS\r\n");
   }
   CMD_FROM_PC[0] = 0;                                                           //prepare to get MAIN command
   PC_DATA = 0;                                                                  //prepare to get MAIN command
   output_high(PIN_C4);                                                          //COM_MUX COM side 
 return;
}
//--------HIGH SAMPLING HK collection------------------------------------------

void CHECK_HIGH_SAMP_FABDATA(int8 in)                                            //FAB sensor data collect
{
   fprintf(PC,"\r\nFAB DATA OBTAINED\r\n");
   Delete_HKDATA();
   for(int num = 1; num < 11; num++)                                             //Collect HK DATA
   {
      HKDATA[num + 5+4] = in_HK[num + 2 - in];
      fprintf(PC, "%x,", HKDATA[num]);
   }
   MEASURE_BC_TEMP();
   HKDATA[18] = BC_temp_data_h;                                                  //-X Panel Temp
   HKDATA[19] = BC_temp_data_l;                                                  //-X Panel Temp
   for(num = 9; num < FAB_SENSOR_size - 2; num++)                                //[FAB] from CPLD temp to Kill status(array[20] to [49])
   {
      HKDATA[num + 7+4] = in_HK[num + 2 - in];
      fprintf(PC, "%x,", HKDATA[num + 7+4]);
   }
   FAB_DATA = 0;
}

void VERIFY_HIGH_SAMP_FABDATA(int32 delaytime)
{
   for(int8 num = 0; num < 3; num++)
   {
      COMMAND_TO_FAB(delaytime);
      //FAB_DATA = 0;
      //waiting(1000);
      if(in_HK[0] == 0x33)                                                       //gather sensor data by interrupt
      {
         CHECK_HIGH_SAMP_FABDATA(2);
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[1] == 0x33){
         //delay_ms(200);
         CHECK_HIGH_SAMP_FABDATA(1);
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }else if(in_HK[2] == 0x33){
         //delay_ms(200);
         CHECK_HIGH_SAMP_FABDATA(0);
         CHECK_FAB_RESPONSE = 1;                                                 //1 is succeeded to get response from FAB
         break;
      }
      FAB_DATA = 0;
   }
   return;
}



void GET_HIGH_SAMP_RESET_DATA()
{
   RESET_DATA = 0;
   COLLECT_RESET_DATA();
   if(RESET_bffr[0] == 0x8e)
   {
      fprintf(PC,"GET RESET\r\n");
      for(int num = 0; num < 5; num++)                                           //timedata
      {
         HKDATA[num + 2] = reset_bffr[num + 1];
         fputc(HKDATA[num + 2],PC);
      }
   
      for(num = 0; num < 5; num++)                                               //reset sensor data
      {
         HKDATA[num + 116] = reset_bffr[num + 6];                                //HKDATA[116] = reset[6]
         fputc(HKDATA[num + 116],PC);
      }
   }else{
      fprintf(PC,"NO RESET\r\n");
   }
   //Delete_Reset();
}

void MAKE_HIGH_SAMP_ADCS_FORMAT()
{
   GET_ADCS_SENSOR_DATA();
   for(int num = 53; num < 113; num++)                                           //12byte+48byte = 60 byte
   {
      HKDATA[num] = ADCS_SENSOR_DATA[num - 52];                                  //ADCS[1] to ADCS[60]
   }
//!   for(num = 65; num < 107; num++)
//!   {
//!      HKDATA[num] = ADCS_SENSOR_DATA[num - 34];//HKDATA[54] = ADCS_SENSOR_DATA[21]
//!   }
   return;
}

void SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY()
{
   output_low(PIN_C4);
   CHECK_50_and_CW_RESPOND();
   for(int i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_SCF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }
   output_high(PIN_C4); 
   
   CHECK_50_and_CW_RESPOND();
   for(i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_OF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }
   
   CHECK_50_and_CW_RESPOND();
   output_low(PIN_A5);
   for(i = 0; i < HIGH_SAMP_HK_size; i++)
   {
      WRITE_DATA_BYTE_SMF(HIGH_SAMP_HK_ADDRESS + i,HKDATA[i]);
   }  
   
   CHECK_50_and_CW_RESPOND();
   HIGH_SAMP_HK_ADDRESS = HIGH_SAMP_HK_ADDRESS + HIGH_SAMP_HK_size;              //prepare for next storing address
   
   return;
}

void HIGH_SAMP_FAB_OPERATION()
{
   Turn_ON_ADCS();
   CHECK_50_and_CW_RESPOND();
   HIGH_SAMP_FAB_MEASUERING_FLAG++;                                              //count until 90(it means 10 min)
   //disable_interrupts(INT_rda2);
   if(HIGH_SAMP_FAB_MEASUERING_FLAG > 17)                                        //HIGH_SAMP_FAB_MEASUERING_FLAG=18 --> 18*5 = 90 (sec)
      {
         
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\n\90sec\r\n");
         Delete_in_HK();                                                         //delet HK array
         VERIFY_FABDATA(10000,10);                                               //sends cmd to FAB and writes the array in_HK[] with FAB data
         GET_RESET_DATA();                                                       //writes HKDATA Reset PIC data
         MAKE_ADCS_HKDATA();                                                     //writes HKDATA[] with ADCS data
         SET_IDENTIFIER();
         MAKE_CW_FORMAT();
         output_low(PIN_C4);
         output_low(PIN_A5);
         
         SEND_HKDATA_to_SCF(FAB_HK_ADDRESS);                                     //save HK in COM flash memory
         SEND_HKDATA_to_SMF(FAB_HK_ADDRESS);                                     //save HK in Mission flash memory
         SEND_HKDATA_to_OF(FAB_HK_ADDRESS);                                      //save HK in OBC flash memory
         SEND_CWFORMAT_TO_SCF(FAB_CW_ADDRESS);
         SEND_CWFORMAT_TO_SMF(FAB_CW_ADDRESS);
         SEND_CWFORMAT_TO_OF(FAB_CW_ADDRESS);
         
         CHECK_50_and_CW_RESPOND();
         FAB_HK_ADDRESS = FAB_HK_ADDRESS + HK_size;                              //prepare for next storing address
         FAB_CW_ADDRESS = FAB_CW_ADDRESS + CW_size;                              //prepare for next storing address
         
         SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY();
         
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\nCOUNT:%d\r\n",HIGH_SAMP_FAB_MEASUERING_FLAG);
         HIGH_SAMP_FAB_MEASUERING_FLAG = 0;
         
      }else{
         Delete_in_HK();                                                         //delet HK array
         GET_RESET_DATA();                                                       //funcion que carga el array HKDATA con los datos del Reset PIC
         VERIFY_HIGH_SAMP_FABDATA(10000);                                        //get FAB data   
         GET_HIGH_SAMP_RESET_DATA();                                             //get reset data
         MAKE_HIGH_SAMP_ADCS_FORMAT();                                           //get ADCS data
         SET_IDENTIFIER();
         //MAKE_CW_FORMAT();
              
         fprintf(PC,"\r\n");
         for(int num = 0; num < 65; num++)                                       //array[0] to [64](until gyro data)
         {
            fprintf(PC,"%x,",HKDATA[num]);
         }
         CHECK_50_and_CW_RESPOND();
         for(num = 65; num < 113; num++)
         {
          //fputc(HKDATA[num],PC);
            fprintf(PC,"%x,",HKDATA[num]);
         }
         CHECK_50_and_CW_RESPOND();
         for(num = 113; num < HK_Size; num++)
         {
            fprintf(PC,",%x",HKDATA[num]);
         } 
         CHECK_50_and_CW_RESPOND();
         
         fprintf(PC,"\r\nCOUNT:%d\r\n",HIGH_SAMP_FAB_MEASUERING_FLAG);
         CHECK_FAB_RESPONSE = 0;
         SAVE_HIGH_SAMP_DATA_TO_EACH_MEMORY();
      }    
      //Delete_in_HK();

   FAB_DATA = 0;
   output_high(PIN_C4);

   return;
}


//--------CAM MISSION----------------------------------------------------------
//#define STORE_TO_SING AA
//////////////////////////////////////////////////
//CAM MISSION ID: 0x54                          //
//CAM ACK: 0x61                                 //
//CAM Finish Storing: 0x62                      //
//////////////////////////////////////////////////

int8 CAM_ACK = 0;


void Turn_On_CAM()
{
   output_high(PIN_D7);
   return;
}

void Turn_Off_CAM()
{
   output_low(PIN_D7);
   return;
}

void Send_Command_to_CAM(int8 data)                                              //testing for EM integration
{
   fputc(data,CAM);
   fprintf(PC,"Command:%x\r\n",data);
   return;
}

void CAM_SETTINGS()
{
   //disable_interrupts(INT_rda);                                                // Main to PC
   fprintf(PC,"CAM Start operation\r\n");
   COM_ONEBYTE_COMMAND = 0;
   fprintf(PC,"Deleting 1 sector SMF Address 00080000\r\n");
   REFRESH_SECTOR_SMF(0x00,0x08,0x00,0x00);                                      //clear 1 sector MISSION flash CAM PART
   output_high(PIN_A5);                                                          //turn on MUX (give access to CAM)
   delay_ms(100);
   Turn_On_CAM();                                                                //Turn on CAM
   fprintf(PC,"Turned On CAM and MUX CAM side\r\n");
   delay_ms(10);
   return;
}

void CHECK_ACK_FROM_CAM_10k()                                                    //check 10000 times (keep listening about 10 sec)
{
   for(int32 num = 0; num < 1000000; num++) 
   {
      if(kbhit(CAM))                                                             //kbhit(PC)
      {
         CAM_ACK = fgetc(CAM);                                                   //PC   
         break;
      }
      //delay_ms(1);
   }
   return;
}

void CHECK_ACK_FROM_CAM_30k()                                                    //check 30000 times (keep listening about 30 sec)
{
   for(int32 num = 0; num < 3000000; num++) 
   {
      if(kbhit(CAM))                                                             //kbhit(PC)
      {
         CAM_ACK = fgetc(CAM);                                                   //PC   
         break;
      }
      //delay_ms(1);
   }
   return;
}

void CHECK_ACK_FROM_CAM_200k()                                                   //check 200000 times(keep listening about 200 sec)
{
   for(int32 num = 0; num < 999000000; num++)                                     //actual value 40000000
   {
      if(kbhit(CAM))                                                             //kbhit(PC)
      {
         CAM_ACK = fgetc(CAM);                                                   //PC
         break;
      }
      //delay_ms(1);
   }
   return;
}
/*
int8 ACK_from_CAM()
{
   int8 ack;
   ack = fgetc(CAM | PC);
   return ack;
}

void Transfer_Photo() //transfer photo from Shared Mission Flash to Shared COM Flash
{
   output_low(PIN_A5);
   output_low(PIN_C4);
   TRANSFER_DATA_NBYTE_SMFtoSCF(0,CAM_ADDRESS,65536); //from: sector0, to: sector1, datasize: 64kbyte(this value is for testing)
   CAM_ADDRESS = CAM_ADDRESS + 65536;
   return;
}
*/
void CAM_TEST_OPERATION(int8 mode)                                               //Integration Testing With CAM
{
   CAM_SETTINGS();                                                               //Turn ON CAM, delete 1 sector SCF address 00080000, MUX CAM side
   delay_ms(5000);                                                               //Time is necessary for CAM to be ready


   fprintf(PC,"Sending hand shake Command to CAM\r\n");
   delay_ms(10);
   Send_Command_to_CAM(0x54);                                                    //CAM MISSION ID
   //fputc(0x54,PC);
   CHECK_ACK_FROM_CAM_10k();                                                     //keep listening about 10 sec
   
   if(CAM_ACK == 0x61)                                                           //CAM ACK
   {
      CAM_ACK = 0;
      LOOP_CAM_ADDRESS();                                                        
      fprintf(PC,"ACK received from CAM\r\n\r\n");
      //delay_ms(1000);
      fprintf(PC,"Sending time and CAM MODE data\r\n");
      CAM_SYNC();
      delay_ms(100);
      fputc(mode,CAM);
      CHECK_ACK_FROM_CAM_10k();                                                  //keep listening about 10 sec
      if(CAM_ACK == mode)
      {
            fprintf(PC,"Capturing and saving in SFM...\r\n\r\n");
         
         CHECK_ACK_FROM_CAM_200k();                                              //keep listening about 200 sec
         
         if(CAM_ACK == 0x62)                                                     //CAM Finish Storing
         {
            CAM_ACK = 0;
            fprintf(PC,"PHOTO SAVING DONE\r\n");   
            output_low(PIN_A5);                                                  //Mission Flash MUX Main side
            delay_ms(10);
            //TRANSFER_DATA_NBYTE_TOPC_SMF(0x00000000,256);                      //for checking
            output_low(PIN_C4);                                                  //COM Flash MUX Main side
            delay_ms(10);
            fprintf(PC,"Read and Transfer data from SMF TO SCF and OF Start\r\n");
            delay_ms(1000);
            
            if(mode == 0xC0 || mode == 0xC1 || mode == 0xC2 || mode == 0xC3)     //if CAM data size will be 4sectors
            {
               sector_erase_SCF(CAM_ADDRESS);                                    //delete 1st sector
               sector_erase_SCF(CAM_ADDRESS + 65536);                            //delete 2nd sector
               sector_erase_SCF(CAM_ADDRESS + 65536*2);                          //delete 3er sector
               sector_erase_SCF(CAM_ADDRESS + 65536*3);                          //delete 4th sector
               TRANSFER_DATA_NBYTE_SMFtoSCF(0x00080000,CAM_ADDRESS,65536*4);     //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_SMFtoOF(0x00080000,CAM_ADDRESS,65536*4);      //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_TOPC_SCF(CAM_ADDRESS,262144);                 //From(SCF): FROM CAM_ADDRESS To PC
               CAM_ADDRESS = CAM_ADDRESS + 65536*4;                              //se ubica en la nueva posicion, 4 sectores adelante
               delay_ms(10);
            }
            if(mode == 0xB0 || mode == 0xB1 || mode == 0xB2)                     //if CAM data size will be 2sectors
            {
               sector_erase_SCF(CAM_ADDRESS);                                    //delete 1st sector
               sector_erase_SCF(CAM_ADDRESS + 65536);                            //delete 2nd sector
               TRANSFER_DATA_NBYTE_SMFtoSCF(0x00080000,CAM_ADDRESS,80000);       //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_SMFtoOF(0x00080000,CAM_ADDRESS,80000);        //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_TOPC_SCF(CAM_ADDRESS,90000);                  //From(SCF): FROM CAM_ADDRESS To PC
               CAM_ADDRESS = CAM_ADDRESS + 65536*2;                              //se ubica en la nueva posicion, dos sectores adelante
               delay_ms(10);
            }
            if(mode == 0xA0 || mode == 0xA1 || mode == 0xA2)
            {
               sector_erase_SCF(CAM_ADDRESS);                                    //delete 1st sector
               sector_erase_SCF(CAM_ADDRESS + 65536);                            //delete 2nd sector
               TRANSFER_DATA_NBYTE_SMFtoSCF(0x00080000,CAM_ADDRESS,65536);       //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_SMFtoOF(0x00080000,CAM_ADDRESS,65536);        //From(SMF): sector8, To(SCF): sector indicated by CAM_ADDRESS
               TRANSFER_DATA_NBYTE_TOPC_SCF(CAM_ADDRESS,65536);
               CAM_ADDRESS = CAM_ADDRESS + 65536;
               delay_ms(10);
            }
            
         }else{
            CAM_ACK = 0;
            fprintf(PC,"CAM DATA SAVING FAILURED\r\n");
         }   
      }   

   }else{
      CAM_ACK = 0;
      fprintf(PC,"CAM ACK FAILURED\r\n");
   }
  
   Turn_Off_CAM();
   fprintf(PC,"Turned OFF CAM\r\n");
   output_high(PIN_C4); //Give Access to COM PIC
   //fputc(0x69,COM); 
   //enable_interrupts(INT_rda);   // Main to PC
   return;
}

void CAM_DEPLOYMENT_OPERATION(int8 mode)
{
   CAM_SETTINGS();                                                               //Turn ON CAM, delete 1 sector SCF address 00080000, MUX CAM side
   delay_ms(5000);                                                               //Time is necessary for CAM to be ready
   fprintf(PC,"Sending hand shake Command to CAM\r\n");
   delay_ms(10);
   Send_Command_to_CAM(0x54);                                                    //CAM MISSION ID
   CHECK_ACK_FROM_CAM_10k();                                                     //keep listening about 10 sec
   if(CAM_ACK == 0x61)                                                           //CAM ACK
   {
      fprintf(PC,"ACK received from CAM\r\n\r\n");
      CAM_ACK = 0;      
      LOOP_CAM_ADDRESS();                                                       
      fprintf(PC,"Sending time and BURST MODE data\r\n");
      CAM_SYNC();
      delay_ms(100);
         fputc(mode,CAM);
         CHECK_ACK_FROM_CAM_10k();                                              //keep listening about 10 sec
            if(CAM_ACK == mode)
            {
               fprintf(PC,"Capturing and saving in SFM...\r\n\r\n");
               CHECK_ACK_FROM_CAM_200k();                                       //keep listening about 200 sec
               //CHECK_ACK_FROM_CAM_30k();                                      //keep listening about 30 sec
                  if(CAM_ACK == 0x62)                                           //CAM Finish Storing
                  {
                     CAM_ACK = 0;
                     fprintf(PC,"PHOTO SAVING DONE\r\n");                    
                     CAM_ADDRESS = CAM_ADDRESS + 65536*5;                       
                     //sector_erase_SCF(CAM_ADDRESS);
                     delay_ms(10);
      
                  }else{
                     CAM_ACK = 0;
                     fprintf(PC,"CAM DATA SAVING FAILURED\r\n");
                  }   
            }   
         
   }else{
      CAM_ACK = 0;
      fprintf(PC,"CAM ACK FAILURED\r\n");
   }
   output_low(PIN_A5);                                                           //Mission Flash MUX Main side
   delay_ms(10);
   Turn_Off_CAM();
   fprintf(PC,"Turned OFF CAM\r\n");
   output_high(PIN_C4);                                                          //Give Access to COM PIC
   return;
}


//--------Mission Boss PIC Data Collection----------------------------------------------

int8 DC_ACK = 0;
int8 MBP_DATA[83] = {};

void Turn_On_MBP()
{
   output_high(PIN_F5);
   return;
}

void Turn_Off_MBP()
{
   output_low(PIN_F5);
   return;
}

void CHECK_ACK_FROM_DC_10k()                                                     //Wait ACK from DC for 10s
{
   for(int32 num = 0; num < 2000000; num++) 
   {
      if(kbhit(DC))                                                             
      {
         DC_ACK = fgetc(DC);                                                    
         if(DC_ACK == 0x20)                                                       //Success ACK
         {
            for(int32 num = 0; num < 2000000; num++) 
            {
               if(kbhit(DC))                                                             
               {
                  MISSION_STATUS = fgetc(DC);                                    //Mission status flag reception                                                 
                  break;
               }
            }  
            fprintf(PC,"MBP ACK succesfully\r\n");
            //delay_ms(1000);
            //DC_ACK = 0;
            
         }else{
            DC_ACK = 0;
            fprintf(PC,"FAILURED GETTING ACK From MBP\r\n");
         }
         break;
      }
   }
   return;
}
void CHECK_ACK_FROM_MBP()                                                        //Wait ACK from DC for 10s
{
   for(int32 num = 0; num < 2000000; num++) 
   {
      if(kbhit(DC))                                                             
      {
         DC_ACK = fgetc(DC);                                                    
         if(DC_ACK == 0x20)                                                       //Success ACK
         {  
            fprintf(PC,"MBP ACK succesfully\r\n");
            //delay_ms(1000);
            //DC_ACK = 0;
            
         }else{
            DC_ACK = 0;
            fprintf(PC,"FAILURED GETTING ACK From MBP\r\n");
         }
         break;
      }
   }
   return;
}
void DELETE_MBP_DATA()                                                           //Clear MBP array to receive data
{
   for(int8 i = 0; i < 81; i++)
   {
      MBP_DATA[i] = 0;
   }
   return;
}

void GET_MBP_DATA(int16 transf_size)
{
   int16 n = 0;
   int8 input = 0;
   for(int32 i = 0; i < 99000000; i++)                                            //1000000
   {                                                                              //99000000 es tiempo suficiente para transferir un sector
      if(kbhit(DC))
      {
         input = fgetc(DC);                                                      //get data from MBP
         WRITE_DATA_BYTE_OF(DC_STATUS_ADDRESS,input);                            //Save data on OF, SCF, SMF
         WRITE_DATA_BYTE_SCF(DC_STATUS_ADDRESS,input);
         WRITE_DATA_BYTE_SMF(DC_STATUS_ADDRESS,input);
         n++;
         DC_STATUS_ADDRESS = DC_STATUS_ADDRESS + 1;
         if(n==transf_size)                                                      
         {
         break;
         }
      }
   }

   DC_STATUS_ADDRESS=0x06660000;

   fprintf(PC,"Transferred %ld Bytes",n);
   n=0;
   output_high(PIN_C4);                                                          //COM_MUX COM side
   return;
//!   fputc(0x67,DC);                                                               
//!   int16 n = 0;
//!   for(int32 i = 0; i < 1000000; i++)                                            //10000000
//!   {
//!      if(kbhit(DC))
//!      {
//!         MBP_DATA[n] = fgetc(DC);                                                //get data from MBP
//!         n++;
//!      }
//!   }
//!   for(int m = 0; m < 83; m++)                                                   //Show data from MBP on PC
//!   {
//!      fprintf(PC,"%x",MBP_DATA[m]);
//!   }
//!   fprintf(PC,"\r\n");
//!   output_low(PIN_C4);                                                           //COM_MUX Main side
//!   output_low(PIN_A5);                                                           //CAM_MUX Main side
//!   for(m = 0; m < 83; m++)                                                       //Save data on OF, SCF, SMF
//!   {
//!      WRITE_DATA_BYTE_OF(DC_STATUS_ADDRESS + m,MBP_DATA[m]);
//!      WRITE_DATA_BYTE_SCF(DC_STATUS_ADDRESS + m,MBP_DATA[m]);
//!      WRITE_DATA_BYTE_SMF(DC_STATUS_ADDRESS + m,MBP_DATA[m]);
//!   }
//!   output_high(PIN_C4);                                                          //COM_MUX COM side
//!   DC_STATUS_ADDRESS = DC_STATUS_ADDRESS + 83;                                   //save the new DC address
//!   
//!   return;
}

void DATACOLLECTION_TEST_OPERATION(int8 cmd1,int8 cmd2,int8 cmd3,int8 cmd4,int8 cmd5,int8 cmd6,int8 cmd7)
{
   DELETE_MBP_DATA();                                                            //Clear MBP array to receive data
  // LOOP_DC_STATUS_ADDRESS();                                                   
   REFRESH_SECTOR_OF(0x06,0x66,0x00,0x00);                                       //clear 1 sector OBC flash DC part
   REFRESH_SECTOR_SCF(0x06,0x66,0x00,0x00);                                      //clear 1 sector COM flash DC part 
   REFRESH_SECTOR_SMF(0x06,0x66,0x00,0x00);                                      //clear 1 sector MISSION flash DC part
   
   int16 data_pckts = make16(cmd6,cmd7);
   int16 data_size = data_pckts*81;
   
   fprintf(PC,"sending commands to MBP\r\n");
   Turn_On_MBP();                                                                 //RF5=HIGH
   delay_ms(30);

   fputc(cmd1,DC);
   delay_ms(100);
   fputc(cmd2,DC);
   delay_ms(100);
   fputc(cmd3,DC);
   delay_ms(100);
   fputc(cmd4,DC);
   delay_ms(100);
   fputc(cmd5,DC);
   delay_ms(100);
   fputc(cmd6,DC);
   delay_ms(100);
   fputc(cmd7,DC);

   CHECK_ACK_FROM_MBP();                                                      //Wait ACK from MBP for 10s
   if(DC_ACK == 0x20)                                                            //Success ACK
   {
      //delay_ms(1000);
      DC_ACK = 0;
      output_low(PIN_C4);                                                        //SCF main side
      output_low(PIN_A5);                                                        //SMF main side
      fprintf(PC,"TRANSFERRING %lx PACKETS\r\n",data_pckts);
      
      GET_MBP_DATA(data_size);

   }else{                                                                        //FAILURED GETTING ACK From MBP
      DC_ACK = 0;

      fprintf(PC,"PWR cycle and sending commands again\r\n");
      Turn_Off_MBP();
      delay_ms(1000);
      Turn_On_MBP();                                                              //RF5=HIGH
      delay_ms(5000);
      DC_ACK = 0;
      fputc(cmd1,DC);
      delay_ms(100);
      fputc(cmd2,DC);
      delay_ms(100);
      fputc(cmd3,DC);
      delay_ms(100);
      fputc(cmd4,DC);
      delay_ms(100);
      fputc(cmd5,DC);
      delay_ms(100);
      fputc(cmd6,DC);
      delay_ms(100);
      fputc(cmd7,DC);
      CHECK_ACK_FROM_MBP();                                                      //Wait ACK from DC for 10s
      if(DC_ACK == 0x20)                                                            //Success ACK
      {
         //delay_ms(1000);
         DC_ACK = 0;
         output_low(PIN_C4);                                                        //SCF main side
         output_low(PIN_A5);                                                        //SMF main side
         fprintf(PC,"TRANSFERRING %lx PACKETS\r\n",data_pckts);         
         GET_MBP_DATA(data_size);

      }
   }
/*   
   fprintf(PC,"Turning OFF MBP\r\n");
   Turn_Off_MBP();*/                                                             //RF5=LOW
   output_high(PIN_C4);                                                          //COM_MUX COM side
   
   return;
}

void MBP_TLE_TRANSFER()
{
   for(int c = 0; c < TLE_size; c++)                                             // Read TLE data from flash and put in TLE[] array
      {
         TLE[c]=READ_DATA_BYTE_SMF(ADCS_TLE_ADDRESS+c);
      }
   fprintf(PC,"\r\nTLE:");
   for(int t = 0; t < TLE_size; t++)
      {
         fputc(TLE[t],DC);                                                       //send TLE data to MBP
         delay_ms(100);
         fprintf(PC,"%x,",TLE[t]);
      }
      fprintf(PC,"\r\nSent TLE data to MBP\r\n");
return;
}


void MISSION_TEST_OPERATION(int8 cmd1,int8 cmd2,int8 cmd3,int8 cmd4,int8 cmd5,int8 cmd6,int8 cmd7)                                                    //send array of commnands to MBP
{
   fprintf(PC,"sending commands to MBP\r\n");
   Turn_On_MBP();                                                                 //RF5=HIGH
   delay_ms(30);

      fputc(cmd1,DC);
      delay_ms(100);
      fputc(cmd2,DC);
      delay_ms(100);
      fputc(cmd3,DC);
      delay_ms(100);
      fputc(cmd4,DC);
      delay_ms(100);
      fputc(cmd5,DC);
      delay_ms(100);
      fputc(cmd6,DC);
      delay_ms(100);
      fputc(cmd7,DC);
      
   CHECK_ACK_FROM_DC_10k();                                                      //Wait ACK from DC for 10s
   if(DC_ACK == 0x20)                                                            //Success ACK
   {
      DC_ACK = 0;
      if(cmd1==0xD4 && cmd2==0xA3)
      {
      MBP_TLE_TRANSFER();                                                        //send TLE data to MBP
      }
   }else{
      DC_ACK = 0;
      fprintf(PC,"PWR cycle and sending commands again\r\n");
      Turn_Off_MBP();
      delay_ms(1000);
      Turn_On_MBP();                                                              //RF5=HIGH
      delay_ms(5000);
      DC_ACK = 0;
      fputc(cmd1,DC);
      delay_ms(100);
      fputc(cmd2,DC);
      delay_ms(100);
      fputc(cmd3,DC);
      delay_ms(100);
      fputc(cmd4,DC);
      delay_ms(100);
      fputc(cmd5,DC);
      delay_ms(100);
      fputc(cmd6,DC);
      delay_ms(100);
      fputc(cmd7,DC);   
      CHECK_ACK_FROM_DC_10k();                                                      //Wait ACK from DC for 10s
      if(DC_ACK == 0x20)                                                            //Success ACK
      {
         //delay_ms(1000);
         DC_ACK = 0;
         if(cmd1==0xD4 && cmd2==0xA3)
         {
         MBP_TLE_TRANSFER();                                                        //send TLE data to MBP
         }
      }
   }
   return;
}

void MISSION_FLAG_STATUS()                                                      //Print on PC port current flag status
{
   fprintf(PC,"\r\nMISSIONS STATUS:\r\n");
   fprintf(PC,"MBP DATA TRANSFERING:%x\r\n",(MISSION_STATUS & 0b00000001));
   fprintf(PC,"PSC MISSION:%x\r\n",(MISSION_STATUS & 0b00000010)>>1);
   fprintf(PC,"TMCR MISSION:%x\r\n",(MISSION_STATUS & 0b00000100)>>2);
   fprintf(PC,"NTU MISSION:%x\r\n",(MISSION_STATUS & 0b00001000)>>3);
   fprintf(PC,"SFWARD MISSION:%x\r\n",(MISSION_STATUS & 0b00010000)>>4);
   fprintf(PC,"HNT MISSION:%x\r\n",(MISSION_STATUS & 0b00100000)>>5);
   fprintf(PC,"APRSDP MISSION:%x\r\n",(MISSION_STATUS & 0b01000000)>>6);
   fprintf(PC,"ADCS MISSION:%x\r\n",(MISSION_STATUS & 0b10000000)>>7);
   
   return;
}

//-------Kill Switch-----------------------------------------------------------
Static int8 KILL_COUNTER = 0;
void Count_Kill_Flag()
{
   KILL_COUNTER++;
   if(Kill_COUNTER > 4)
   {
      Kill_FLAG_MAIN = 1;
      output_high(PIN_A4);                                                       //Kill Switch ON
      fputc(0x17,FAB);
   }
   fprintf(PC,"Kill counter: %x\r\n",KILL_COUNTER);
   return;
}

void Disable_Kill()
{
   KILL_COUNTER = 0;
   
   Kill_FLAG_MAIN = 0;
   output_low(PIN_A4);                                                           //Kill Switch OFF
   fputc(0x18,FAB);
   
   fprintf(PC,"Kill counter: %x\r\n",KILL_COUNTER);
   return;
}


//-------Reset Function--------------------------------------------------------


void BC_ON_30min()
{
   if(currenttime > 1805 && BC_ATTEMPT_FLAG == 0)                                //if first attempt and 30 min(1800sec) passed // changed to 1 min
   {
      fprintf(PC,"sending BC command to RESET PIC\r\n");
      for(int16 num = 0; num < 200; num++)
      {
         fputc(0xBC,reset);
         delay_ms(100);
         if(reset_bffr[0] == 0xCB)                                               
         {
         break;
         }
      }
      delay_ms(1000);                                                            
      if(reset_bffr[0] == 0xCB)
      {
         BC_OPERATION();
         BC_ATTEMPT_FLAG++;
         WRITE_FLAG_to_EEPROM();
         STORE_FLAG_INFO();
         STORE_ADRESS_DATA_TO_FLASH();
         reset_bffr[0] = 0;
         RESET_DATA = 0;
         delay_ms(1000);
         delay_ms(20000);                                                        //wait until RESET goes back to nomal loop
         SAVE_SAT_LOG(0xBC,0x30,0x30);                                           //first 30 min antenna deployment
      }
   }
   return;
}



void HIGHSAMP_SENSOR_COLLECTION(int16 times)
{
   fputc(0X36,ADCS);                                                             //ADCS GPS ON   
   LOOP_HIGH_SAMP_HK_ADDRESS();                                                  //loop in memory to save data, keep first 3 sectors forever
   int32 num = 0;

   while(num < times)
   {
      CHECK_50_and_CW_RESPOND();                                                 //check cw cmd from COM PIC
      if(FAB_FLAG > 4)
      {
         FAB_FLAG = 0;
         HIGH_SAMP_FAB_OPERATION();                                              //collect FAB, RESET and ADCS data
         
         num++;
         if((num % 20) == 0)                                                     //once in the 20 times(every 100sec save the address to the flash)
         {
            STORE_ADRESS_DATA_TO_FLASH();                                        //for store the flag
         } 
      }

      BC_ON_30min();                                                             //check if first attempt and 30 min(1800sec) passed
      if((CMD_FROM_PC[0] == 0xAB)||(in_bffr_main[4] == 0xAB))                    //forced termination function
      {
         Delete_Buffer();
         break;
      }
      PC_DATA = 0;
      RESET_SATELLITE();                                                         //check reset command from RESET PIC
   }
   fputc(0X37,ADCS);                                                             //ADCS GPS OFF   
   fprintf(PC,"HSSC DONE\r\n");
   return;
}

void REPLY_TO_COM(int8 data1,int8 data2)
{
   if(ACK_for_COM[12] == 0x66 && ACK_for_COM[14] == 0x77)
   {
      for(int n = 0; n < 24; n++)                                                //send back the acknowledge
      {
         fputc(ACK_for_COM[n],COM);
      }
      for(n = 0; n < 24; n++)                                                    //send back the acknowledge
      {
         fprintf(PC,"%x,",ACK_for_COM[n]);
      }
      fprintf(PC,",AUTO\r\n");
   }
   else{
      //REFLESH_MSN_ACK_for_COM();
//!      for(int n = 7; n < 24; n++)//send back the acknowledge
//!      {
//!         ACK_for_COM[n] = 0xff;
//!      }
      ACK_for_COM[0] = 0xAA;
      ACK_for_COM[12] = data1;
      ACK_for_COM[13] = data2;
      ACK_for_COM[23] = 0xBB;
      for(int n = 0; n < 24; n++)                                                //send back the acknowledge
      {
         fputc(ACK_for_COM[n],COM);
      }
      for(n = 0; n < 24; n++)                                                    //send back the acknowledge
      {
         fprintf(PC,"%x,",ACK_for_COM[n]);
      }
      fprintf(PC,",NORMAL\r\n");
   }

}

void UPDATE_ACK_for_COM(int8 data1,int8 data2,int32 address, int16 size)         //send data to COM for automatical mission
{
   fprintf(PC,"address is %lx\r\n", address);
   fprintf(PC,"size is %lu\r\n",size);
   REFLESH_MSN_ACK_for_COM();                                                    //clear array ACK_for_COM[i] from position 12 to 22
   ACK_for_COM[15] = address >> 24;
   ACK_for_COM[16] = address >> 16;
   ACK_for_COM[17] = address >> 8;
   ACK_for_COM[18] = address;
   
   ACK_for_COM[19] = 0;
   ACK_for_COM[20] = 0;
   ACK_for_COM[21] = size >> 8;
   ACK_for_COM[22] = size;
   
   ACK_for_COM[0] = 0xAA;                                                        //header
   ACK_for_COM[12] = data1;                                                      //success ack indicator 0x66
   ACK_for_COM[14] = data2;                                                      //auto indicator 0x77
   ACK_for_COM[23] = 0xBB;                                                       //footer
   fprintf(PC,"sending to COM PIC: \r\n");
   for(int m = 0; m<24; m++)
   {
      fprintf(PC,"%x ",ACK_for_COM[m]);
   }
   fprintf(PC,"\r\n");
   for(int n = 0; n < 24; n++)                                                   //send back the acknowledge
   {
      fputc(ACK_for_COM[n],COM);
   }
}

void RSV_DATA_DOWNLOAD(int8 cmnd1,int8 cmnd2,int8 cmnd3,int8 cmnd4,int8 cmnd5,int8 cmnd6,int8 cmnd7) //carga el array ACK_for_COM[] con el dato del array CW_FORMAT[] y envia a COM PIC
{
   for(int a = 13; a < 23; a++)                                                   //clear ACK_for_COM array 
   {
      ACK_for_COM[a] = 0x00;
   }
   delay_ms(100);
//   ACK_for_COM[0] = 0xAA;
 //ACK_for_COM[13] = cmnd2;                                                      //CMD2 Mode (time to give access)
   ACK_for_COM[14] = cmnd1;                                                      //CMD1 Function
   ACK_for_COM[15] = cmnd3;                                                      //CMD3 address1
   ACK_for_COM[16] = cmnd4;                                                      //CMD4 address2
   ACK_for_COM[17] = cmnd5;                                                      //CMD5 address3
   ACK_for_COM[18] = cmnd6;                                                      //CMD6 address4
   ACK_for_COM[19] = 0x00;                                                       //empty
   ACK_for_COM[20] = 0x00;                                                       //empty
   ACK_for_COM[21] = 0x00;                                                       //empty
   ACK_for_COM[22] = cmnd7;                                                      //CMD7 pckt
//   ACK_for_COM[23] = 0xBB;
 /*  for(int m = 0; m < 24; m++)                                                   //send the reservation function data to COM 
   {
      fputc(ACK_for_COM[m],COM);
   }*/

   for(int m = 0; m < 23; m++)                                                       //send the reservation function data to PC
   {
      fprintf(PC,"%x,",ACK_for_COM[m]);
   }
   fprintf(PC,"%x\r\n",ACK_for_COM[23]);
   
/* for(int k = 14; k < 23; k++)                                                   //clear ACK_for_COM array
   {
      ACK_for_COM[k] = 0x00;
   }*/
   return;
}

void MBP_SYNC()
{
   fprintf(PC,"\r\nsending SYNC time to MBP\r\n");
   Turn_On_MBP();                                                                 //RF5=HIGH
   delay_ms(30);
   COLLECT_RESET_DATA();
      if(RESET_bffr[0] == 0x8e)
   {
      fprintf(PC,"Time received from RESET\r\n");

   }else{
      fprintf(PC,"NO RESET time\r\n");
   }
      
      fputc(0XAA,DC);
      delay_ms(100);
      fputc(reset_bffr[1],DC);                                                       //sec
      delay_ms(100);
      fputc(reset_bffr[2],DC);                                                       //minutes
      delay_ms(100);
      fputc(reset_bffr[3],DC);                                                       //hours
      delay_ms(100);
      fputc(reset_bffr[4],DC);                                                       //day_H
      delay_ms(100);
      fputc(reset_bffr[5],DC);                                                       //day_L
      delay_ms(100);
      fputc(0XBB,DC);
   fprintf(PC,"%x:%x:%x,%x%x\r\n",reset_bffr[1], reset_bffr[2], reset_bffr[3], reset_bffr[4], reset_bffr[5]);
   CHECK_ACK_FROM_DC_10k();                                                      //Wait ACK from DC for 10s
   if(DC_ACK == 0x20)                                                            //Success ACK
   {  
      fprintf(PC,"Time sent to MBP\r\n");
      //delay_ms(1000);
      DC_ACK = 0;
      
   }else{
      DC_ACK = 0;
      fprintf(PC,"FAILURE sending time\r\n");
   }
return;
}

void CAM_SYNC()
{
   fprintf(PC,"\r\nTime: ");
   COLLECT_RESET_DATA();
   /* if(RESET_bffr[0] == 0x8e)
   {
      fprintf(PC,"Time received from RESET\r\n");

   }else{
      fprintf(PC,"NO RESET time\r\n");
   }*/

      fputc(reset_bffr[1],CAM);                                                       //sec
      delay_ms(100);
      fputc(reset_bffr[2],CAM);                                                       //minutes
      delay_ms(100);
      fputc(reset_bffr[3],CAM);                                                       //hours
      delay_ms(100);
      fputc(reset_bffr[4],CAM);                                                       //day_H
      delay_ms(100);
      fputc(reset_bffr[5],CAM);                                                       //day_L
      delay_ms(100);
      fputc(0xEE,CAM);                                                                 //separator need for CAM
   fprintf(PC,"%x:%x:%x,%x%x\r\n",reset_bffr[1], reset_bffr[2], reset_bffr[3], reset_bffr[4], reset_bffr[5]);

return;
}


