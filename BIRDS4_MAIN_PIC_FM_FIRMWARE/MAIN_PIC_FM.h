#include <18F67J94.h>
#FUSES NOWDT, NOBROWNOUT, NOPROTECT
#use delay(crystal=16Mhz, clock=16Mhz)
#include<PIC18F67J94_registers.h>

//#include<HAMING_CORRECT.h>
//#org 0x100, 0x11000 DEFAULT 
//#org DEFAULT
    
#device HIGH_INTS=TRUE

//-------------UART and SPI Setting--------------------------------------------

#pin_select TX1=PIN_C6                                                           //TX PIN EXT +/- 6v
#pin_select RX1=PIN_C7                                                           //RX PIN EXT +/- 6V
#use rs232(baud=115200,parity=N,UART1,bits=8,stream=pc)                          // MAIN to Ext +/- 6v

//#use rs232(baud=9600,parity=N,xmit=PIN_B6,rcv=PIN_B7,bits=8,stream=pc)           //UART MAIN to ICSP, PGC and PGD pins

#pin_select TX2=PIN_D3
#pin_select RX2=PIN_D2
#use rs232(baud=9600,parity=N,UART2,bits=8,stream=com,ERRORS)                           //UART Main to COM PIC

#pin_select TX3=PIN_E5  
#pin_select RX3=PIN_E4
#use rs232(baud=9600,parity=N,UART3,bits=8,stream=fab,ERRORS)                           //UART MAIN to FAB PIC

#pin_select TX4=PIN_E3  
#pin_select RX4=PIN_F2
#use rs232(baud=9600,parity=N,UART4,bits=8,stream=reset,ERRORS)                         //UART MAIN to RESET PIC

#use rs232(baud=9600,parity=N,xmit=pin_F7,rcv=pin_F6,bits=8,stream=ADCS,ERRORS)         //UART MAIN to ADCS
#use rs232(baud=9600,parity=N,xmit=pin_G2,rcv=pin_G3,bits=8,stream=CAM,ERRORS)          //UART MAIN to CAM
#use rs232(baud=9600,parity=N,xmit=pin_G0,rcv=pin_G1,bits=8,stream=DC,ERRORS)          //UART MAIN to MB DCM

#use spi(MASTER, CLK=PIN_E1, DI=PIN_E0, DO=PIN_E6,  BAUD=1000000, BITS=8, STREAM=MAIN_FM, MODE=0)        //MAIN flash memory port
#use spi(MASTER, CLK=PIN_B2, DI=PIN_B5, DO=PIN_B4,  BAUD=1000000, BITS=8, STREAM=COM_FM, MODE=0)         //COM flash memory port
#use spi(MASTER, CLK=PIN_A3, DI=PIN_A0, DO=PIN_A1,  BAUD=1000000, BITS=8, STREAM=MISSION_FM, MODE=0)     //MISSION flash memory port

//-------------mainpic_function.c----------------------------------------------



void Check_UART();
void waiting(int32 counterval);
//void Turn_On_PWM(int a)
//void Turn_OFF_PWM()


/////////////MAIN PIC Buffer/////////////
void SerialDataReceive();                                                        //get buffer data one by one
void Delete_Buffer();                                                            //get buffer data one by one
void Transfer_Buffer(int PORT_NUM);                                              //get buffer data one by one

SerialDataReceive();
Transfer_Buffer(int PORT_NUM); //1:PC 2:COMPIC 3:RESETPIC 4:ADCS 5:S&F 6:CAM

/////////////RESET SENSOR collection/////////////

void DELETE_RESET();
void COLLECT_RESET_DATA();

/////////////FAB SENSOR collection/////////////
void Delete_in_HK();
void Delete_HKDATA();
void Delete_CWFORMAT();
void MAKE_CW_FORMAT();
void SAVE_HKDATA_TO_OF(unsigned int32 Memory_Adress);
void SAVE_HKDATA_TO_SCF(unsigned int32 Memory_Adress);
void SAVE_CWFORMAT_TO_OF(unsigned int32 Memory_Adress);
void SAVE_CWFORMAT_TO_SCF(unsigned int32 Memory_Adress);
void Send_COMMAND_TO_FAB(int8 cmd);
int8 ACK_FROM_FAB();
void Send_HKDATA_to_SCF(int32 adress);
void Send_HKDATA_to_OF(int32 adress);
void SEND_CWFORMAT_TO_SCF(int32 adress);
void SEND_CWFORMAT_TO_OF(int32 adress);
void Send_HKDATA_to_PC(int32 adress);
void CHECK_HKDATA(int8 in);
void VERIFY_FABDATA(int32 delaytime1,int32 delaytime2);
void FAB_TEST_OPERATION();
void GET_RESET_DATA();

