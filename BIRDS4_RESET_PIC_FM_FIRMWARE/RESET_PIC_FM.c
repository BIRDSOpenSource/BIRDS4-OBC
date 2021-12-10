#include <16F1789.h>                                                             //device library
#fuses NOWDT, NOBROWNOUT, NOPROTECT                                              //Fuses: No WatchDog Timer, No brownout, No program memory protection
#include <PIC16F1789_registers.h>
#device ADC=8;                                                                   //choose 8 bits A/D conversion 
#use delay(crystal=16MHz,clock=16MHz)                                            //clock=set clock value and clock type

#use rs232(baud=9600,parity=N,xmit=PIN_B6,rcv=PIN_B7,bits=8,stream=PORT1,ERRORS)        //rs232 Configuration, PORT1
#use rs232(baud=9600,parity=N,xmit=PIN_D4,rcv=PIN_D5,bits=8,stream=MPIC,ERRORS)         //rs232 Configuration, MPIC


int8 sec = 0;                                                                    //seconds counter
int8 min = 0;
int8 hou = 0;
unsigned int16 day_num = 0;                                                      //day_num counter
int16 main_count = 0;                                                            //main counter 
//unsigned int16 batvol = 0;                                                       
int8 resetData[11] = {0};                                                        //array for comunication with MAIN PIC
int8 MAIN_ACK;
int8 res_flag = 0;                                                               //24hs reset flag


#include<power_enable.c>                                                         
#include<read_current.c>                                                         
#include<define_fun.c>                                                           

#INT_TIMER0                                                                      //TMR0 interrupt function
void TIMER0_isr() {                                                              //to send a pulse to external WDT

if(!get_timer0()){                                                               
               RB2 = 1;                                                          //5us pulse in RB2
               delay_us(5);
               RB2 = 0; 
           }
     
}

#INT_TIMER1                                                                      //TMR1 interrupt function
void TIMER1_isr() {                                                              //It is used to generate hhmmss
      set_timer1(0x8000);                                                        //Set timer1 (1 second)
      main_count++;                                                              

      if (sec < 59){
      sec++;
      }else{
         sec = 0;
         min++;
      }
      if (min == 60){
         min = 0;
         hou++;
      }
      
      if (hou == 24){                                                            //Every 24hs
         hou = 0;                                                                //Reset hou variable
         day_num++;                                                              //Increse day variable
         res_flag = 1;                                                           //24hs reset flag in high
      }
/*
      fprintf(PORT1,"%02ld", day_num);                                           
      fprintf(PORT1,":%02d", hou);
      fprintf(PORT1,":%02d", min);
      fprintf(PORT1,":%02d\r\n", sec);
      fprintf(PORT1,"maincount:%04ld\r\n", main_count);                          
*/
}

