#ifndef _W25QXX_H_
#define _W25QXX_H_			    

#include "sys.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 		(0XEF13) 	
#define W25Q16 		(0XEF14)
#define W25Q32 		(0XEF15)
#define W25Q64 		(0XEF16)
#define W25Q128		(0XEF17)

////////////////////////////////////////////////////////////////////////////
extern uint16_t W25QXX_TYPE;	//定义W25QXX芯片型号		   

extern void W25QXX_Init(void);			//初始化片选引脚

extern uint16_t W25QXX_ReadID(void);  	//读取FLASH ID

extern void W25QXX_Read (uint8_t *pBuffer,uint32_t ReadAddr, uint16_t NumByteToRead );//读取flash
extern void W25QXX_Write(uint8_t *pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash

extern void W25QXX_Erase_Chip(void);    	  		//整片擦除
extern void W25QXX_Erase_Sector(uint32_t Dst_Addr);	//扇区擦除

extern void W25QXX_PowerDown(void);	//进入掉电模式
extern void W25QXX_WAKEUP(void);	//唤醒

#endif
