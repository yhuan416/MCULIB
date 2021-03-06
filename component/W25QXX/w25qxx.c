#include "w25qxx.h"

//4Kbytes为一个Sector/扇区
//16个扇区为1个Block/块
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector 

/* 指令表 */
#define W25X_WriteEnable		(0x06) 
#define W25X_WriteDisable		(0x04) 
#define W25X_ReadStatusReg		(0x05) 
#define W25X_WriteStatusReg		(0x01) 
#define W25X_ReadData			(0x03) 
#define W25X_FastReadData		(0x0B) 
#define W25X_FastReadDual		(0x3B) 
#define W25X_PageProgram		(0x02) 
#define W25X_BlockErase			(0xD8) 
#define W25X_SectorErase		(0x20) 
#define W25X_ChipErase			(0xC7) 
#define W25X_PowerDown			(0xB9) 
#define W25X_ReleasePowerDown	(0xAB) 
#define W25X_DeviceID			(0xAB) 
#define W25X_ManufactDeviceID	(0x90) 
#define W25X_JedecDeviceID		(0x9F)
/* 指令表 */

/* 移植宏定义 */
uint16_t W25QXX_TYPE = W25Q128;//默认是W25Q128

//W25QXX模块的延时函数
#include "delay.h"
#define W25QXX_DELAY_US			delay_us

//W25QXX使用的SPI读写接口
#include "soft_spi.h"
#define W25QXX_ReadWriteByte	SOFTSPI_ReadWriteByte

//W25QXX的片选信号
#define W25QXX_CS_H()			PBout(14)=1
#define W25QXX_CS_L()			PBout(14)=0

//GPIO初始化结构体
static GPIO_InitTypeDef GPIO_InitStructure;
/* 移植宏定义 */

//初始化SPI FLASH的片选IO口
void W25QXX_Init(void)
{
	/* 片选引脚初始化 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//打开时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//PB14
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化引脚
	/* 片选引脚初始化 */
	
	W25QXX_CS_H();//拉高片选
	W25QXX_TYPE = W25QXX_ReadID();//读取FLASH ID
}  

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;                             
	W25QXX_CS_L();							  //使能器件
	W25QXX_ReadWriteByte(W25X_ReadStatusReg); //发送读取状态寄存器命令    
	byte=W25QXX_ReadWriteByte(0XFF);          //读取一个字节  
	W25QXX_CS_H();                            //取消片选     
	return byte;   
} 

//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
static void W25QXX_Write_SR(uint8_t sr)   
{   
	W25QXX_CS_L();                            //使能器件   
	W25QXX_ReadWriteByte(W25X_WriteStatusReg);//发送写取状态寄存器命令    
	W25QXX_ReadWriteByte(sr);               //写入一个字节  
	W25QXX_CS_H();                            //取消片选     	      
}

//W25QXX写使能	
//将WEL置位   
static void W25QXX_Write_Enable(void)   
{
	W25QXX_CS_L();                          	//使能器件   
    W25QXX_ReadWriteByte(W25X_WriteEnable); 	//发送写使能  
	W25QXX_CS_H();                           	//取消片选     	      
}

//W25QXX写禁止	
//将WEL清零  
static void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS_L();                            //使能器件   
    W25QXX_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令    
	W25QXX_CS_H();                            //取消片选     	      
}

//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	W25QXX_CS_L();				    
	W25QXX_ReadWriteByte(W25X_ManufactDeviceID);//发送读取ID命令	    
	W25QXX_ReadWriteByte(0x00); 	    
	W25QXX_ReadWriteByte(0x00); 	    
	W25QXX_ReadWriteByte(0x00); 	 			   
	Temp|=W25QXX_ReadWriteByte(0xFF)<<8;//读取两个字节的ID
	Temp|=W25QXX_ReadWriteByte(0xFF);	 
	W25QXX_CS_H();				    
	return Temp;
}

//等待空闲
static void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);// 等待BUSY位清空
}

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	W25QXX_CS_L();                            		//使能器件   
    W25QXX_ReadWriteByte(W25X_ReadData);         	//发送读取命令   
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr)>>16));//发送24bit地址    
    W25QXX_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (ReadAddr));   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=W25QXX_ReadWriteByte(0XFF);   	//循环读数据
    }
	W25QXX_CS_H();  				    	      
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数	 
static void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
    W25QXX_Write_Enable();                  			//SET WEL 写使能
	W25QXX_CS_L();                            			//使能器件   
    W25QXX_ReadWriteByte(W25X_PageProgram);      		//发送写页命令   
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr)>>16));	//发送24bit地址    
    W25QXX_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (WriteAddr));   
    for(i=0;i<NumByteToWrite;i++)W25QXX_ReadWriteByte(pBuffer[i]);//循环写数据 
	W25QXX_CS_H();                            			//取消片选 
	W25QXX_Wait_Busy();					   				//等待写入结束
}

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
static void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{
	uint16_t pageremain;	   
	pageremain=256-(WriteAddr%256); 	//单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);//直接往页写入数据
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256) pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	}	    
}

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535) 
static uint8_t W25QXX_BUFFER[4096];//操作缓冲区		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;//扇区地址(/4096)
	uint16_t secoff;//扇区内的偏移
	uint16_t secremain;//扇区剩余空间的大小
 	uint16_t i;    
	uint8_t* W25QXX_BUF=W25QXX_BUFFER;//缓冲区首地址
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
	//DEBUG("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			W25QXX_Erase_Sector(secpos);		//擦除这个扇区
			for(i=0;i<secremain;i++)	   		//复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0
		   	pBuffer+=secremain;  				//指针偏移
			WriteAddr+=secremain;				//写地址偏移	   
		   	NumByteToWrite-=secremain;			//字节数递减
			if(NumByteToWrite>4096)secremain=4096;//下一个扇区还是写不完
			else secremain=NumByteToWrite;		//下一个扇区可以写完了
		}
	}
}

//擦除整个芯片
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                 	//SET WEL 写使能
    W25QXX_Wait_Busy();   					//等待忙信号
  	W25QXX_CS_L();                         	//使能器件   
    W25QXX_ReadWriteByte(W25X_ChipErase);  	//发送片擦除命令，擦除结束自动禁止写使能  
	W25QXX_CS_H();                         	//取消片选     	      
	W25QXX_Wait_Busy();   				   	//等待芯片擦除结束
}  

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{
 	Dst_Addr*=4096;
    W25QXX_Write_Enable();                  	//SET WEL 写使能
    W25QXX_Wait_Busy();   						//等待忙
  	W25QXX_CS_L();                            	//使能器件   
    W25QXX_ReadWriteByte(W25X_SectorErase);     //发送扇区擦除指令 
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr)>>16));//发送24bit地址    
    W25QXX_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    W25QXX_ReadWriteByte((uint8_t) (Dst_Addr));  
	W25QXX_CS_H();                            	//取消片选     	      
    W25QXX_Wait_Busy();   				   		//等待擦除完成
}

//进入掉电模式
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS_L();                        	//使能器件   
    W25QXX_ReadWriteByte(W25X_PowerDown); 	//发送掉电命令  
	W25QXX_CS_H();                        	//取消片选     	      
    W25QXX_DELAY_US(3);                   	//等待TPD  
}   

//唤醒
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS_L();                            		//使能器件   
    W25QXX_ReadWriteByte(W25X_ReleasePowerDown);	//发送唤醒指令    
	W25QXX_CS_H();                            		//取消片选     	      
    W25QXX_DELAY_US(3);                            	//等待TRES1
}