///////////////HIGH SAMPLING HK collection//////////////
void SAVE_HIGH_HKDATA_TO_SCF(unsigned int32 Memory_Adress);
void Send_HIGH_HKDATA_to_SCF(int32 adress);
void SAVE_HIGH_HKDATA_TO_OF(unsigned int32 Memory_Adress);
void Send_HIGH_HKDATA_to_OF(int32 adress);
void CHECK_HIGH_SAMP_FABDATA(int8 in);
void VERIFY_HIGH_SAMP_FABDATA(int32 delaytime);
void GET_HIGH_SAMP_RESET_DATA();
void HIGH_SAMP_FAB_OPERATION();
void HIGHSAMP_SENSOR_COLLECTION(int16 second);

/////////////ANTENNA DEPLOYMENT/////////////
void BC_SETUP();
void BC_READ_TO_PC();
void CHECK_BC_TEMP();
void Turn_ON_BC();
void Turn_OFF_BC();
void CHECK_POWER();
void BC_OPERATION();
void CLEAR_BC_FLAG();
void MAKE_BC_FLAG_1();
void MAKE_BC_FLAG_2();
void MAKE_BC_FLAG_3();
void MAKE_BC_FLAG_4();


/////////////CAM MISSION/////////////
void Turn_On_CAM();
void Turn_Off_CAM();
void Send_Command_to_CAM(int8 data);
int8 ACK_from_CAM();
void Transfer_Photo();
void CAM_TEST_OPERATION(int8 mode);
void CAM_DEPLOYMENT_OPERATION(int8 mode);
void CAM_SYNC();

/////////////ADCS MISSION/////////////

void Turn_On_ADCS();
void Turn_Off_ADCS();
void Send_Command_to_ADCS(int8 data);
void DELETE_ADCS_SENSOR();
void ADCS_SETTINGS();                                                            //initiallize
void DISPLAY_ADCS_SENSOR_TO_PC();                                                //for testing
void Get_ADCS_Data();
void SAVE_SENSOR_TO_SMF();
void TRANSFER_SENSOR_FROM_SMF_TO_SCF();
void ADCS_TEST_OPERATION();
void ADCS_TLE_JOIN();
void ADCS_TLE_TRANSFER();
void ADCS_GAIN_JOIN();
void ADCS_GAIN_TRANSFER();
void ADCS_UTC_TRANSFER();
void ADCS_UTC_UPLINK();
void GET_ADCS_FLASH();
void ADCS_FLASH_CHECK();

/////////////MBP Data Collection MISSION/////////////
void Turn_On_MBP();
void Turn_Off_MBP();
void DATACOLLECTION_TEST_OPERATION();
void MISSION_TEST_OPERATION();
void MBP_SYNC();
void MBP_TLE_TRANSFER();

/////////////KILL FUNCTION///////////////////////
void Count_Kill_Flag();

/////////////OTHER FUNCTION//////////////////////
void COLLECT_RESET_DATA();
void CURRENT_ADDRESS_OF_COM_FLASH();
void STORE_ADRESS_DATA_TO_FLASH();
void TAKE_ADDRESS_DATA_FROM_OF();
void MAKE_ADDRESS_DATA();
void CHECK_ADDRESS_DATA();
void MEMORY_ERASE();


////////////////////////////////RESERVATION FUNCTION (reserve_function.c)/////////////////////////////////
void initialize_table();
void sort_table();                                                               //(command info, time info, mission mode)Å~10Å®compare the time and sort
void Save_RSV();
void table_refresh();
void Get_RSV();
void Disp_RSV();                                                                 //show the reserve status (reserve command and reserve table)
void input_reservation_COM();                                                    //put the command into the table and sort
void input_reservation_PC();                                                     //put the command into the table and sort
void Remove_1_Reservation();                                                     //after executing the mission, it should be finished
void Reserve_command_COM();                                                      //reservation from COM
void Reserve_command_PC();                                                       //reservation from PC
void Reserve_next();                                                             //after finishing executing the reservation command, this function should be execute
void RSV_DATA_DOWNLOAD();                                                        //send cmd to COM for data download reservation function
#include<mainpic_function.h>




