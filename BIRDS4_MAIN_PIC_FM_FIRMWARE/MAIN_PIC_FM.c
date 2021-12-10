
#include <MAIN_PIC_FM.h>

#int_timer0                                                                      //TMR0 interruption
void intval()
{   
   static int32 counter = 0;                                                     //the variable is defined as global, it maintains its value when entering and exiting the function
   counter++;                                                                    //it increases every 16.384ms
   if (counter > 60)                                                             //the counters inside increases every 1s
   {
      counter = 0;
//!      nowtime++;
//!      
//!      if(nowtime > 0)
//!      {
         //fprintf(PC,"%ld",nowtime);
         FAB_FLAG++;                                                             //time counter in seconds, at 90 seconds it requests data from the FAB
         RESERVE_SEC_FLAG++;                                                     //timer in seconds (used for cmds reservation)
         currenttime++;                                                          //timer in seconds (Reset Function)
         //fprintf(PC,"currenttime:%ld \r\n", currenttime);                      //for Checking TMR0 operation
         //fprintf(PC,"Done\r\n");
//!      }
      if(RESERVE_SEC_FLAG > 59)                                                  //the counters inside increases every 60s
      {
         RESERVE_SEC_FLAG = 0;
         RESERVE_MIN_FLAG++;                                                     //time counter in (used for cmds reservation)
      }
   }
}

#INT_rda                                                                         //PC Interrupt, data available in buffer 1
void UART1_RXD(void)
{
   CMD_FROM_PC[PC_DATA] = fgetc(PC);                                             //data sent by PC is writen in CMD_FROM_PC[] array
   PC_DATA = ((PC_DATA + 1) % 8);                                                //PC_DATA=0 when are received 8 bytes from pc
   //fputc(fgetc(PC),PC);
}


#INT_rda2 HIGH                                                                   //COM Interrupt, data available in buffer 2
void UART2_RXD(void)
{
   //SerialDataReceive();  //get 32 byte
   //COM_ONEBYTE_COMMAND = fgetc(COM);
   in_bffr_main[COM_DATA] = fgetc(COM);                                          //read COM data and saves in array in_bffr_main[]
//   fprintf(PC,"I receive from COM PIC: %x \r\n", in_bffr_main[COM_DATA]);
   COM_DATA = ((COM_DATA + 1) % 16);                                             //COM_DATA=0 when are received 16 bytes from COM
} 


#INT_rda3                                                                        //FAB Interrupt, data available in buffer 3
void UART3_RXD(void)
{
   //collect_HK_from_FAB();
   in_HK[FAB_DATA] = fgetc(FAB);                                                 //read FAB data and saves in the array in_HK[]
   FAB_DATA = ((FAB_DATA + 1) % FAB_SENSOR_size);                                //FAB_DATA=0 when are received 45 bytes from FAB
} 


#INT_rda4                                                                        //Reset PIC Interrupt, data available in buffer 4
void UART4_RXD(void)
{
   reset_bffr[RESET_DATA] = fgetc(reset);                                        //read ResetPIC data and saves in array reset_bffr
   RESET_DATA = ((RESET_DATA + 1) % RESET_size);                                 //RESET_DATA=0 when are received 11 bytes from FAB
   //fputc(fgetc(reset),PC);
   if(reset_bffr[0] == 0xaa)                                                     //get ready for reset satellite
   {
      reset_flag = 1;                                                            //pone a alto bandera para reseteo
      RESET_DATA = 0;                                                            //indicador de posicion dentro del vector reset_data
   }
   
} 

void settings()
{
   set_tris_b(0b11010000);                                                       // port b configuration
   enable_interrupts(global);                                                    // Enabled global interrupt      
   enable_interrupts(INT_rda);                                                   // Main to PC
   enable_interrupts(INT_rda2);                                                  // Main to COM PIC
   enable_interrupts(INT_rda3);                                                  // Main to FAB PIC   
   enable_interrupts(INT_rda4);                                                  // Main to RESET PIC

   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256 | RTCC_8_BIT);                     //Config TMR0, internal clock, prescaler 256, clock/counter 8bit
   set_timer0(0);                                                                //this sets timer0 register to 0
   
   READ_WRTITING_ADDRESS_LOCATION();                                             //save in ADD_INFO_ADDRESS with address info location
   RESERVE_CHECK = 0;                                                            //RSV cmd flag
   RESET_DATA = 0;                                                               
 
   CHECK_FLAG_INFO();                                                            //print flags status
   CHECK_ADDRESS_DATA();                                                         //print addresses
   
   BC_SETUP();                                                                   //configuration of the digital analog converter for temperature sensor reading
  
   output_low(PIN_A4);                                                           //kill switch off
   Turn_OFF_CAM();                                                               //Camera switch OFF, RD7=0
 //Turn_OFF_ADCS();                                                              //ADCS switch OFF, RD6=0  
   Turn_ON_ADCS();                                                               //ADCS switch ON, RD6=1
   Turn_ON_MBP();                                                                //Mission Boss switch ON, RF5=1
   Turn_OFF_BC();                                                                //Burner Circuit switch OFF, RD5=0

   fprintf(PC,"\r\n");
   CURRENT_FLAG_STATUS();                                                        //print flags status
   fprintf(PC,"\r\n");
   CURRENT_ADDRESS_OF_FLASH();                                                   //Print current address allocation
   CHANGE_ADDRESS_WRITING_LOCATION();                                            //change writing direction when 95,000 cycles are exceeded

   output_high(PIN_C4);                                                          //MUX: COM side (MAIN-COM)
   output_low(PIN_A5);                                                           //MUX: Main side (MAIN-MISSION)
  
   Get_RSV();                                                                    //read the reservation table info from flash memory  
   SAVE_SAT_LOG(0x25,0x25,0x25);                                                 //0X25 0X25 0X25 SATELLITE RESET INDICATOR  
   return;                                                                       
  
}                                  

