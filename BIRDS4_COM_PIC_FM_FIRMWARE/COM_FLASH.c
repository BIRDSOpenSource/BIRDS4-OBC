//commands
#define READ_ID           0x9F
#define READ_STATUS_REG   0x05 
//#define READ_DATA_BYTES   0x13  //0x03 for 3byte mode
//#define ENABLE_WRITE      0x06
//#define WRITE_BYTE        0x12  //0x02 for 3byte mode
//#define ERASE_SECTOR      0xDC  //0xD8 for 3byte mode

//______________________Write Enable Codes____________________________________________________\\
void CF_WRITE_ENABLE(){
   LATD5 = 0;
   delay_us(50);
   spi_xfer(CFM,0x06);                                                           //Send 0x06
   LATD5 = 1;
   return;
}

void SF_WRITE_ENABLE(){
   LATC2 = 0;
   delay_us(50);
   spi_xfer(SFM,0x06);                                                           //Send 0x06
   LATC2 = 1;
   return;
}
//----------------------------------------------------------------------------------------------\\
//----------------------------------------------------------------------------------------------\\


//_____________________Sector Erase Functions___________________________________________________\\
void CF_SECTOR_ERASE(unsigned int32 sector_address)
{
   unsigned int8 adsress[4];
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);                   // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);                   // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8) & 0xFF);                    // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((sector_address) & 0xFF);                       // 0x 00 00 00 _ _
   
   CF_WRITE_ENABLE();                                                            //writting enabled                                                         
   
   LATD5 = 0;                                                                    //lower the CS PIN
   delay_us(2);
   ///////////////////////////////////////////////////////////////////
   spi_xfer(CFM,0xDC);                                                           //SECTOR ERASE COMAND   (0xDC)
   spi_xfer(CFM,adsress[0]);   
   spi_xfer(CFM,adsress[1]);    
   spi_xfer(CFM,adsress[2]);    
   spi_xfer(CFM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   delay_us(2);
   LATD5 = 1;                                                                    //take CS PIN higher back
   delay_ms(3000);  
   return;
}

void SF_SECTOR_ERASE(unsigned int32 sector_address)
{
   unsigned int8 adsress[4];
   
   adsress[0]  = (unsigned int8)((sector_address>>24) & 0xFF);   // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((sector_address>>16) & 0xFF);   // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((sector_address>>8) & 0xFF);    // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((sector_address) & 0xFF);       // 0x 00 00 00 _ _
   
   SF_WRITE_ENABLE();
   
   LATC2 = 0;             //lower the CS PIN
   delay_us(2);
   ///////////////////////////////////////////////////////////////////
   spi_xfer(SFM,0xDC); //SECTOR ERASE COMAND   (0xDC)
   spi_xfer(SFM,adsress[0]);   
   spi_xfer(SFM,adsress[1]);    
   spi_xfer(SFM,adsress[2]);    
   spi_xfer(SFM,adsress[3]);
   //////////////////////////////////////////////////////////////////
   delay_us(2);
   LATC2 = 1;           //take CS PIN higher back
   delay_ms(3000);  
   return;
}
//-----------------------------------------------------------------------------------------------\\
//-----------------------------------------------------------------------------------------------\\


//________________________BYTE WRITE Function____________________________________________________\\
void CF_BYTE_WRITE(unsigned int32 byte_address, int8 data)
{
   unsigned int8 adsress[4];
   
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);                     // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);                     // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8) & 0xFF);                      // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((byte_address) & 0xFF);                         // 0x 00 00 00 _ _
   
   CF_WRITE_ENABLE();
   
   LATD5 = 0;                                                                    //lower the CS PIN
   delay_us(2);
   
   ////////////////////////////////////////////////////////////////
   spi_xfer(CFM,0x12);                                                           //Byte WRITE COMAND  (0x12)
   spi_xfer(CFM,adsress[0]);    
   spi_xfer(CFM,adsress[1]);    
   spi_xfer(CFM,adsress[2]);    
   spi_xfer(CFM,adsress[3]);
   
   spi_xfer(CFM,data); 
   ////////////////////////////////////////////////////////////////
   
   LATD5 = 1;                                                                    //take CS PIN higher back

   return;
}

