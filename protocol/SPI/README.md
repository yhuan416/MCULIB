**2020.3.18**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**软件模拟SPI**

# 功能介绍
通过引脚操作模拟SPI时序

# 函数接口

## 初始化
> void SOFTSPI_Init(void);   
```
初始化软件SPI对应的引脚
```

## 读取数据
> uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData);
```
发送一个字节的同时读取一个字节
TxData : 要发送的字节

return : 读取到的字节

注意 : 当前只有SPI3模式测试过可以使用,其他模式还需要测试
```

# 移植说明  
修改相关宏定义
```
/* 移植宏定义 */
/*		空闲电平		时序
MODE	CPOL		CPHA
SPI0	0			0
SPI1	0			1
SPI2	1			0
SPI3	1			1
*/
//SPI 模式
#define SOFTSPI_CPOL		1
#define SOFTSPI_CPHA		1

//延时宏定义
#include "delay.h"
#define SOFTSPI_DELAY_US	delay_us
#define SOFTSPI_DELAY_TICK	2
//周期 = SOFTSPI_DELAY_TICK*4

//引脚操作宏定义
//CLK
#define SOFTSPI_CLK_H()		PBout(3)=1
#define SOFTSPI_CLK_L()		PBout(3)=0

//MOSI
#define SOFTSPI_MOSI_H()	PBout(5)=1
#define SOFTSPI_MOSI_L()	PBout(5)=0

//MISO
#define SOFTSPI_MISO_I()	PBin(4)

static GPIO_InitTypeDef GPIO_InitStructure;
/* 移植宏定义 */
```

修改初始化函数
```
void SOFTSPI_Init(void)
{
    /* 引脚初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;//pb3 -> clk, pb5 -> mosi
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//pb4 -> miso
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* 引脚初始化 */

    ...
}
```
