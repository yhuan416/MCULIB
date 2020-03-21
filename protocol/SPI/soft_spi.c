/*
AUTHOR : YHUAN416
���SPI

V1.0 2020.3.18
ע��,��ǰ�汾ֻ��SPI3ģʽ�Ѳ���,��֤����ʹ��

*/
#include "soft_spi.h"

/*
clk 	pb3
miso	pb4
mosi	pb5
*/

/* ��ֲ�궨�� */
/*		���е�ƽ		ʱ��
MODE	CPOL		CPHA
SPI0	0			0
SPI1	0			1
SPI2	1			0
SPI3	1			1
*/
//SPI ģʽ
#define SOFTSPI_CPOL		1
#define SOFTSPI_CPHA		1

//��ʱ�궨��
#include "delay.h"
#define SOFTSPI_DELAY_US	delay_us
#define SOFTSPI_DELAY_TICK	2
//���� = SOFTSPI_DELAY_TICK*4

//���Ų����궨��
//CLK
#define SOFTSPI_CLK_H()		PBout(3)=1
#define SOFTSPI_CLK_L()		PBout(3)=0

//MOSI
#define SOFTSPI_MOSI_H()	PBout(5)=1
#define SOFTSPI_MOSI_L()	PBout(5)=0

//MISO
#define SOFTSPI_MISO_I()	PBin(4)

static GPIO_InitTypeDef GPIO_InitStructure;
/* ��ֲ�궨�� */

void SOFTSPI_Init(void)
{
	/* ���ų�ʼ�� */
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
	/* ���ų�ʼ�� */
	
	SOFTSPI_MOSI_H();//mosi����
	
	//ʱ���߿��е�ƽ
#if	SOFTSPI_CPOL
	SOFTSPI_CLK_H();
#else
	SOFTSPI_CLK_L();
#endif
}

#if SOFTSPI_CPOL

#if SOFTSPI_CPHA
//SPI3 �Ѳ���ͨ��
//ʱ���߿��е�ƽ 1
//�ڶ����زɼ�����
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;

	for(i = 0; i < 8; i++)
	{
		res<<=1;//������
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_L();
		
		//��һ���������������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_H();
		
		//�ڶ��������زɼ�����
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(SOFTSPI_MISO_I()) res+=1;
		
		TxData<<=1;//����
	}
	
	SOFTSPI_CLK_H();//���е�ƽ
	return res;
}
#else
//SPI2
//ʱ���߿��е�ƽ 1
//��һ���زɼ�����
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;
	
	for(i = 0; i < 8; i++)
	{
		res<<=1;//������
		
		//��һ��������֮ǰ�������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_L();
		
		//��һ�������زɼ�����
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(SOFTSPI_MISO_I()) res+=1;
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_H();
		
		TxData<<=1;//����
	}
	
	SOFTSPI_CLK_H();//���е�ƽ
	return res;
}
#endif

#else //SOFTSPI_CPOL == 0

#if SOFTSPI_CPHA
//SPI1
//ʱ���߿��е�ƽ 0
//�ڶ����زɼ�����
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;

	for(i = 0; i < 8; i++)
	{
		res<<=1;//������
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_H();
		
		//��һ���������������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_L();
		
		//�ڶ��������زɼ�����
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(SOFTSPI_MISO_I()) res+=1;
		
		TxData<<=1;//����
	}
	
	SOFTSPI_CLK_L();//���е�ƽ
	return res;
}
#else
//SPI0
//ʱ���߿��е�ƽ 0
//��һ���زɼ�����
uint8_t SOFTSPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t i, res;
	
	for(i = 0; i < 8; i++)
	{
		res<<=1;//������
		
		//��һ��������֮ǰ�������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(TxData & 0x80) SOFTSPI_MOSI_H();
		else SOFTSPI_MOSI_L();
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_H();
		
		//��һ�������زɼ�����
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		if(SOFTSPI_MISO_I()) res+=1;
		
		//ʱ��������
		SOFTSPI_DELAY_US(SOFTSPI_DELAY_TICK);//��ʱ
		SOFTSPI_CLK_L();
		
		TxData<<=1;//����
	}
	
	SOFTSPI_CLK_L();//���е�ƽ
	return res;
}


#endif


#endif
