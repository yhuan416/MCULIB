#ifndef _SOFT_IIC_H_
#define _SOFT_IIC_H_

//顶层头文件,include "stm32f4xx.h"
//#include "sys.h"

//读写操作
enum{
	IIC_WR_BIT = 0,
	IIC_RD_BIT = 1
};

//IIC返回值
typedef enum{
	IIC_NO_ERR = 0,			/* 没有错误 */
	IIC_NO_ACK,				/* 无应答错误 */
} iicErr;

//初始化
extern void SOFTIIC_Init(void);

//发送起始/终止信号
extern void SOFTIIC_Start(void);
extern void SOFTIIC_Stop(void);

/* 
写一个字节
参数:
	data : 要写的字节
	wait_ack : 0 不等待应答位,1 等待应答位
返回值:
	0 : 发送成功,如果需要等待应答则成功收到应答位
	1 : 未收到应答位
*/
extern iicErr SOFTIIC_Send_Byte(uint8_t data, uint8_t ack);

/*
读取一个字节
参数 : 
	ack : 0 接收一个字节成功后回复一个应答, 1 不回复应答
返回值 : 
	读取到的字节
*/
extern iicErr SOFTIIC_Recv_Byte(uint8_t *data, uint8_t ack);

#endif