void main()
{
                                                                                 //In/Out pins configuration
   TRISC2 = 0; RC2 = 0;                                                          //Unreg#1 Enable (OCP 4A)                                                        
   TRISC3 = 0; RC3 = 0;                                                          //5V0 Enable (OCP 2A)
   TRISC4 = 0; RC4 = 0;                                                          //3V3#1 ON (converter)
   TRISC5 = 0; RC5 = 0;                                                          //Unreg#2 Enable (OCP 3A)
   
   TRISD0 = 0; RD0 = 0;                                                          //3V3#2 Enable (OCP 2A)
   TRISD1 = 0; RD1 = 0;                                                          //3V3#1 Enable (OCP 2A)
   TRISD2 = 0; RD2 = 0;                                                          //5V0 ON (converter)
   TRISD3 = 0; RD3 = 0;                                                          //3V3#2 ON (converter)
   TRISD6 = 0; RD6 = 0;                                                          //COM PIC Enable
   TRISD7 = 0; RD7 = 0;                                                          //MAIN PIC Enable
   
   TRISB2 = 0; RB2 = 0;                                                          //external WDT
   TRISB5 = 0; RB5 = 1;                                                          //RAW voltage monitor ON
   
   SETUP_ADC(ADC_CLOCK_INTERNAL);                                                //chooses Clock source used for conversion
   SETUP_ADC_PORTS(sAN0|sAN1|sAN2|sAN3|sAN4|sAN5|sAN6);                          //Set analog inputs
   
   clear_interrupt(int_timer1);
   setup_timer_1(T1_EXTERNAL | T1_DIV_BY_1);                                     //Config TMR1, external clock, prescaler=1
   set_timer1(0x8000);                                                           //timer1 for 1 second interruption
   T1OSCEN = 1;                                                                  //Enable TMR1 Oscillator
   enable_interrupts(GLOBAL);                                                    //Global interrupts enabled
   enable_interrupts(INT_TIMER1);                                                //TMR1 interrupt enabled
   enable_interrupts(INT_TIMER0);                                                //TMR0 interrupt enabled
   enable_interrupts(INT_RDA);
   setup_timer_0(RTCC_DIV_256 | RTCC_INTERNAL);                                  //Config TMR0, prescaler=256, internal clock   
   set_timer0(0xFF);                                                             //Carga del TMR0
/*                                                                               //Interrupt every 64us
   printf("*********************************************************\n\r");
   printf("********        BIRDS4 RESET PIC          ***********\n\r");
   printf("*********************************************************\n\r");
*/
   day_read();                                                                   //read day data from EEPROM
   
   if(make16( read_eeprom(day1H),read_eeprom(day1L) ) == -1)                     //if the readed value is -1, EEPROM still empty
   {
      day_num = 0;                                                               //initializes day as zero
      day_num_write();                                                           //saves in EEPROM
   }
   
   
   if (dy[0]==dy[1] && dy[1]==dy[2] && dy[2]==dy[3] && dy[3]==dy[4] && dy[4]==dy[5] && dy[5]==dy[6] && dy[6]==dy[7] && dy[7]==dy[8] && dy[8]==dy[9])
   {
      day_num = dy[0];                                                           //read day_num after confirming all values are the same  
      //fprintf(PORT1,"day number1###### =%ld\n\r",day_num);
   }
   
   else 
   {
      day_num = return_mode();                                                   //in case all elements are not the same, use return_mode function to read the data                                                  
      // fprintf(PORT1,"day number1***** =%ld\n\r",day_num);
   }
 
   delay_ms(500);
   
   first_run_mode();                                                             //Main=ON, COM=OFF, 3V3#1=ON, 3V3#2=ON, Unreg#1and#2=OFF, 5V=ON

   while(TRUE)
   { 
     resetData[0] = 0x8E;
     resetData[1] = sec;
     resetData[2] = min;
     resetData[3] = hou;   
      
     resetData[4] = (day_num >> 8) & 0xFF;
     resetData[5] = day_num & 0x00FF; 

     adcreading_reset();                                                        //read analog inputs
     
   if((min==30) && (hou <=1))                                                    //First 30 minutes
     {                                                                           
      RD6 = 1;                                                                   // Switch enable for COM PIC
      RC2 = 1;                                                                   //Buckboost enable for Unreg #1 (VHF/UHF Tranceiver)
     //printf("******     COM PIC ON     *********\n\r");
     //printf("******     VHF/UHF TRANCEIVER ON     *********\n\r");
     //delayloop(60);                                                           

     }

 //===============================UART WITH MAIN PIC==========================      
         for(int32 num = 0; num < 100; num++)                                                      
         {                                                                       
           if(kbhit(MPIC))                                                       
           {                                                                     
               MAIN_ACK = fgetc(MPIC);                                           //UART, read main pic data
               break;
           }                                                                     
                                                                                 
         } 
         
        
//======================================================================================================= 

         if(MAIN_ACK == 0x28)                                                    //MainPIC command requesting ResetPIC HK data
         {   
           
            //fprintf(PORT1,"******Sending data to main *********\n\r");
            MAIN_ACK=0;
            main_count = 0;
            
            for(int i=0;i<11;i++)                                                
            { 
               fputc(resetData[i],MPIC);  
               //delay_ms(10);              
            }
            //fprintf(PORT1,"******data sent *********\n\r");
          }       
       
         else if(MAIN_ACK == 0xBC)                                               //MainPIC command for turning on Unreg 2
          {            
             //fprintf(PORT1,"******BURNER CIRCUIT ON *********\n\r");
             MAIN_ACK=0;
             main_count = 0;
             RC5 = 1;                                                            //Enable Unreg#2
             fputc(0xCB,MPIC);                                                   //Unreg 2 On signal to Main PIC
             delayloop(45);                                                      //45 seconds
             RC5 = 0;                                                            //Disable Unreg#2
             //fprintf(PORT1,"******BURNER CIRCUIT OFF *********\n\r");
          }      
          
        else if(MAIN_ACK == 0x27)                                                //MainPIC command for system reset
          {            
             MAIN_ACK=0;
             main_count = 0;
             //fprintf(PORT1,"******Received command for satellite reset *********\n\r");
             reset_mode();
          }   
          
        else if(main_count >= 3600)                                              //Resets Main PIC if not receiving data in 1 hour (main_count >= 3600)
           {
            resetMain();
           }
        
        else if(MAIN_ACK == 0xEE)                                                //resets the variable day (day=0) and writes in EEPROM
           {
            //resetDay();
            day_num = 0;                                                         //initializes day as zero
            day_num_write();
           }
 //=================================24hr reset part=========================================================================      
         if(res_flag == 1)                                                       //If reset flag is high
         {
            day_num_write();                                                     //saves day counter in EEPROM
            RD1 = 0;                                                             //3V3#1 turned off first (to prevent FAB interrupt)
           do
            {
               fputc(0xAA,MPIC);                                                 //send cmd to MainPIC to save flags and addresses before satellite reset
               
               if(main_count >= 3600)                                            //Resets Main PIC if not receiving data in 1 hour (main_count >= 3600)
                 {                                                               
                  resetMain();                                                   
                  break;
                 }
      
            }while(MAIN_ACK == 0x27);                                            //MAinPIC confirmation to reset satellite
           
            //tel mpic to wait delay 20ms
            //fprintf(PORT1,"******Received acknoledgement for reset*********\n\r");
            
             MAIN_ACK =0;
             main_count = 0;
             reset_mode();                                                       //System Reset function
             res_flag = 0;                                                       
         }
         
   }

}


