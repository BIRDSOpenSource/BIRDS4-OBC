#define loop_day 36
int16 AD = 0;                                                                    //DATE DATA
int16 D = 0;                                                                     //everyday change the DL address. Increase 5 or 10 packets for each day
                                                                                 
int8 ADH = 0;                                                                    //for satellite log(int16 date high)
int8 ADL = 0;                                                                    //for satellite log(int16 date low)
int32 DL_AD = 0;                                                                 //Data size to increase when day changed

//@@@@@@@@@@@ HIGH SAMPLING@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void DAY0()                                                                      //2 hour high sampling sensor
{
   SAVE_SAT_LOG(0xFA,0x11,0x11);                                                 //11,11: automatical mission/HSSC
   FIRST_HSSC_DONE = 1;                                                          //flag in HIGH
   STORE_FLAG_INFO();                                                            //save passed days
   WRITE_FLAG_to_EEPROM();
   HIGHSAMP_SENSOR_COLLECTION(1440);                                             //execute HSSC mission for 2.5 minutes
   STORE_ADRESS_DATA_TO_FLASH();                                                 // save all address data in FLASH memories
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xFD,ADH,ADL);                                                   //11,11: automatical mission
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639,810);                                  //send data1,data2,address,size to COM for automatical mission)                                                                             //118byte*3600*2/5 (about 2.59sector) 65536*1639
   return;                                                                       
}

void DAY1()                                                                      //CAM mission and download                                                                      
{
   SAVE_SAT_LOG(0xC0,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_CAM_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   CAM_TEST_OPERATION(0xA0);                                                     //cam mission 320x240 low quality
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xCD,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8,810);                                     //65536*8
   return;                                                                       
}

void DAY2()                                                                      //PSC
{
   SAVE_SAT_LOG(0xD1,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_PSC_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD1,0xA9,0x00,0x00,0x00,0x00,0x00);                   //PSC mission ON.
   delay_ms(60000);                                                               //delay time TBD
   MISSION_TEST_OPERATION(0xD1,0xA0,0x00,0x0,0x00,0x00,0x00);                    //PSC mission OFF.
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   DATACOLLECTION_TEST_OPERATION(0xD0,0x00,0x00,0x10,0x00,0x00,0x0C);            //PSC data transfer 12 packets
   //delay_ms(60000);                                                            //In case it's necesary before start downloading 
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD1,ADH,ADL);                                                   //11,11: automatical mission
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1638,972);                                  //PSC data download, 12 pckts
   MISSION_TEST_OPERATION(0xDF,0x00,0x00,0x10,0x00,0x00,0x00);                   //ERASE PSC data transferred.
   DELAY_MS(5000);
   MISSION_TEST_OPERATION(0xDF,0x00,0x00,0x00,0x00,0x00,0x00);                   //reset address to save for PSC
   return;                                                                     
}


void DAY3to4()                                                                   //PSC
{
   SAVE_SAT_LOG(0xD1,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_PSC_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD1,0xA9,0x00,0x00,0x00,0x00,0x00);                   //PSC mission ON.
   delay_ms(60000);
   MISSION_TEST_OPERATION(0xD1,0xA0,0x00,0x0,0x00,0x00,0x00);                    //PSC mission OFF.
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   DATACOLLECTION_TEST_OPERATION(0xD0,0x00,0x00,0x10,0x00,0x00,0x0C);            //PSC data transfer 12 packets
   //delay_ms(60000);                                                            //In case it's necesary before start downloading 
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD1,ADH,ADL);                                                   //11,11: automatical mission
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1638,972);                                  //PSC data download, 12 pckts
   MISSION_TEST_OPERATION(0xDF,0x00,0x00,0x10,0x00,0x00,0x00);                   //ERASE PSC data transferred.
   DELAY_MS(5000);
   MISSION_TEST_OPERATION(0xDF,0x00,0x00,0x00,0x00,0x00,0x00);                   //reset address to save for PSC
   return;                                                                     
}

void DAY5()                                                                     //SFWARD
{
   SAVE_SAT_LOG(0xD4,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_SFWARD_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD4,0xA9,0x00,0x00,0x00,0x00,0x00);                   //SFWARD mission ON for 23hs.
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD4,ADH,ADL);                                                   //11,11: automatical mission
   return;                                                                     
}

