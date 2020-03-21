/*
AUTHOR : YHUAN416
软件SPI

V1.0 2020.3.18
注意,当前版本只有SPI3模式已测试,保证可以使用

*/
#include "soft_spi.h"

/*
clk 	pb3
miso	pb4
mosi	pb5
*/

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
	
	SOFTSPI_MOSI_H();//mosi拉高
	
	//时钟线空闲电平
#if	SOFTSPI_CPOL
	SOFTSPI_CLK_H();
#else
	SOFTSPI_CLK_L();
#endif
}

#if SOFTSPI_CPOL

#if SOFTSPI_CPHA
//SPI3 已测试通过
//时钟线空闲电平 1
//第二边沿采集数据
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;

	for(i = 0; i < 8; i++)
	{
		res<<=1;//先左移
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_L();
		
		//第一个跳变沿输出数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_H();
		
		//第二个跳变沿采集数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(SOFTSPI_MISO_I()) res+=1;
		
		TxData<<=1;//左移
	}
	
	SOFTSPI_CLK_H();//空闲电平
	return res;
}
#else
//SPI2
//时钟线空闲电平 1
//第一边沿采集数据
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;
	
	for(i = 0; i < 8; i++)
	{
		res<<=1;//先左移
		
		//第一个跳变沿之前输出数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_L();
		
		//第一个跳变沿采集数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(SOFTSPI_MISO_I()) res+=1;
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_H();
		
		TxData<<=1;//左移
	}
	
	SOFTSPI_CLK_H();//空闲电平
	return res;
}
#endif

#else //SOFTSPI_CPOL == 0

#if SOFTSPI_CPHA
//SPI1
//时钟线空闲电平 0
//第二边沿采集数据
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;

	for(i = 0; i < 8; i++)
	{
		res<<=1;//先左移
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_H();
		
		//第一个跳变沿输出数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_L();
		
		//第二个跳变沿采集数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(SOFTSPI_MISO_I()) res+=1;
		
		TxData<<=1;//左移
	}
	
	SOFTSPI_CLK_L();//空闲电平
	return res;
}
#else
//SPI0
//时钟线空闲电平 0
//第一边沿采集数据
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;
	
	for(i = 0; i < 8; i++)
	{
		res<<=1;//先左移
		
		//第一个跳变沿之前输出数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_H();
		
		//第一个跳变沿采集数据
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		if(SOFTSPI_MISO_I()) res+=1;
		
		//时钟线跳变
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//延时
		SOFTSPI_CLK_L();
		
		TxData<<=1;//左移
	}
	
	SOFTSPI_CLK_L();//空闲电平
	return res;
}


#endif


#endif