void SF_BYTE_WRITE(unsigned int32 byte_address, int8 data)
{
   unsigned int8 adsress[4];
   
   //Byte extraction
   adsress[0]  = (unsigned int8)((byte_address>>24) & 0xFF);   // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((byte_address>>16) & 0xFF);   // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((byte_address>>8) & 0xFF);    // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((byte_address) & 0xFF);       // 0x 00 00 00 _ _
   
   SF_WRITE_ENABLE();
   
   LATC2 = 0;           //lower the CS PIN
   delay_us(2);
   
   ////////////////////////////////////////////////////////////////
   spi_xfer(SFM,0x12); //PAGE WRITE COMAND  (0x12)
   spi_xfer(SFM,adsress[0]);    
   spi_xfer(SFM,adsress[1]);    
   spi_xfer(SFM,adsress[2]);    
   spi_xfer(SFM,adsress[3]);
   
   spi_xfer(SFM,data); 
   ////////////////////////////////////////////////////////////////
   
   LATC2 = 1;        //take CS PIN higher back

   return;
}
//-----------------------------------------------------------------------------------------------\\
//-----------------------------------------------------------------------------------------------\\


//________________________BYTE read Function____________________________________________________\\
unsigned int8 CF_BYTE_READ(unsigned INT32 ADDRESS)
{

   unsigned int8 adsress[4];
   //Byte extraction
   adsress[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);                          // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);                          // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((ADDRESS>>8) & 0xFF);                           // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((ADDRESS) & 0xFF);                              // 0x 00 00 00 _ _
   
   LATD5 = 0;                                                                    //lower the CS PIN
   delay_us(2);
    //////////////////////////////////////////////////////////////////
   int8 data;
   spi_xfer(CFM,0X13);                                                           //READ DATA COMAND   (0x13)
   spi_xfer(CFM,adsress[0]);
   spi_xfer(CFM,adsress[1]);
   spi_xfer(CFM,adsress[2]);
   spi_xfer(CFM,adsress[3]);
 
   data = spi_xfer(CFM);
   //////////////////////////////////////////////////////////////////
 
   LATD5 = 1;                                                                    //take CS PIN higher back
   return data;
}

unsigned int8 SF_BYTE_READ(unsigned INT32 ADDRESS)
{

   unsigned int8 adsress[4];
   //Byte extraction
   adsress[0]  = (unsigned int8)((ADDRESS>>24) & 0xFF);                          // 0x _ _ 00 00 00
   adsress[1]  = (unsigned int8)((ADDRESS>>16) & 0xFF);                          // 0x 00 _ _ 00 00
   adsress[2]  = (unsigned int8)((ADDRESS>>8) & 0xFF);                           // 0x 00 00 _ _ 00
   adsress[3]  = (unsigned int8)((ADDRESS) & 0xFF);                              // 0x 00 00 00 _ _
   
   LATC2 = 0;                                                                    //lower the CS PIN
   delay_us(2);
   //////////////////////////////////////////////////////////////////
   int8 data;
   spi_xfer(SFM,0X13);                                                           //READ DATA COMAND   (0x13)
   spi_xfer(SFM,adsress[0]);
   spi_xfer(SFM,adsress[1]);
   spi_xfer(SFM,adsress[2]);
   spi_xfer(SFM,adsress[3]);
 
   data = spi_xfer(SFM);
   //////////////////////////////////////////////////////////////////
 
   LATC2 = 1;                                                                    //take CS PIN higher back
   return data;
}
//-----------------------------------------------------------------------------------------------\\
//-----------------------------------------------------------------------------------------------\\

Void FLASH_COPY(unsigned int32 start_address,unsigned int32 data_length)
{
 
 CF_SECTOR_ERASE(start_address);                                                 //erases i sector of COM Flash indicated by start_address
 
 
   for(int32 inc = 0 ;inc < data_length ;inc++)
   {
    CF_BYTE_WRITE(start_address+inc , SF_BYTE_READ(start_address+inc));          //reads Shared flash and writes COM flash 
   } 
 return;  
}









































/*
int8 READ_CHIP_ID()
{
   RC7 = 0;           //lower the CS PIN
 
   ////////////////////////////////////////////////////////////////
   int8 chip_id;
   spi_xfer(SPIPORT,READ_ID);    //READ ID COMAND   (0x9F)
   chip_id = spi_xfer(SPIPORT);
   ////////////////////////////////////////////////////////////////
 
   RC7 = 1;         //take CS PIN higher back
   return chip_id;
}


int8 READ_STATUS_REGISTER()
{
 RC7 = 0;           //lower the CS PIN
 
 /////////////////////////////////////////////////////////////////
 int8 status_reg;
 spi_xfer(SPIPORT,READ_STATUS_REG); //READ STATUS REGISTER COMAND  (0x05)
 status_reg = spi_xfer(SPIPORT);
 /////////////////////////////////////////////////////////////////
 
 RC7 = 1;         //take CS PIN higher back
 return status_reg;
}



*/