///////////TEST PART//////////////////////////
//#org DEFAULT 
void main()
{

   settings();
   enable_interrupts(INT_TIMER0);
   fprintf(PC,"Start Operating\r\n");
   
   FAB_TEST_OPERATION();                                                         //load the FABDATA[] array, create the CW format and save them to Flash
   
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save the address data in flash, it will be saved in a new sector if 95000 cycles are fulfilled
   CURRENT_ADDRESS_OF_FLASH();                                                   //print the Flash memory address allocation
   STORE_FLAG_INFO();                                                            //save flag data to flash memory (in OF, SCF and SMF)
   WRITE_FLAG_to_EEPROM();                                                       //save the flags in the EEPROM from the address 0x18000 (75%)

  
   fprintf(PC,"\r\nCW:");
   for(int i = 0; i < 4; i++)                                                    //print CW_FORMAT[] array
   {
      fprintf(PC,"%x,",CW_FORMAT[i]);
   }
   fprintf(PC,"%x\r\n",CW_FORMAT[4]);
   Antenna_Deploy();                                                             //after each restart check BC_ATTEMPT_FLAG to deploy antenna
   //CAM deployment
//!   if(PASSED_DAYS == 0 && UPLINK_SUCCESS == 0 && HKDATA[44] >= 0x90)          //automated CAM DEPLOYMENT mission, condition battvolt > 3.7v
//!   {  
//!      CAM_DEPLOYMENT_OPERATION(0xAD);                                         //CMD for CAM deployment mode
//!   }
   //try 5 times for reliability
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   AUTOMATICAL_MISSION_CHECK();
   RESET_DATA = 0;                                                               

   delay_ms(5000);
   ADCS_TLE_TRANSFER();
   delay_ms(500);
   ADCS_GAIN_TRANSFER();
   delay_ms(500);
   ADCS_UTC_TRANSFER();
   
   
   while(true)
   {

      BC_ON_30min();                                                             //if satellite shut down during automatical MSN, this function will be used after start again      
      RESET_SATELLITE();                                                         //check reset command from RESET PIC
      
      if(FAB_FLAG > 89)                                                          //every 90 sec, OBC gather sensor data and update CW format 
      {
         FAB_FLAG = 0;
         fprintf(PC,"\r\n***90sec passed***\r\n");
         FAB_TEST_OPERATION();                                                   //writes FABDATA[] array, makes CW format saves in Flash
         COM_DATA = 0;
         STORE_ADRESS_DATA_TO_FLASH();                                           //for store the address info
         fprintf(PC,"CW:");
         for(int i = 0; i < 4; i++)                                              //show CW format
         {
            fprintf(PC,"%x,",CW_FORMAT[i]);
         }
         fprintf(PC,"%x\r\n",CW_FORMAT[4]);
         DELETE_CMD_FROM_PC();                                                   //delete PC command
         PC_DATA = 0;                                                           
         COM_DATA = 0;                                                          
         RESET_DATA = 0;                                                         
      }   
      
      if((RESERVE_MIN_FLAG >= RESERVE_TARGET_FLAG) && RESERVE_CHECK == 1)        //check the reservation command, if time came, execute
      {
         fprintf(PC,"Execute reserved command\r\n");
         MISSION_CONTENTS = CHECK_MEMORY_FUNCTION(MISSION_CONTENTS);             //avoid erase or transfer sectors from memory as reserved commands
         EXECUTE_MISSION_from_COM(MISSION_CONTENTS,MISSION_DETAIL,RESERVE_ADDRESS_1,RESERVE_ADDRESS_2,RESERVE_ADDRESS_3,RESERVE_ADDRESS_4,RESERVE_PACKET_NUM);    //execute command
         Remove_1_Reservation();                                                 //remove the finished command, sort again and save updated command table
         if(reserve_table[80] != 0x00)                                           //if next reservation is registered, wait until time come
         {
            Reserve_next();                                                      //take the next CMD from the table
         }else{                                                                  //if all reservation finished, reset flag about reservation
            RESERVE_TARGET_FLAG = 0;                                             //reset flag
            RESERVE_CHECK = 0;
            MISSION_CONTENTS = 0;
            MISSION_DETAIL = 0;
         }
      }
      
      if(buffer_from_com)                                                        //If TRU -> (in_bffr_main[0]==0xAA) && (in_bffr_main[15]==0xBB)
      {
         fprintf(PC,"COM COMMAND:\r\n");
         for(COM_DATA = 0; COM_DATA < 16; COM_DATA++)                            //display command in PC
         {
            //fputc(in_bffr_main[COM_DATA],PC);
            fprintf(PC,"%x,",in_bffr_main[COM_DATA]);                            //display in PC
         }
         COM_DATA = 0;                                                           //position indicator for the vector in_bffr_main[]
         //fprintf(PC,"COM COMMAND\r\n");
         if(in_bffr_main[5] == 0)                                                //if time data is not placed
         {
            EXECUTE_MISSION_from_COM(in_bffr_main[4],in_bffr_main[6],in_bffr_main[7],in_bffr_main[8],in_bffr_main[9],in_bffr_main[10],in_bffr_main[11]);
         }else{                                                                  //if time data is placed (that means reservation command)
            SAVE_SAT_LOG(in_bffr_main[4],in_bffr_main[5],in_bffr_main[6]);       //reservation command log
            Reserve_command_COM();                                               //used when in_bffr_main[5] has a Time value
         }
         DELETE_CMD_FROM_PC();                                                   //set to zero the array CMD_FROM_PC[]
         Delete_Buffer();                                                        //set to zero the array in_bffr_main[]
         CMD_FROM_PC[1] = 0;
         COM_DATA = 0;                                                           //position indicator for the vector in_bffr_main[]
         PC_DATA = 0;                                                            //position indicator for the vector CMD_FROM_PC[]
      }
      
//COMMANDS EXECUTION FROM PC//       
      
      if(CMD_FROM_PC[0])
      {
         fprintf(PC,"COMMAND RECEIVED FROM PC: ");
         for(int m = 0; m < 8; m++)
         {
            fprintf(PC,"%x",CMD_FROM_PC[m]);
         }
         fprintf(PC,"\r\n");
         
         if(CMD_FROM_PC[1] == 0)                                                 //if time data is not placed
         {
            EXECUTE_MISSION_from_PC(CMD_FROM_PC[0],CMD_FROM_PC[2],CMD_FROM_PC[3],CMD_FROM_PC[4],CMD_FROM_PC[5],CMD_FROM_PC[6],CMD_FROM_PC[7]);//execute mission
         }else{                                                                  //if time data is placed (that means reservation command)
            SAVE_SAT_LOG(CMD_FROM_PC[0],CMD_FROM_PC[1],CMD_FROM_PC[2]);          //reservation command log
            Reserve_command_PC();                                                //mission command reservation from PC comand
         }
         DELETE_CMD_FROM_PC();                                                   //clear CMD_FROM_PC[] array
         Delete_Buffer();                                                        //clear in_bffr_main[] array
         CMD_FROM_PC[1] = 0;
         COM_DATA = 0;                                                           //clear COM correct receiving data flag
         PC_DATA = 0;                                                            //clear PC correct receiving data flag
         for(int k = 14; k < 23; k++)                                            //clear RSV part
         {
            ACK_for_COM[k] = 0x00;
         }
      }
      
      if(COM_DATA != 0 || PC_DATA != 0)                                          //COM_DATA AND PC_DATA MUST BE ZERO IF THE CORRECT NUMBER OF CHARACTERS ARE RECEIVED
      {
         delay_ms(500);
         if(PC_DATA != 0)                                                        //If the correct amount of characters is not received, it sets the CMD_FROM_PC array to zero.
         {
            CMD_FROM_PC[0] = 0;
            CMD_FROM_PC[1] = 0;
            CMD_FROM_PC[2] = 0;
            CMD_FROM_PC[3] = 0;
            CMD_FROM_PC[4] = 0;
            CMD_FROM_PC[5] = 0;
            CMD_FROM_PC[6] = 0;
            CMD_FROM_PC[7] = 0;
            COM_DATA = 0;
            PC_DATA = 0;
         }
      }
      delay_ms(400); 
   
   }

}


