int8 READ_PIC_ADC(int8 port_name)                                                //A/D converter function
{
   SET_ADC_CHANNEL(port_name);                                                   //Select the analog channel
   delay_us(20);                                                                 //acquisition time delay, tipical=20us
   int8 ADC_VALUE = READ_ADC();                                                  //analog reading
   return ADC_VALUE;                                                             //Return the analog reading
}

void adcreading_reset(void)                                                      //ResetPIC HK function
{                                                                                
   //fprintf(PORT1,"******ADC reading start *********\n\r");
   
   resetData[6] = READ_PIC_ADC(0);                                               //V out raw
              
   resetData[7] = READ_PIC_ADC(1);                                               //I out 3V3 #1 
      
   resetData[8] = READ_PIC_ADC(2);                                               //I out 3V3 #2
        
   resetData[9] = READ_PIC_ADC(4);                                               //I out unreg #1 
        
 //resetData[10] = READ_PIC_ADC(5);                                              //I out unreg #2   
   
   resetData[10] = READ_PIC_ADC(3);                                              //I out 5V
   return;
        
}


