#include<flash_ memory_ MT25QL01GBBB_OF.c>                                       //all flash memory

#include<MEMORY_OPERATION.c>
#include<reserve_function.c>
#include<SATELLITE_LOG.c>
#include<FLASH_OPERATION.c>                                                      //erase or transfer
#include<mainpic_function.c>
#include<AUTO_MSN.c>
#include<analyze_command.c>

//----------Flash_memory_MT25QL01GBBB_OF.c-------------------------------------

void WRITE_ENABLE_OF();
void WRITE_ENABLE_SCF();
void WRITE_ENABLE_SMF();

void sector_erase_OF(unsigned int32 sector_address);
void sector_erase_SCF(unsigned int32 sector_address);
void sector_erase_SMF(unsigned int32 sector_address);

void SUBSECTOR_4KB_ERASE_OF(unsigned int32 sector_address);
void SUBSECTOR_4KB_ERASE_SCF(unsigned int32 sector_address);
void SUBSECTOR_4KB_ERASE_SMF(unsigned int32 sector_address);

void SUBSECTOR_32KB_ERASE_OF(unsigned int32 sector_address);
void SUBSECTOR_32KB_ERASE_SCF(unsigned int32 sector_address);
void SUBSECTOR_32KB_ERASE_SMF(unsigned int32 sector_address);

void WRITE_DATA_BYTE_OF(unsigned int32 page_address, int8 data);
void WRITE_DATA_BYTE_SCF(unsigned int32 page_address, int8 data);
void WRITE_DATA_BYTE_SMF(unsigned int32 page_address, int8 data);

int8 READ_CHIP_ID_OF();
int8 READ_CHIP_ID_SCF();
int8 READ_CHIP_ID_SMF();

int8 READ_STATUS_REGISTER_OF();
int8 READ_STATUS_REGISTER_SCF();
int8 READ_STATUS_REGISTER_SMF();

int8 READ_DATA_BYTE_OF(unsigned INT32 ADDRESS);
int8 READ_DATA_BYTE_SCF(unsigned INT32 ADDRESS);
int8 READ_DATA_BYTE_SMF(unsigned INT32 ADDRESS);

void TRANSFER_DATA_256byte_OF(unsigned INT32 ADRESS, int PORT_NUM);
void TRANSFER_DATA_256byte_SCF(unsigned INT32 ADRESS, int PORT_NUM);
void TRANSFER_DATA_256byte_SMF(unsigned INT32 ADRESS, int PORT_NUM);

void WRITE_DATA_NBYTE_OF(unsigned INT32 ADDRES,int32 data_byte);
void WRITE_DATA_NBYTE_SCF(unsigned INT32 ADDRES,int32 data_byte);
void WRITE_DATA_NBYTE_SMF(unsigned INT32 ADDRES,int32 data_byte);

void TRANSFER_DATA_NBYTE_TOPC_OF(unsigned INT32 ADRESS,int32 data_byte); //for testing
void TRANSFER_DATA_NBYTE_TOPC_SCF(unsigned INT32 ADRESS,int32 data_byte); //for testing
void TRANSFER_DATA_NBYTE_TOPC_SMF(unsigned INT32 ADRESS,int32 data_byte); //for testing

void TRANSFER_DATA_NBYTE_OFtoSCF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from MAIN FLASH to COM FLASH
void TRANSFER_DATA_NBYTE_SCFtoOF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from COM FLASH to MAIN FLASH
void TRANSFER_DATA_NBYTE_OFtoSMF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from MAIN FLASH to MISSION FLASH
void TRANSFER_DATA_NBYTE_SMFtoOF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from MISSION FLASH to MAIN FLASH
void TRANSFER_DATA_NBYTE_SMFtoSCF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from MISSION FLASH to COM FLASH
void TRANSFER_DATA_NBYTE_SCFtoSMF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //transfer data from COM FLASH to MISSION FLASH

void TRANSFER_DATA_NBYTE_OF(unsigned INT32 FROM_ADRESS,unsigned INT32 TO_ADRESS,int32 data_byte); //for testing

//void sector_erase(unsigned int32 sector_address);  //this is for sector erase


//-----------------------------------------------------------------------------