void DAY6()                                                                     //SFWARD
{
   SAVE_SAT_LOG(0xD4,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_SFWARD_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD4,0xA9,0x00,0x00,0x00,0x00,0x00);                   //SFWARD mission ON for 23hs.
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD4,ADH,ADL);                                                   //11,11: automatical mission
   return;                                                                     
}

void DAY7()                                                                      //SFWARD
{
   //////SFWARD download part////////
   MISSION_TEST_OPERATION(0xD4,0xA0,0x00,0x00,0x00,0x00,0x00);                   //SFWARD Turn OFF   
   MISSION_TEST_OPERATION(0xD4,0xA2,0x00,0x01,0x10,0x00,0x0A);            //SFWARD data transfer to MBPIC
   delay_ms(60000);                                                            //In case it's necesary before start downloading
   delay_ms(60000);                                                            //In case it's necesary before start downloading
   MISSION_TEST_OPERATION(0xD4,0xA5,0x00,0x10,0x00,0x00,0x00);            //ERASE SFWARD data transfer for the next set
   DATACOLLECTION_TEST_OPERATION(0xD0,0x05,0x57,0x00,0x00,0x00,0x0A);            //SFWARD data transfer to MainPIC, 10 pckts
//   delay_ms(60000);                                                            //In case it's necesary before start downloading
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD4,ADH,ADL);                                                   //11,11: automatical mission
   
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1638,810);                                  //SFWARD data download, 10 pckts
   MISSION_TEST_OPERATION(0xDF,0x05,0x57,0x00,0x00,0x00,0x00);                   //ERASE SFWARD data transferred.
   DELAY_MS(5000);
   MISSION_TEST_OPERATION(0xDF,0x05,0x55,0xFA,0xAB,0x00,0x00);                   //reset address to save for PSC 0555FAAB
   //////end SFWARD//////
   return;
}
void DAY8()                                                                      //HNT
{
   SAVE_SAT_LOG(0xD5,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_HNT_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD5,0xA2,0x03,0xB6,0x00,0x00,0x00);                   //HNT compare mode mission execution for 24hs- 950 comparisons
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD5,ADH,ADL);                                                   //11,11: automatical mission
   return;
}
void DAY9()                                                                      //APRSDP                                                                      
{
   MISSION_TEST_OPERATION(0xD5,0xA0,0x00,0x00,0x00,0x00,0x00);                   //HNT Turn OFF
   SAVE_SAT_LOG(0xD6,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_APRSDP_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD6,0xA9,0x00,0x00,0x00,0x00,0x00);                   //APRSDP mission execution. setted to execute for 23hs
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD6,ADH,ADL);                                                   //11,11: automatical mission
   return;
}

void DAY10()                                                                     //TMCR                                                                      
{
   MISSION_TEST_OPERATION(0xD6,0xA0,0x00,0x00,0x00,0x00,0x00);                   //APRSDP Turn OFF
   SAVE_SAT_LOG(0xD2,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_TMCR_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD2,0xA9,0x00,0x00,0x00,0x00,0x00);                   //TMCR mission execution. all devices measured every 6hs
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days

   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD2,ADH,ADL);                                                   //11,11: automatical mission
   return;
   
}
void DAY11()                                                                      //NTU                                                                      
{
   //////TMCR download part////////
   MISSION_TEST_OPERATION(0xD2,0xA0,0x00,0x00,0x00,0x00,0x00);                   //TMCR Turn OFF
   DATACOLLECTION_TEST_OPERATION(0xD0,0x02,0xAB,0x10,0x00,0x00,0x05);            //TMCR data transfer 
   //delay_ms(60000);                                                            //In case it's necesary before start downloading
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD2,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1638,405);                                  //TMCR data download
   MISSION_TEST_OPERATION(0xDF,0x02,0xAB,0x10,0x00,0x00,0x00);                   //ERASE TMCR data transferred.
   DELAY_MS(5000);
   MISSION_TEST_OPERATION(0xDF,0x02,0xAA,0xFD,0x57,0x00,0x00);                   //reset address to save for PSC 0555FAAB
   
   //////end TMCR//////
   
   SAVE_SAT_LOG(0xD3,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_NTU_DONE = 1;
   AUTO_MBP_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD3,0xA9,0x00,0x00,0x00,0x00,0x00);                   //NTU mission execution. Turned ON indefinitely
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD3,ADH,ADL);                                                   //11,11: automatical mission
   
   return;    
                                                                        
}

 void DAY12()                                                                    //NTU                                                                      
{
   SAVE_SAT_LOG(0xD3,0x11,0x11);                                                 //11,11: automatical mission
   AUTO_NTU_DONE = 1;
   STORE_FLAG_INFO();                                                            //save flag
   WRITE_FLAG_to_EEPROM();
   MISSION_TEST_OPERATION(0xD3,0xA9,0x00,0x00,0x00,0x00,0x00);                   //NTU mission execution. Turned ON indefinitely
   STORE_ADRESS_DATA_TO_FLASH();                                                 //save passed days
   
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD3,ADH,ADL);                                                   //11,11: automatical mission

   return;                                                                         
}                                                                                   

