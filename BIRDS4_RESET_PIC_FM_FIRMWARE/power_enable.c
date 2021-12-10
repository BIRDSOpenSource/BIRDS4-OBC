void delayloop(int32 seconds)                                                    //Timer function
{                                                                                
   int32 count = 1;                                                              //From 0 to 4.294.967.295 seconds
  
   while(count <= seconds) 
   { 
    delay_ms(1000); 
    count++; 
   } 
 return; 
}

void first_run_mode(void)                                                        //This function runs only at the beginning, after sat deployment
{                                                                                

    //fprintf(PORT1,"******Reset first run enabled*********\n\r");
    RD6 = 0;                                                                     // Switch disabled for COM PIC
    //fprintf(PORT1,"******COM PIC OFF *********\n\r");
    RD7 = 1;                                                                     // Switch enabled for Main PIC
    //fprintf(PORT1,"******Main PIC ON *********\n\r");
    
    RC4 = 1;                                                                     // Converter enabled for 3V3 #1 
    RD1 = 1;                                                                     // OCP switch enabled for 3V3 #1 (OCP 2A)
    
    RD3 = 1;                                                                     // Converter enabled for 3V3 #2
    RD0 = 1;                                                                     // OCP switch enabled for 3V3 #2 (OCP 2A)
    
    RD2 = 1;                                                                     // Converter enabled for 5V
    RC3 = 1;                                                                     // OCP Switch enabled for 5V
    
    RC2 = 0;                                                                     //OCP switch disabled for Unreg #1
    
    RC5 = 0;                                                                     //OCP switch disabled for Unreg #2
   
   
    return;
 
}



void reset_mode(void)                                                            //Satellite Reset Function
{                                                                                //COM, MAIN and 3V3 #1 y #2 power lines are reseted
    //fprintf(PORT1,"******System Reset *********\n\r"); 
    RD6 = 0;                                                                     // Switch disabled for COM PIC
    RD7 = 0;                                                                     // Switch disabled for Main PIC
    
    RD1 = 0;                                                                     // OCP disabled  for 3V3#1
    RD0 = 0;                                                                     // OCP disabled for 3V3#2
    RC2 = 0;                                                                     // OCP disabled for Unreg#1
    
    //fprintf(PORT1,"****** SYSTEM OFF *********\n\r");
    delay_ms(5000);
     
    
    RD6 = 1;                                                                     // Switch enable for COM PIC
    RD7 = 1;                                                                     // Switch enable for Main PIC
    
    RC4 = 1;                                                                     // Converter enable for 3V3 #1
    RD1 = 1;                                                                     // OCP enable for 3V3#1
    RD3 = 1;                                                                     // Converter enable for 3V3 #2
    RD0 = 1;                                                                     // OCP enable for 3V3 #2
    
    RC2 = 1;                                                                     // OCP enable for Unreg #1 (TRANSCEIVER VHF/UHF)
    RC5 = 0;                                                                     // OCP enable for Unreg #2 (BURNER CIRCUIT)
    //fprintf(PORT1,"****** SYSTEM ON *********\n\r");

    
    return; 
}



void resetMain(void)                                                             //MainPIC Reset function
{
    RD7 = 0;                                                                     // Switch disabled for Main PIC
    delay_ms(1000);
    RD7 = 1;                                                                     // Switch enabled for Main PIC
    //fprintf(PORT1,"******Main PIC Reset *********\n\r");
    main_count  = 0;                                                             //main counter reset
   
    return;
}


