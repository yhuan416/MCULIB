#include "soft_iic.h"

/* ��ֲ������� */
//��ʱ����
#include "delay.h"
#define SOFTIIC_DELAY_US	delay_us
#define SOFTIIC_DELAY_MS	delay_ms

//��ǰ�ļ�Ƶ������ 100KHz ÿ����ʱ5us ÿ������10us
#define SOFTIIC_DELAY_TICK	5

//ʱ����
#define IIC_SCL_H()		PBout(8)=1
#define IIC_SCL_L()		PBout(8)=0

//���������
#define IIC_SDA_O_H()	PBout(9)=1
#define IIC_SDA_O_L()	PBout(9)=0

//����������
#define IIC_SDA_I()		PBin(9)

//����������
#define IIC_SDA_INPUT()		IIC_SDA_PIN_MODE(GPIO_Mode_IN)
#define IIC_SDA_OUTPUT()	IIC_SDA_PIN_MODE(GPIO_Mode_OUT)
/* ��ֲ������� */

//���ų�ʼ��
static GPIO_InitTypeDef GPIO_InitStructure;

//SDA����ģʽ����
static void IIC_SDA_PIN_MODE(GPIOMode_TypeDef mode)
{
	//��ʼ������
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//���IIC��ʼ��
void SOFTIIC_Init(void)
{
	/* ��ʼ����Ӧ������ */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��ʱ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ������
	/* ��ʼ����Ӧ������ */
	
	//���ߵ�ƽ
	IIC_SCL_H();
	IIC_SDA_O_H();
}

//���Ϳ�ʼ�ź�
void SOFTIIC_Start(void)
{
	IIC_SDA_OUTPUT();//���ģʽ

	//ʱ������������������
	IIC_SCL_H();
	IIC_SDA_O_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

	IIC_SDA_O_L();//����������	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	IIC_SCL_L();//ʱ��������
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//����ֹͣ�ź�
void SOFTIIC_Stop(void)
{
	IIC_SDA_OUTPUT();//���ģʽ
	
	IIC_SCL_L();
	IIC_SDA_O_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

	IIC_SCL_H();//ʱ��������
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	IIC_SDA_O_H();//����������
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//����Ӧ��
static void SOFTIIC_Ack(void)
{
	IIC_SDA_OUTPUT();//���ģʽ
	
	//ʱ������ ��������
	IIC_SCL_L();
	IIC_SDA_O_L();//�������ͱ�ʾӦ��
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//ʱ������
	IIC_SCL_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//ʱ������
	IIC_SCL_L();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//������Ӧ��
static void SOFTIIC_nAck(void)
{
	IIC_SDA_OUTPUT();//���ģʽ
	
	//ʱ������ ��������
	IIC_SCL_L();
	IIC_SDA_O_H();//���������߱�ʾ��Ӧ��
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//ʱ������
	IIC_SCL_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//ʱ������
	IIC_SCL_L();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//�ȴ�Ӧ��
static uint8_t SOFTIIC_Wait_Ack(void)
{
	uint8_t ack = 0;
	
	IIC_SDA_INPUT();//SDA����ģʽ

	IIC_SCL_H();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);//�ȴ�һ������
	
	if(IIC_SDA_I()) ack = 1;//������Ǹߵ�ƽ,��ʾû���յ�Ӧ��

	IIC_SCL_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	return ack;//����Ӧ��Ľ��
}

//IIC дһ���ֽ�
iicErr SOFTIIC_Send_Byte(uint8_t data, uint8_t ack)
{
	uint8_t i;

	IIC_SDA_OUTPUT();//���ģʽ

	IIC_SCL_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	for(i = 0; i < 8; i++)
	{	
		//���һλ/���λ
		if(data & 0x80)
		{
			IIC_SDA_O_H();
		}
		else
		{
			IIC_SDA_O_L();
		}
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

		IIC_SCL_H();
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

		IIC_SCL_L();	
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

		data <<= 1;
	}

	/* ��ҪӦ���ź��ҶԷ�û�в���Ӧ���ź� */
	if(ack && SOFTIIC_Wait_Ack())
	{
		return IIC_NO_ACK;//�д���
	}
	
	return IIC_NO_ERR;
}

//IIC ��һ���ֽ�
iicErr SOFTIIC_Recv_Byte(uint8_t *data, uint8_t ack)
{
	uint8_t i, ret;

	IIC_SDA_INPUT();//sda in put mode
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	for(i = 0; i < 8; i++)
	{
		IIC_SCL_H();//�ߵ�ƽ�ڼ��ȡ����
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
		
		ret<<= 1;
		
		if(IIC_SDA_I())
		{
			ret += 1;
		}
		
		IIC_SCL_L();
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	}

	/* �Ƿ񷢳�Ӧ�� */
	if(ack)
	{
		SOFTIIC_Ack();//����Ӧ��
	}
	else
	{
		SOFTIIC_nAck();//��Ӧ��
	}

	*data = ret;//�����ȡ�����ֽ�����
	
	return IIC_NO_ERR;
}