void DAY13()                                                                      //NTU                                                                      
{
   
   //////NTU download part////////
   delay_ms(60000);
   delay_ms(60000);
   delay_ms(60000);
   MISSION_TEST_OPERATION(0xD3,0xA0,0x00,0x00,0x00,0x00,0x00);                   //NTU Turn OFF
   DATACOLLECTION_TEST_OPERATION(0xD0,0x04,0x01,0x10,0x00,0x00,0x05);            //NTU data transfer
   //delay_ms(60000);                                                            //In case it's necesary before start downloading
   AD = PASSED_DAYS % loop_day;                                                  //1,2,3,4
   ADH = AD << 8;
   ADL = AD;
   SAVE_SAT_LOG(0xD3,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1638,405);                                  //NTU data download
   MISSION_TEST_OPERATION(0xDF,0x04,0x01,0x10,0x00,0x00,0x00);                   //ERASE NTU data transferred.
   DELAY_MS(5000);
   MISSION_TEST_OPERATION(0xDF,0x04,0x00,0xFC,0x00,0x00,0x00);                   //reset address to save for PSC 0555FAAB
   //////end NTU//////

   return;                                                                         
}


void DAY14to16()                                                                 //HSSC mission 124*3600*2/5 total packts = 2205
{
   MISSION_TEST_OPERATION(0xD3,0xA0,0x00,0x00,0x00,0x00,0x00);                   //NTU Turn OFF   
   AD = PASSED_DAYS % loop_day;                                                  //14,15,16
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 13;                                                                  //1,2,3
   DL_AD = 730*D;                                                                //jumps of 730 pckts
   DL_AD = DL_AD * 81;
   /////////PACKET/////////////////
   //730-739, 1460-1469, 2190-2199
   ///////////////////////////////
   
   SAVE_SAT_LOG(0xFD,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*1639+DL_AD,810);                            // 10 pckts per day
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}

void DAY17to27()                                                                 //CAM mission total packts = 120
{
   AD = PASSED_DAYS % loop_day;                                                  //17,18,19,20,21,22,23,24,25,26,27
   ADH = AD << 8;
   ADL = AD;
   
   D = AD - 16;                                                                  //substracted 16 to continue downloading from pckt 10, the first 10 are downloaded in day 1 
   DL_AD = D*10;                                                                  //jumps of 10 pckts
   DL_AD = DL_AD * 81;
   ///////////////////////PACKET/////////////////////////
   //10-19,   20-29,   30-39,   40-49,   50-59
   //60-69,   70-79,   80-89,   90-99,   100-109
   //110-119
   //----------------------------------------------------
   //total is 120 packet
   //////////////////////////////////////////////////////
   
   /////////////////////////////BYTE/////////////////////////
   //810-1619, 1620-2429, 2430-3239, 3240-4049, 4050-4859
   //4860-5669, 5670-6479, 6480-7289, 7290-8099, 8100-8909
   //8910-9719
   //--------------------------------------------------------
   //total is about 10kB
   //////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xCD,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*8+DL_AD,810);                               //65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;
}


void DAY28to30()                                                                  //SAT LOG, total dta pckts = 30                                                                  
{
   AD = PASSED_DAYS % loop_day;                                                  //28,29,30
   ADH = AD << 8;
   ADL = AD;
   D = AD - 28;                                                                  //0,1,2
   
   DL_AD = D*10;
   DL_AD = DL_AD * 81;
   /////////////PACKET//////////
   //0-9, 10-19, 20-29
   ////////////////////////////
   
   SAVE_SAT_LOG(0xAD,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*6+DL_AD,810);                               //65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;                                                                       
}

