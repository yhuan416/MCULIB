**2020.3.18**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**软件IIC**

# 功能介绍
实现IIC接口的通信功能  
包括起始位,结束位,应答位以及数据传输

# 函数接口

## 初始化
> void SOFTIIC_Init(void);
```
软件IIC初始化接口,调用该函数对IIC接口使用到的引脚进行初始化
```
---

## 起始/终止位
> void SOFTIIC_Start(void);  
> void SOFTIIC_Stop(void);  
```
调用这两个接口开始/结束一次传输
```
---

## 发送一个字节
> uint8_t SOFTIIC_Send_Byte(uint8_t data, uint8_t wait_ack);
```
通过模拟IIC发送一个字节  

data : 要发送的字节  

wait_ack : 是否需要等待应答   
    SOFTIIC_TRUE : 等待应答  
    SOFTIIC_FALSE ： 不等待应答

return :   
    0 : 发送字节成功  
    1 : 发送出错/未接收到应答
```
---

## 接收一个字节
> uint8_t SOFTIIC_Recv_Byte(uint8_t ack);  
```
通过模拟IIC接收一个字节  

ack : 是否需要发送应答位  
    SOFTIIC_TRUE : 发送应答  
    SOFTIIC_FALSE ： 不发送应答

return :
    接收到的字节
```

# 移植说明
修改相关宏定义
```
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
```
修改初始化函数
```
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
	
	...
}
```