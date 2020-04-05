#include "soft_iic.h"

/* 移植相关配置 */
//延时配置
#include "delay.h"
#define SOFTIIC_DELAY_US	delay_us
#define SOFTIIC_DELAY_MS	delay_ms

//当前文件频率配置 100KHz 每次延时5us 每个周期10us
#define SOFTIIC_DELAY_TICK	5

//时钟线
#define IIC_SCL_H()		PBout(8)=1
#define IIC_SCL_L()		PBout(8)=0

//数据线输出
#define IIC_SDA_O_H()	PBout(9)=1
#define IIC_SDA_O_L()	PBout(9)=0

//数据线输入
#define IIC_SDA_I()		PBin(9)

//数据线配置
#define IIC_SDA_INPUT()		IIC_SDA_PIN_MODE(GPIO_Mode_IN)
#define IIC_SDA_OUTPUT()	IIC_SDA_PIN_MODE(GPIO_Mode_OUT)
/* 移植相关配置 */

//引脚初始化
static GPIO_InitTypeDef GPIO_InitStructure;

//SDA引脚模式设置
static void IIC_SDA_PIN_MODE(GPIOMode_TypeDef mode)
{
	//初始化引脚
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//软件IIC初始化
void SOFTIIC_Init(void)
{
	/* 初始化对应的引脚 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化引脚
	/* 初始化对应的引脚 */
	
	//拉高电平
	IIC_SCL_H();
	IIC_SDA_O_H();
}

//发送开始信号
void SOFTIIC_Start(void)
{
	IIC_SDA_OUTPUT();//输出模式

	//时钟线拉高数据线拉高
	IIC_SCL_H();
	IIC_SDA_O_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

	IIC_SDA_O_L();//数据线拉低	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	IIC_SCL_L();//时钟线拉低
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//发送停止信号
void SOFTIIC_Stop(void)
{
	IIC_SDA_OUTPUT();//输出模式
	
	IIC_SCL_L();
	IIC_SDA_O_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);

	IIC_SCL_H();//时钟线拉高
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	IIC_SDA_O_H();//数据线拉高
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//产生应答
static void SOFTIIC_Ack(void)
{
	IIC_SDA_OUTPUT();//输出模式
	
	//时钟拉低 数据拉低
	IIC_SCL_L();
	IIC_SDA_O_L();//数据拉低表示应答
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//时钟拉高
	IIC_SCL_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//时钟拉低
	IIC_SCL_L();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//产生非应答
static void SOFTIIC_nAck(void)
{
	IIC_SDA_OUTPUT();//输出模式
	
	//时钟拉低 数据拉高
	IIC_SCL_L();
	IIC_SDA_O_H();//数据线拉高表示非应答
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//时钟拉高
	IIC_SCL_H();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	//时钟拉低
	IIC_SCL_L();	
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
}

//等待应答
static uint8_t SOFTIIC_Wait_Ack(void)
{
	uint8_t ack = 0;
	
	IIC_SDA_INPUT();//SDA输入模式

	IIC_SCL_H();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);//等待一个周期
	
	if(IIC_SDA_I()) ack = 1;//如果还是高电平,表示没有收到应答

	IIC_SCL_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	return ack;//返回应答的结果
}

//IIC 写一个字节
iicErr SOFTIIC_Send_Byte(uint8_t data, uint8_t ack)
{
	uint8_t i;

	IIC_SDA_OUTPUT();//输出模式

	IIC_SCL_L();
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	for(i = 0; i < 8; i++)
	{	
		//输出一位/最高位
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

	/* 需要应答信号且对方没有产生应答信号 */
	if(ack && SOFTIIC_Wait_Ack())
	{
		return IIC_NO_ACK;//有错误
	}
	
	return IIC_NO_ERR;
}

//IIC 读一个字节
iicErr SOFTIIC_Recv_Byte(uint8_t *data, uint8_t ack)
{
	uint8_t i, ret;

	IIC_SDA_INPUT();//sda in put mode
	SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	
	for(i = 0; i < 8; i++)
	{
		IIC_SCL_H();//高电平期间读取数据
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
		
		ret<<= 1;
		
		if(IIC_SDA_I())
		{
			ret += 1;
		}
		
		IIC_SCL_L();
		SOFTIIC_DELAY_US(SOFTIIC_DELAY_TICK);
	}

	/* 是否发出应答 */
	if(ack)
	{
		SOFTIIC_Ack();//发出应答
	}
	else
	{
		SOFTIIC_nAck();//非应答
	}

	*data = ret;//保存读取到的字节数据
	
	return IIC_NO_ERR;
}
