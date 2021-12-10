//___character_space_function___///////////////////////////////////
void CH_SPACE()
{
   LATD3 = 0 ;
   delay_ms(180);
   return;
}

//___Word_space_function___////////////////////////////////////////
void WORD_SPACE()
{
   LATD3 = 0 ;
   delay_ms(420);
   return;
}

//___dit_function___///////////////////////////////////////////////
void DIT()
{
   LATD3 = 1;
   delay_ms(60);
   LATD3 = 0;
   delay_ms(60);
   return;
}

//___dit_function___///////////////////////////////////////////////
void DAH()
{
   LATD3 = 1;
   delay_ms(180);
   LATD3 = 0;
   delay_ms(60);
   return;
}

///////////////////////___character-A___//////////////////////////
void CW_A()
{
    DIT();
    DAH();
    CH_SPACE();
}

///////////////////////___character-B___//////////////////////////
void CW_B()
{
   DAH();
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-C___//////////////////////////
void CW_C()
{
   DAH();
   DIT();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-D___//////////////////////////
void CW_D()
{
   DAH();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-E___//////////////////////////
void CW_E()
{
   DIT();
   CH_SPACE();  
}

///////////////////////___character-F___//////////////////////////
void CW_F()
{
   DIT();
   DIT();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-G___//////////////////////////
void CW_G()
{
   DAH();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-H___//////////////////////////
void CW_H()
{
   DIT();
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-I___//////////////////////////
void CW_I()
{
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-J//////////////////////////
void CW_J()
{
   DIT();
   DAH();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-K___//////////////////////////
void CW_K()
{
   DAH();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-L___//////////////////////////
void CW_L()
{
   DIT();
   DAH();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-M___//////////////////////////
void CW_M()
{
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-N___//////////////////////////
void CW_N()
{
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-O___//////////////////////////
void CW_O()
{
   DAH();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-P___//////////////////////////
void CW_P()
{
   DIT();
   DAH();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-Q___//////////////////////////
void CW_Q()
{
   DAH();
   DAH();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-R___//////////////////////////
void CW_R()
{
   DIT();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-S___//////////////////////////
void CW_S()
{
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___character-T___//////////////////////////
void CW_T()
{
   DAH();
   CH_SPACE();  
}

///////////////////////___character-U___//////////////////////////
void CW_U()
{
   DIT();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-V___//////////////////////////
void CW_V()
{
   DIT();
   DIT();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-W___//////////////////////////
void CW_W()
{
   DIT();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-X___//////////////////////////
void CW_X()
{
   DAH();
   DIT();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-Y___//////////////////////////
void CW_Y()
{
   DAH();
   DIT();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___character-Z___//////////////////////////
void CW_Z()
{
   DAH();
   DAH();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-1___/////////////////////////////
void CW_1()
{
   DIT();
   DAH();
   DAH();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___Number-2___/////////////////////////////
void CW_2()
{
   DIT();
   DIT();
   DAH();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___Number-3___/////////////////////////////
void CW_3()
{
   DIT();
   DIT();
   DIT();
   DAH();
   DAH();
   CH_SPACE();  
}

///////////////////////___Number-4___/////////////////////////////
void CW_4()
{
   DIT();
   DIT();
   DIT();
   DIT();
   DAH();
   CH_SPACE();  
}

///////////////////////___Number-5___/////////////////////////////
void CW_5()
{
   DIT();
   DIT();
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-6___/////////////////////////////
void CW_6()
{
   DAH();
   DIT();
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-7___/////////////////////////////
void CW_7()
{
   DAH();
   DAH();
   DIT();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-8___/////////////////////////////
void CW_8()
{
   DAH();
   DAH();
   DAH();
   DIT();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-9___/////////////////////////////
void CW_9()
{
   DAH();
   DAH();
   DAH();
   DAH();
   DIT();
   CH_SPACE();  
}

///////////////////////___Number-9___/////////////////////////////
void CW_0()
{
   DAH();
   DAH();
   DAH();
   DAH();
   DAH();
   CH_SPACE();  
}

void CW_BIRDS4()
{
   CW_B();
   CW_I();
   CW_R();
   CW_D();
   CW_S();
   CW_4();
   return;
}

Void Call_sign()
{
   CW_J();
   CW_G();
   CW_6();
   CW_Y();
   CW_M();
   CW_X();
   return;
}


void CW_LETTER(unsigned int8 CWL)
{
   //Hexa decimal values
   if(CWL == 0x00||CWL ==0x30) CW_0();
   else if(CWL == 0x01||CWL ==0x31) CW_1();
   else if(CWL == 0x02||CWL ==0x32) CW_2();
   else if(CWL == 0x03||CWL ==0x33) CW_3();
   else if(CWL == 0x04||CWL ==0x34) CW_4();
   else if(CWL == 0x05||CWL ==0x35) CW_5();
   else if(CWL == 0x06||CWL ==0x36) CW_6();
   else if(CWL == 0x07||CWL ==0x37) CW_7();
   else if(CWL == 0x08||CWL ==0x38) CW_8();
   else if(CWL == 0x09||CWL ==0x39) CW_9();
   else if(CWL == 0x0A||CWL ==0x61) CW_A();
   else if(CWL == 0x0B||CWL ==0x62) CW_B();
   else if(CWL == 0x0C||CWL ==0x63) CW_C();
   else if(CWL == 0x0D||CWL ==0x64) CW_D();
   else if(CWL == 0x0E||CWL ==0x65) CW_E();
   else if(CWL == 0x0F||CWL ==0x66) CW_F();
   //-------------------------
   
   //normal ascii for CW mission
   else if(CWL == 0x67) CW_G();
   else if(CWL == 0x68) CW_H();
   else if(CWL == 0x69) CW_I();
   else if(CWL == 0x6a) CW_J();
   else if(CWL == 0x6b) CW_K();
   else if(CWL == 0x6c) CW_L();
   else if(CWL == 0x6d) CW_M();
   else if(CWL == 0x6e) CW_N();
   else if(CWL == 0x6f) CW_O();
   else if(CWL == 0x70) CW_P();
   else if(CWL == 0x71) CW_Q();
   else if(CWL == 0x72) CW_R();
   else if(CWL == 0x73) CW_S();
   else if(CWL == 0x74) CW_T();
   else if(CWL == 0x75) CW_U();
   else if(CWL == 0x76) CW_V();
   else if(CWL == 0x77) CW_W();
   else if(CWL == 0x78) CW_X();
   else if(CWL == 0x79) CW_Y();
   else if(CWL == 0x7a) CW_Z();
   
   return;
}

Void CW_MSN()
{
   if(CF_BYTE_READ(0x00000000) == 0xFF)
   {
    CW_LETTER(0x73);    //S
    CW_LETTER(0x69);    //I
    
    CW_LETTER((RSSI_VALUE >>12)&0x000F);
    CW_LETTER((RSSI_VALUE >>8 )&0x000F);
    CW_LETTER((RSSI_VALUE >>4 )&0x000F);
    CW_LETTER((RSSI_VALUE     )&0x000F);
   }
    
   else
   {
    CW_LETTER(CF_BYTE_READ(0x00000001));
    CW_LETTER(CF_BYTE_READ(0x00000002));
    CW_LETTER(CF_BYTE_READ(0x00000003));
    CW_LETTER(CF_BYTE_READ(0x00000004));
    CW_LETTER(CF_BYTE_READ(0x00000005));
    CW_LETTER(CF_BYTE_READ(0x00000006));
   }
   return;  
}

unsigned int8 HK1 =0,HK2=0,HK3=0,HK4=0,HK5=0,HK6=0,HK7=0,HK8=0,HK9=0,HK10=0;


Void CW_PCKT()                                                                   // genera el array con el formato [BIRDS4]__[CALL SIGN]__[CW_MSN]__
{
   delay_ms(500);
   CW_BIRDS4();                                                                  // BIRDS-4 Part of  CW
   delay_ms(500);                                                                
   
   Call_sign();                                                                  // Call_sign Part of  CW
   delay_ms(500);
   
   CW_MSN();                                                                     // RSSI Part of  CW
   delay_ms(500);
   
   if(MAIN_TO_COM[1]==0x50)                                                      // HK part of CW
   {
   HK1  =  (MAIN_TO_COM[2]&0xF0)>>4 ;
   HK2  =  (MAIN_TO_COM[2])&0x0F   ;
   
   HK3  =  (MAIN_TO_COM[3]&0xF0)>>4 ;
   HK4  =  (MAIN_TO_COM[3])&0x0F   ;
   
   HK5  =  (MAIN_TO_COM[4]&0xF0)>>4 ;
   HK6  =  (MAIN_TO_COM[4])&0x0F    ;
   
   HK7  =  (MAIN_TO_COM[5]&0xF0)>>4 ;
   HK8  =  (MAIN_TO_COM[5])&0x0F   ;
   
   HK9  =  (MAIN_TO_COM[6]&0xF0)>>4 ;
   HK10 =  (MAIN_TO_COM[6])&0x0F   ;
   }
   
   CW_LETTER(HK1);
   CW_LETTER(HK2);
   
   CW_LETTER(HK3);
   CW_LETTER(HK4);
   
   CW_LETTER(HK5);
   CW_LETTER(HK6);
   
   CW_LETTER(HK7);
   CW_LETTER(HK8);
   
   CW_LETTER(HK9);
   CW_LETTER(HK10);
   
   return;
}


//_________________________________________________________________________________________________