void DAY31to35()                                                                  //SAT HK total pckts 52900 -> 124*3600*24*36/90                                                                     
{
   AD = PASSED_DAYS % loop_day;                                                  //31,32,33,34,35
   ADH = AD << 8;
   ADL = AD;
     
   D = AD - 31;                                                                  //0,1,2,3,4
   DL_AD = 13000*D;                                                               //jumps of 14600 pckts
   DL_AD = DL_AD * 81;
   /////////////////////////////PACKET//////////////////////////////
   //    0-10, 13000-13009, 26000-26009, 39000-39009, 52000-52009
   /////////////////////////////////////////////////////////////////
   
   SAVE_SAT_LOG(0xAD,ADH,ADL);                                                   //11,11: automatical mission
   UPDATE_ACK_for_COM(0x66,0x77,SECT*98+DL_AD,810);                              //65536*8
   ADH=0; ADL=0; DL_AD=0; D=0;
   return;                                                                       
}



//@@@@@@@@@@@ ADDRESS CHANGE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void AUTO_MSN_AD_CHANGE()                                                        //update address for a new cycle acording to data size and loop day
{
   unsigned int16 D = PASSED_DAYS/loop_day;                                      //1,2,3... cycles
   int8 UPDATE_TIME = (int8)D;
   SAVE_SAT_LOG(0xF9,UPDATE_TIME,UPDATE_TIME);
   
   int32 DATA = 2430*D;                                                          //11byte*220 times = 30 pckts, SAT_LOG data sent in one cycle
   SAT_LOG = SECT*6 + DATA;                                                      //2 sectors assigned
   
   DATA = SECT*D;                                                                //1sector, CAM data sent in one cycle
   CAM_ADDRESS = SECT*8 + DATA;
   
   DATA = SECT*66;                                                              //124*3600*24*36/90 < 66sectors, HK data sent in one cycle (Loop = 36)
   DATA = DATA*D;                                                                
   FAB_HK_ADDRESS = SECT*98 + DATA;                                              //1000sectors assigned

   
   DATA = SECT*3;                                                                //124*3600*2/5 < 3 sectors, HSSC data sent in one cycle
   DATA = DATA*D;
   HIGH_SAMP_HK_ADDRESS = SECT*1639 + DATA;                                      //409 sectors assigned
   
   STORE_ADRESS_DATA_TO_FLASH();                                                 //update
   
   return;
}


