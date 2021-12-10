

#define table_size 80
unsigned int8 reserve_table[table_size+1] = {};
unsigned int8 table_num = 0;

void initialize_table()                                                          
{                                                                                
   for(int num = 0; num < table_size; num++)
   {
      reserve_table[num] = 0xff;
   }
   reserve_table[80] = 0;                                                        //writes 0 in the last position (counter for position in used)
   return;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//                          ***Sort Function***
//This is for sorting the saved reservation commands in ascending order.
//
//reserved_table[j-9] reserved_table[j-8] reserved_table[j-7] reserved_table[j-6] reserved_table[j-5] reserved_table[j-4] reserved_table[j-3] reserved_table[j-2]
//reserved_table[j-1]  reserved_table[j]  reserved_table[j+1] reserved_table[j+2] reserved_table[j+3] reserved_table[j+4] reserved_table[j+5] reserved_table[j+6] 
//
//j-9 and j-1 data are the contents information(like mission)
//j-8 and j data are the time data(execute the command reserved_table[j] min later )
//j-7 and j+1 data are the contents detail information
//
//by comapring j-8 and j data,if it is needed, swhich j-9 and j-1, j-8 and j, j-7 and j+1....etc
////////////////////////////////////////////////////////////////////////////////////////////////

int8 count_reservation()                                                         //counts how many positions have data different of 0xFF 
{
   int8 counter = 0;
   for(int num = 1; num < table_size - 6; num = num + 8)
   {
      if(reserve_table[num-1] != 0xff)
      {
         counter++;
      }
   }
   return counter * 8;
}

void sort_table()                                                                //(command info, time info, mission mode)~10¨compare the time and so in ascending formrt
{
   for(int i = 1; i < table_size - 7; i = i + 8){

         // compare from lower site
      for(int j = table_size - 7; j > i; j = j - 8){

      // if upper value is larger, switch the values
         if(reserve_table[j] < reserve_table[j-8]){                              //compare the time of [j]th and [j-8]th
            int t1 = reserve_table[j+6];
            int t2 = reserve_table[j+5];
            int t3 = reserve_table[j+4];
            int t4 = reserve_table[j+3];
            int t5 = reserve_table[j+2];
            int t6 = reserve_table[j+1];
            int t7 = reserve_table[j];
            int t8 = reserve_table[j-1];
            
            reserve_table[j+6] = reserve_table[j-2];
            reserve_table[j+5] = reserve_table[j-3];
            reserve_table[j+4] = reserve_table[j-4];
            reserve_table[j+3] = reserve_table[j-5];
            reserve_table[j+2] = reserve_table[j-6];
            reserve_table[j+1] = reserve_table[j-7];
            reserve_table[j] = reserve_table[j-8];
            reserve_table[j-1] = reserve_table[j-9];
            
            reserve_table[j-2] = t1;
            reserve_table[j-3] = t2;
            reserve_table[j-4] = t3;
            reserve_table[j-5] = t4;
            reserve_table[j-6] = t5;
            reserve_table[j-7] = t6;
            reserve_table[j-8] = t7;
            reserve_table[j-9] = t8;
                
         
         }
      }
   }
   return;
}

void Save_RSV()
{
   output_low(PIN_A5);                                                           //CAM_MUX MAINSIDE
   output_low(PIN_C4);                                                           //COM_MUX MAINSIDE
   SUBSECTOR_4KB_ERASE_OF(RSV_DATA_ADDRESS);
   delay_ms(100);
   SUBSECTOR_4KB_ERASE_SCF(RSV_DATA_ADDRESS);
   delay_ms(100);
   SUBSECTOR_4KB_ERASE_SMF(RSV_DATA_ADDRESS);
   delay_ms(100);
   
   for(int num = 0; num < table_size + 1; num++)                                 //save in flash memory reserve_table[] content
   {
      WRITE_DATA_BYTE_OF(RSV_DATA_ADDRESS+num,reserve_table[num]);
      WRITE_DATA_BYTE_SMF(RSV_DATA_ADDRESS+num,reserve_table[num]);
      WRITE_DATA_BYTE_SCF(RSV_DATA_ADDRESS+num,reserve_table[num]);
   }
   
   WRITE_DATA_BYTE_OF(RSV_DATA_ADDRESS+81,RESERVE_MIN_FLAG);                     // save reserve flag
   WRITE_DATA_BYTE_SCF(RSV_DATA_ADDRESS+81,RESERVE_MIN_FLAG);
   WRITE_DATA_BYTE_SMF(RSV_DATA_ADDRESS+81,RESERVE_MIN_FLAG);
   for(num = 0; num < table_size + 1; num++)                                     //show in PC RSV DATA
   {
      fprintf(PC,"%x",READ_DATA_BYTE_OF(RSV_DATA_ADDRESS + num));
   }
   output_high(PIN_C4);                                                          //COM_MUX MAINSIDE
   fprintf(PC,"\r\n");
   return;
}

void table_refresh()                                                             //table and flags are come back to default 
{
   for(int i = 0; i < table_size; i++)
   {
      reserve_table[i] = 0xff;
   }
   reserve_table[80] = 0;
   RESERVE_MIN_FLAG = 0;
   RESERVE_SEC_FLAG = 0;
   RESERVE_CHECK = 0;
   MISSION_CONTENTS = 0;                                                         //closest mission command
   RESERVE_TARGET_FLAG = 0;                                                      //closest time data
   MISSION_DETAIL = 0;                                                           //closest mission command detail
   RESERVE_ADDRESS_1 = 0;                                                        //closest mission address
   RESERVE_ADDRESS_2 = 0;                                                        //closest mission address
   RESERVE_ADDRESS_3 = 0;                                                        //closest mission address
   RESERVE_ADDRESS_4 = 0;                                                        //closest mission address
   RESERVE_PACKET_NUM = 0;                                                       //closest mission pckt number
   Save_RSV();
   return;
}

void Get_RSV()
{
   //fprintf(PC,"%x");
   for(int num = 0; num < table_size+1; num++)                                   //writes reserve_table[] with data readed from flash memory 
   {                                                                             
      reserve_table[num]=READ_DATA_BYTE_OF(RSV_DATA_ADDRESS + num);              //import reservation table info
   }
   reserve_table[80] = count_reservation();                                      //writes in position 30 the reservation counter
//   RESERVE_MIN_FLAG = READ_DATA_BYTE_OF(RSV_DATA_ADDRESS+31);
   if(reserve_table[80] == 0xFF || reserve_table[80] == 0x00)                    //if table is empty
   {
      initialize_table();                                                        //writes 0xff 
   }
   table_num = reserve_table[80];                                                
   if(table_num != 0x00)                                                         //yes-->get reserved command and target time
   {
      RESERVE_CHECK = 1;                                                         //table is in use
      MISSION_CONTENTS = reserve_table[0];                                       //closest mission command
      RESERVE_TARGET_FLAG = reserve_table[1];                                    //closest time data
      MISSION_DETAIL = reserve_table[2];                                         //closest mission command detail
      RESERVE_ADDRESS_1 = reserve_table[3];                                      //closest mission address
      RESERVE_ADDRESS_2 = reserve_table[4];                                      //closest mission address
      RESERVE_ADDRESS_3 = reserve_table[5];                                      //closest mission address
      RESERVE_ADDRESS_4 = reserve_table[6];                                      //closest mission address
      RESERVE_PACKET_NUM = reserve_table[7];                                     //closest mission pckt number
   }else{
      RESERVE_CHECK = 0;                                                         //table is not in use
      MISSION_CONTENTS = 0xff;                                                   //closest mission command
      RESERVE_TARGET_FLAG = 0xff;                                                //closest time data
      MISSION_DETAIL = 0xff;                                                     //closest mission command detail
      RESERVE_ADDRESS_1 = 0xff;                                                  //closest mission address
      RESERVE_ADDRESS_2 = 0xff;                                                  //closest mission address
      RESERVE_ADDRESS_3 = 0xff;                                                  //closest mission address
      RESERVE_ADDRESS_4 = 0xff;                                                  //closest mission address
      RESERVE_PACKET_NUM = 0xff;                                                 //closest mission pckt number
   }
   return;
}

void Disp_RSV()                                                                  //show the reserve status (reserve command and reserve table)
{
   Get_RSV();                                                                    //get the info from the main flash
   for(int num = 0; num < table_size + 1; num++)
   {
      fprintf(PC,"%x",reserve_table[num]);
   }
   fprintf(PC,"\r\n");
   fprintf(PC,"RESERVE_CHECK = %x\r\n",RESERVE_CHECK);
   fprintf(PC,"NEXT FUNCTION_CONTENTS = %x\r\n",MISSION_CONTENTS);
   fprintf(PC,"RESERVE_TARGET_FLAG = %x\r\n",RESERVE_TARGET_FLAG);
   fprintf(PC,"NEXT FUNCTION DETAIL = %x\r\n",MISSION_DETAIL);
   fprintf(PC,"NEXT FUNCTION ADDRESS = %x%x%x%x\r\n",RESERVE_ADDRESS_1,RESERVE_ADDRESS_2,RESERVE_ADDRESS_3,RESERVE_ADDRESS_4);
   fprintf(PC,"NEXT FUNCTION PCKT NUMBER = %x\r\n",RESERVE_MIN_FLAG);
   fprintf(PC,"RESERVE_MIN_FLAG = %x\r\n",RESERVE_MIN_FLAG);
   fprintf(PC,"TABLE NUM = %x\r\n",table_num);
   
   return;
}



void input_reservation_COM()                                                     //put the command from COM into the table and sort in ascending form
{
//!   reserve_table[table_num] = CMD_FROM_PC[0];                                 //input the data about mission content
//!   reserve_table[table_num + 1] = CMD_FROM_PC[1];                             //input the data about time data
//!   reserve_table[table_num + 2] = CMD_FROM_PC[2];                             //input the data about mission detail mode
   //fprintf(PC,"Table num: %x\r\n",reserve_table[80]);                          //show the amount of bytes ocupied in the table
   table_num = reserve_table[80];
   if(reserve_table[80] == 80)
   {
      fprintf(PC,"Reservation table is Full!\r\n\r\n");
   }else{
      if(RESERVE_CHECK != 0x00)                                                  //if the table is in use
      {
         for(int num = 1; num < table_size - 6; num = num + 8)                   //check all command data placed in the table and update the time
         {
            if(reserve_table[num-1] != 0xff)                                     
            {
               if(reserve_table[num] > RESERVE_MIN_FLAG)
               {
                  reserve_table[num] = reserve_table[num] - RESERVE_MIN_FLAG;    //update the time of all commands
               }else{
                  reserve_table[num] = 0;
               }
            }
         }
         reserve_table[table_num] = in_bffr_main[4];                             //input the data about mission content
         reserve_table[table_num + 1] = in_bffr_main[5];                         //input data about time
         reserve_table[table_num + 2] = in_bffr_main[6];                         //input the data about mission detail mode
         reserve_table[table_num + 3] = in_bffr_main[7];                         //input address
         reserve_table[table_num + 4] = in_bffr_main[8];                         //input address
         reserve_table[table_num + 5] = in_bffr_main[9];                         //input address
         reserve_table[table_num + 6] = in_bffr_main[10];                        //input address
         reserve_table[table_num + 7] = in_bffr_main[11];                        //input packet number
         if(RESERVE_MIN_FLAG != 0)                                               //RESERVE_MIN_FLAG = 0 --> no need to update the target flag
         {
            RESERVE_TARGET_FLAG = RESERVE_TARGET_FLAG - RESERVE_MIN_FLAG;
            RESERVE_MIN_FLAG = 0;
         }
      }else{                                                                     //if first reservation
         RESERVE_MIN_FLAG = 0;
         RESERVE_SEC_FLAG = 0;
         reserve_table[table_num] = in_bffr_main[4];                             //input the data about mission content
         reserve_table[table_num + 1] = in_bffr_main[5];                         //input time data
         reserve_table[table_num + 2] = in_bffr_main[6];                         //input the data about mission mode detail
         reserve_table[table_num + 3] = in_bffr_main[7];                         //input address
         reserve_table[table_num + 4] = in_bffr_main[8];                         //input address
         reserve_table[table_num + 5] = in_bffr_main[9];                         //input address
         reserve_table[table_num + 6] = in_bffr_main[10];                        //input address
         reserve_table[table_num + 7] = in_bffr_main[11];                        //input packet number
      }
      //table_num = table_num + 3;
      //reserve_table[30] = table_num;
      sort_table();
//!      for(int num = 0; num < table_size+1; num++)
//!      {
//!         fprintf(PC,"%x",reserve_table[num]);//show for the test
//!      }
   }
   reserve_table[80] = count_reservation();                                      //update the table number data
   return;
}



void input_reservation_PC()                                                      //put the command from PC into the table and sort
{
//!   reserve_table[table_num] = in_bffr_main[4];
//!   reserve_table[table_num + 1] = in_bffr_main[5];
//!   reserve_table[table_num + 2] = in_bffr_main[6];
   fprintf(PC,"Table num: %x\r\n",reserve_table[80]);                            //print the table
   table_num = reserve_table[80];
   if(reserve_table[80] == 80)
   {
      fprintf(PC,"Reservation table is Full!\r\n\r\n");
   }else{
      if(RESERVE_CHECK != 0x00)                                                  //if new reservation is faster than target time
      {
         for(int num = 1; num < table_size - 6; num = num + 8)
         {
            if(reserve_table[num-1] != 0xff)                                     //check all command data placed in the table and update the time
            {
               if(reserve_table[num] > RESERVE_MIN_FLAG)
               {
                  reserve_table[num] = reserve_table[num] - RESERVE_MIN_FLAG;    //update the time of all commands
               }else{
                  reserve_table[num] = 0;
               }
            }
         }
         reserve_table[table_num] = CMD_FROM_PC[0];                              //input the data about mission content
         reserve_table[table_num + 1] = CMD_FROM_PC[1];                          //input reservation time
         reserve_table[table_num + 2] = CMD_FROM_PC[2];                          //input the data about mission detail mode
         reserve_table[table_num + 3] = CMD_FROM_PC[3];                          //input address
         reserve_table[table_num + 4] = CMD_FROM_PC[4];                          //input address
         reserve_table[table_num + 5] = CMD_FROM_PC[5];                          //input address
         reserve_table[table_num + 6] = CMD_FROM_PC[6];                          //input address
         reserve_table[table_num + 7] = CMD_FROM_PC[7];                          //input packet number
         
         if(RESERVE_MIN_FLAG != 0)                                               //RESERVE_MIN_FLAG = 0 --> no need to update the target flag
         {
            RESERVE_TARGET_FLAG = RESERVE_TARGET_FLAG - RESERVE_MIN_FLAG;
            RESERVE_MIN_FLAG = 0;
         }
      }else{                                                                     //if first reservation
         RESERVE_MIN_FLAG = 0;
         RESERVE_SEC_FLAG = 0;
         reserve_table[table_num] = CMD_FROM_PC[0];                              //input the data about mission content
         reserve_table[table_num + 1] = CMD_FROM_PC[1];
         reserve_table[table_num + 2] = CMD_FROM_PC[2];                          //input the data about mission detail mode
         reserve_table[table_num + 3] = CMD_FROM_PC[3];                          //input address
         reserve_table[table_num + 4] = CMD_FROM_PC[4];                          //input address
         reserve_table[table_num + 5] = CMD_FROM_PC[5];                          //input address
         reserve_table[table_num + 6] = CMD_FROM_PC[6];                          //input address
         reserve_table[table_num + 7] = CMD_FROM_PC[7];                          //input packet number
      }
      //table_num = table_num + 3;
      //reserve_table[30] = table_num;
      sort_table();
//!      for(int num = 0; num < table_size+1; num++)
//!      {
//!         fprintf(PC,"%x",reserve_table[num]);//show for the test
//!      }
   }
   reserve_table[80] = count_reservation();                                      //update the table number data
   return;
}



void Remove_1_Reservation()                                                      //after executing the mission, it should be finished
{
   for(int i = 1; i < table_size - 6; i = i + 8)
   {
      if(reserve_table[i-1] != 0xff)                                             //check whether command data is placed
      {
         reserve_table[i] = reserve_table[i] - RESERVE_TARGET_FLAG;              //update the time of all commands
      }
   }
   reserve_table[0] = 0xff;                                                      //delete the finished mission data, put 0xFF to move at the end of the table
   reserve_table[1] = 0xff;                                                      //delete the finished mission data
   reserve_table[2] = 0xff;                                                      //delete the finished mission data 
   reserve_table[3] = 0xff;                                                      //delete the finished mission data
   reserve_table[4] = 0xff;                                                      //delete the finished mission data
   reserve_table[5] = 0xff;                                                      //delete the finished mission data
   reserve_table[6] = 0xff;                                                      //delete the finished mission data
   reserve_table[7] = 0xff;                                                      //delete the finished mission data
   sort_table();
   reserve_table[80] = count_reservation();                                      //update the table number data
   
   fprintf(PC,"Removed finished mission command:");
   Save_RSV();
   fprintf(PC,"\r\n");
   return;
}

void Reserve_command_COM()                                                       //reservation from COM
{
   fprintf(PC,"Reserve CMD from COM\r\n");
   input_reservation_COM();                                                      //put the command from COM into the table and sort in ascending form
   Save_RSV();                                                                   //save reserve table to flash memory
   RESERVE_CHECK = 1;                                                            // table check flag in HIGH
   if(reserve_table[80] == 0x00 || reserve_table[80] == 0xff)
   {
      RESERVE_MIN_FLAG = 0;                                                      //reset time flag
   }
   MISSION_CONTENTS = reserve_table[0];                                          //closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];                                       //closest time data
   MISSION_DETAIL = reserve_table[2];                                            //closest mission command detail
   RESERVE_ADDRESS_1 = reserve_table[3];                                         //closest mission address
   RESERVE_ADDRESS_2 = reserve_table[4];                                         //closest mission address
   RESERVE_ADDRESS_3 = reserve_table[5];                                         //closest mission address
   RESERVE_ADDRESS_4 = reserve_table[6];                                         //closest mission address
   RESERVE_PACKET_NUM = reserve_table[7];                                        //closest mission pckt number
}



void Reserve_command_PC()                                                        //reservation from PC
{
   fprintf(PC,"Reserve CMD from PC\r\n");
   input_reservation_PC();                                                       //put the command from PC into the table and sort in ascending form
   Save_RSV();                                                                   //save reserve table to flash memory
   RESERVE_CHECK = 1;
   if(reserve_table[80] == 0x00 || reserve_table[80] == 0xff)                    //for only first reservation
   {
      RESERVE_MIN_FLAG = 0;                                                      //reset time flag
   }
   MISSION_CONTENTS = reserve_table[0];                                          //closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];                                       //closest time data
   MISSION_DETAIL = reserve_table[2];                                            //closest mission command detail
   RESERVE_ADDRESS_1 = reserve_table[3];                                         //closest mission address
   RESERVE_ADDRESS_2 = reserve_table[4];                                         //closest mission address
   RESERVE_ADDRESS_3 = reserve_table[5];                                         //closest mission address
   RESERVE_ADDRESS_4 = reserve_table[6];                                         //closest mission address
   RESERVE_PACKET_NUM = reserve_table[7];                                        //closest mission pckt number
}



void Reserve_next()                                                              //after finish execution of reservation command, this function should be execute
{
   fprintf(PC,"Reserve next\r\n");
   //Save_RSV();//save reserve table to flash memory
   
   //RESERVE_CHECK = 1;
   RESERVE_MIN_FLAG = 0;                                                         //reset time flag
   MISSION_CONTENTS = reserve_table[0];                                          //update closest mission command
   RESERVE_TARGET_FLAG = reserve_table[1];                                       //update closest time data (it is the time after 1 mission)
   MISSION_DETAIL = reserve_table[2];                                            //closest mission command detail
   RESERVE_ADDRESS_1 = reserve_table[3];                                         //closest mission address
   RESERVE_ADDRESS_2 = reserve_table[4];                                         //closest mission address
   RESERVE_ADDRESS_3 = reserve_table[5];                                         //closest mission address
   RESERVE_ADDRESS_4 = reserve_table[6];                                         //closest mission address
   RESERVE_PACKET_NUM = reserve_table[7];                                        //closest mission pckt number
}


