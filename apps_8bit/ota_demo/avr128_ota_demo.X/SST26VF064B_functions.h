/* 
 * File:   SST25WF080B_functions.h
 * Author: C03009
 *
 * Created on January 26, 2017, 9:55 AM
 */

#ifndef SST25WF080B_FUNCTIONS_H
#define	SST25WF080B_FUNCTIONS_H


#include "mcc_generated_files/system/pins.h"

#define SST26WF080B_ID          0x51

#define FLASH_SECTOR_SIZE      0x1000
#define FLASH_PAGE_SIZE         0x100

unsigned char Read_Status_Register();

void WRSR(char byte);
void WREN();
void PD();
void EPD();
void WRDI();
void Jedec_ID_Read(int *Manufacturer_Id, int *Device_Type, int *Device_Id);
unsigned char Read(unsigned long Dst); 
void Read_Cont(uint32_t Dst, uint32_t no_bytes, char *read_data);
unsigned char HighSpeed_Read(uint32_t Dst); 
void HighSpeed_Read_Cont(uint32_t Dst,uint32_t no_bytes, char *read_data);
void Page_Program(unsigned long Dst, char *Prog_data);
void Chip_Erase();
void Small_Sector_Erase(unsigned long Dst);
void Sector_Erase(unsigned long Dst);
void Wait_Busy();
void Sector_Program(uint32_t Dst, unsigned char *Prog_data, int length);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SST25WF080B_FUNCTIONS_H */