//@@@@@@@@@@@ AUTO MISSION EXECUTING FUNCTION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void AUTOMATICAL_MSN_EXE()
{
   //PASSED_DAYS = make16(reset_bffr[4],reset_bffr[5]);
   if(((PASSED_DAYS%loop_day) >= 0 && UPLINK_SUCCESS == 0) && FIRST_HSSC_DONE == 0)                      //automated high samp mission condition
   {
      DAY0();
   }else if(PASSED_DAYS < make16(reset_bffr[4],reset_bffr[5]))                                           //if the date changed (next day)
   {
      PASSED_DAYS = make16(reset_bffr[4],reset_bffr[5]);                                                 //update passed days
      STORE_FLAG_INFO();                                                                                 //save passed days
      WRITE_FLAG_to_EEPROM();
      
      
      if(((PASSED_DAYS%loop_day) >= 1 && UPLINK_SUCCESS == 0) && AUTO_CAM_DONE == 0){                    //1st day CAM MSN and 10 pckts download 
         DAY1();
      }else if(((PASSED_DAYS%loop_day) == 2 && UPLINK_SUCCESS == 0) && AUTO_PSC_DONE == 0){              //2nd day PSC MSN and 10 pckts download
         DAY2();
      }else if(((PASSED_DAYS%loop_day) == 3 || (PASSED_DAYS%loop_day) == 4) && UPLINK_SUCCESS == 0){     //PSC MSN and 10 pckts download
         DAY3to4();
      }else if(((PASSED_DAYS%loop_day) == 5 && UPLINK_SUCCESS == 0) && AUTO_SFWARD_DONE == 0){           //5th day SFWARD MSN save data
         DAY5();
      }else if(((PASSED_DAYS%loop_day) > 5 && (PASSED_DAYS%loop_day) < 7) && UPLINK_SUCCESS == 0){      //6th day SFWARD MSN save data
         DAY6();
      }else if(((PASSED_DAYS%loop_day) > 6 && (PASSED_DAYS%loop_day) < 8) && UPLINK_SUCCESS == 0){      //7th day SFWARD 10 pckts download  
         DAY7();
      }else if(((PASSED_DAYS%loop_day) >= 8 && UPLINK_SUCCESS == 0) && AUTO_HNT_DONE == 0){              //8th day HNT mission, no data download 
         DAY8();
      }else if(((PASSED_DAYS%loop_day) >= 9 && UPLINK_SUCCESS == 0) && AUTO_APRSDP_DONE == 0){           //8th day APRSDP mission, no data download 
         DAY9();   
      }else if(((PASSED_DAYS%loop_day) >= 10 && UPLINK_SUCCESS == 0) && AUTO_TMCR_DONE == 0){            //10th day TMCR mission, data reading
         DAY10();   
      }else if(((PASSED_DAYS%loop_day) >= 11 && UPLINK_SUCCESS == 0) && AUTO_NTU_DONE == 0){             //11th day NTU mission data reading and TMCR 5 pckts download 
         DAY11();
      }else if(((PASSED_DAYS%loop_day) > 11 && (PASSED_DAYS%loop_day) < 13) && UPLINK_SUCCESS == 0){      //12th day NTU mission data reading  
         DAY12();
      }else if(((PASSED_DAYS%loop_day) > 12 && (PASSED_DAYS%loop_day) < 14) && UPLINK_SUCCESS == 0){      //13th day NTU 5 pckts download  
         DAY13();
      }else if(((PASSED_DAYS%loop_day) >= 14 && (PASSED_DAYS%loop_day) <= 16) && UPLINK_SUCCESS == 0){   //High sampling sensor collecting mission download
         DAY14to16();
      }else if(((PASSED_DAYS%loop_day) >= 17 && (PASSED_DAYS%loop_day) <= 27) && UPLINK_SUCCESS == 0){   //CAM Mission Download
         DAY17to27();
      }else if(((PASSED_DAYS%loop_day) >= 28 && (PASSED_DAYS%loop_day) <= 30) && UPLINK_SUCCESS == 0){   //Satellite LOG Download
         DAY28to30();
      }else if(((PASSED_DAYS%loop_day) >= 31 && (PASSED_DAYS%loop_day) <= 35) && UPLINK_SUCCESS == 0){   //Normal HK Download
         DAY31to35();
      }

   }
   if(BC_ATTEMPT_FLAG == 0)                                                      //at this moment, this flag should be over 1
   {
      BC_ON_30min();
   }
   
   FAB_FLAG = 0;
   return;
}


//@@@@@@@@@@@ AUTO MISSION OPERATION @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void AUTOMATICAL_MISSION_CHECK()
{
   int c;
   for(c = 0; c < 6; c++)
   {
      COLLECT_RESET_DATA();                                                      //send cmd to get reset data
      if(reset_bffr[0] == 0x8e)
      {
         break;
      }
   }
   
   for(c = 0; c < 10; c++)                                                       //print reset data
   {
      fprintf(PC,"%x,",reset_bffr[c]);
   }
   fprintf(PC,"%x\r\n",reset_bffr[10]);
   
   if(((PASSED_DAYS%loop_day==0)&&(PASSED_DAYS>36))&&UPLINK_SUCCESS == 0)        //after one cycle, auto mission again. Loop value = 36
   {
      FIRST_HSSC_DONE = 0;
      AUTO_CAM_DONE = 0;
      AUTO_MBP_DONE = 0;
      AUTO_ADCS_DONE = 0;
      AUTO_HNT_DONE = 0;                                                         //Automatic HNT mission done
      AUTO_APRSDP_DONE = 0;                                                      //Automatic APRSDP mission done
      AUTO_TMCR_DONE = 0;                                                        //Automatic TMCR data download
      AUTO_NTU_DONE = 0;                                                         //Automatic NTU data download
      AUTO_PSC_DONE = 0;                                                         //Automatic PSC data download
      AUTO_SFWARD_DONE = 0;                                                      //Automatic SFWARD data download
      
      AUTO_MSN_AD_CHANGE();                                                      //update address for a new cycle
      STORE_FLAG_INFO();                                                         //save flag data to flash memory
      WRITE_FLAG_to_EEPROM();                                                    //saves the flags in the EEPROM from the address 0x18000 (75%)
   }
   AUTOMATICAL_MSN_EXE();
   return;
}

