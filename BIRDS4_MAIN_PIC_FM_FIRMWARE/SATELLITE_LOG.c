void waiting(int32 counterval)                                                   //about 200000 : 1s
{
   for(int32 num = 0; num < counterval; num++)
   {}
   return;
}

//--------RESET Function---------------------------------------------


void DELETE_RESET()                                                              //clear reset_bffr[]
{
   for(int8 num = 0; num < Reset_size; num++)
   {
      reset_bffr[num] = 0;
   }
   return;
}

void SEND_CMD_FOR_RESET_SATELLITE()
{  
   STORE_FLAG_INFO();                                                            //save flag data to flash memory
   Save_RSV();                                                                   //save in flash memory reserve_table[] content
   STORE_ADRESS_DATA_TO_FLASH();
// while(true)
// {
      //fputc(0x27,PC);
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-122));                   //sec
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-121));                   //min
      fprintf(PC,"%x,",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-120));                   //hou
      fprintf(PC,"%x",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-119));                    //day H
      fprintf(PC,"%x\r\n",READ_DATA_BYTE_OF(FAB_HK_ADDRESS-118));                //day L
        for(int i = 0; i<100; i++)
        {
         fputc(0x27,reset);                                                      //send command for satellite reset
         delay_ms(10);
        }
      //delay_ms(100);//if it is needed
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-122),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-121),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-120),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-119),reset);
//!      fputc(READ_DATA_BYTE_OF(FAB_HK_ADDRESS-118),reset);
      RESET_DATA = 0;
      fprintf(PC,"Ready for reset");
      delay_ms(1000);
// }
   return;
}

void RESET_SATELLITE()                                                           //if get reset command from reset, prepare for resetting
{
   if(reset_flag == 1)                                                           //get ready for reset satellite
   {
      SEND_CMD_FOR_RESET_SATELLITE();                                            //store flag, save RSV, store Address, send command and wait for reset
   }
   DELETE_RESET();                                                               //load zero in reset_bffr[]
   RESET_DATA = 0;
   return;
}

void COLLECT_RESET_DATA()                                                        //sends 0x28 to reset PIC and writes reset_bffr[]
{
   DELETE_RESET();                                                               //clear the array
   RESET_DATA = 0;
   for(int m=0; m<10; m++)
   {
      fputc(0x28,reset);                                                            
      delay_ms(10);
      waiting(10000);                                                               //wait aprox 0.5s for RESET PIC to sends data
         if(RESET_bffr[0] == 0x8e)
         {
         break;
         }
   }
   return;
}


//@@@@@@@@@@@for satellite log@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define logdata_size 11
int8 satellitelog[logdata_size] = {};

void SAVE_SAT_LOG(int8 data1, int8 data2, int8 data3)
{
   LOOP_SAT_LOG();                                                               
   RESET_DATA = 0;
   COLLECT_RESET_DATA();                                                         //writes RESET_bffr[] with reset PIC data
   if(RESET_bffr[0] == 0x8e)
   {
      satellitelog[0] = 0xda;                                                    //header of the satellite log
      satellitelog[1] = 0xda;                                                    //header of the satellite log
   
      satellitelog[2] = RESET_bffr[1];
      satellitelog[3] = RESET_bffr[2];
      satellitelog[4] = RESET_bffr[3];
      satellitelog[5] = RESET_bffr[4];
      satellitelog[6] = RESET_bffr[5];
      
      satellitelog[7] = data1;
      satellitelog[8] = data2;
      satellitelog[9] = data3;
      satellitelog[10] = 0xef;                                                   //footer, fin del array
      
      output_low(PIN_A5);                                                        //CAM_MUX main side
      output_low(PIN_C4);                                                        //COM_MUX main side
      for(int i = 0; i < logdata_size; i++)                                      //saves data in flash memories 
      {
         WRITE_DATA_BYTE_OF(SAT_LOG + i,satellitelog[i]);
         WRITE_DATA_BYTE_SCF(SAT_LOG + i,satellitelog[i]);
         WRITE_DATA_BYTE_SMF(SAT_LOG + i,satellitelog[i]);
      }
      output_high(PIN_C4);                                                       //COM_MUX COM side
      SAT_LOG = SAT_LOG + logdata_size;
      fprintf(PC,"LOG saving done\r\n");
   }
   
   return;
}



