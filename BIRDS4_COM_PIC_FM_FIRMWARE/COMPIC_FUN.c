// This is the function for initial settings for compic

//#define 


Void settings()
{
   TRISD5 = 0;  LATD5 = 1;                                                       //CFM CS pin_output
   TRISC2 = 0;  LATC2 = 1;                                                       //SFM CS pin_output
 
   TRISD1 = 0;  LATD1 = 0;                                                       //TX on Pin_output
   TRISD2 = 0;  LATD2 = 0;                                                       //RX on Pin_output
   TRISD3 = 0;  LATD3 = 0;                                                       //CW ky pin_output
 
   SETUP_ADC(ADC_CLOCK_INTERNAL);                 
   SETUP_ADC_PORTS(sAN4|sAN5);                                                   //channels 4-5 is enable for ADC
 
   enable_interrupts(global);  
   enable_interrupts(INT_RDA);
   
}

//_____________________TRX Controlling_________________________________________
void TX_ON()                                                                     //RD1 = HIGH, RD2 = LOW, RD3 = LOW
{
 LATD1 = 1;                                                                      // TX on pin (1)
 delay_ms(100);
 LATD2 = 0;                                                                      // 0
 delay_ms(100);
 LATD3 = 0;                                                                      // 0
 delay_ms(100);
 return;
}  
   
void CW_ON()                                                                     //RD1 = LOW, RD2 = HIGH, RD3 = LOW
{
 LATD1 = 0;                                                                      // 0
 delay_ms(100);
 LATD2 = 1;                                                                      // CW on pin (1)
 delay_ms(100);
 LATD3 = 0;                                                                      // 0
 delay_ms(100);
 return;    
}
  
void RX_ON()                                                                     //RD1 = LOW, RD2 = LOW, RD3 = LOW       
{
 LATD1 = 0;                                                                      // 0
 delay_ms(100);
 LATD2 = 0;                                                                      // 0
 delay_ms(100);
 LATD3 = 0;                                                                      // 0
 delay_ms(100);
 
 return;
}
//---------------------------------------------------------------------


//_________________Received signal strength indication_________________________
unsigned int16 TRX_RSSI_READ()
{
 SET_ADC_CHANNEL(4); 
 delay_us(10);
 return(READ_ADC());                                                             //AN4 analog reading 
}

//_________________TEMP funtion________________________________________________
unsigned int16 TRX_TEMP_READ()
{
 SET_ADC_CHANNEL(5); 
 delay_us(10);
 return(READ_ADC()); 
}



