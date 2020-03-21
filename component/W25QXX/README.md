**2020.3.18**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**SPI FLASH**

*根据开源代码修改*

# 功能介绍
通过SPI操作SPI FLASH(W25QXX)

# 函数接口

## 初始化
> void W25QXX_Init(void);
```
引脚初始化
```

---

## 操作接口
> uint16_t W25QXX_ReadID(void);
```
读取模块的ID，判断模块的型号
```

> void W25QXX_Erase_Chip(void);
```
擦除芯片
```

> void W25QXX_Erase_Sector(uint32_t Dst_Addr);
```
扇区擦除
Dst_Addr : 扇区地址
```

> void W25QXX_PowerDown(void);
```
进入掉电模式
```

> void W25QXX_WAKEUP(void);
```
唤醒
```

---

## 读写数据
> void W25QXX_Read(uint8_t *pBuffer,uint32_t ReadAddr, uint16_t NumByteToRead);
```
从指定位置读取指定长度的数据
pBuffer : 保存读取到的数据的地址
ReadAddr : 开始读取的首地址
NumByteToRead : 读取到的数据的个数
```

> void W25QXX_Write(uint8_t *pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
```
往指定位置写入指定长度的数据
pBuffer : 要写入的数据
WriteAddr : 开始写入的地址
NumByteToWrite : 要写入的长度
```

# 移植说明  
修改相关宏定义
```
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
```

修改初始化函数
```
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
	
	...
} 
```
