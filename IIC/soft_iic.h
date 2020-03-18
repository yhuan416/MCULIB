#ifndef _SOFT_IIC_H_
#define _SOFT_IIC_H_

//����ͷ�ļ�,include "stm32f4xx.h"
#include "sys.h"

//���ڴ�������ʱ,�Ƿ���ҪӦ�����,�Ƿ���Ӧ��
enum{
	SOFTIIC_TRUE = 1,
	SOFTIIC_FALSE = 0
};

//��д����
enum{
	IIC_WR_BIT = 0,
	IIC_RD_BIT = 1
};

//��ʼ��
extern void SOFTIIC_Init(void);

//������ʼ/��ֹ�ź�
extern void SOFTIIC_Start(void);
extern void SOFTIIC_Stop(void);

/* 
дһ���ֽ�
����:
	data : Ҫд���ֽ�
	wait_ack : 0 ���ȴ�Ӧ��λ,1 �ȴ�Ӧ��λ
����ֵ:
	0 : ���ͳɹ�,�����Ҫ�ȴ�Ӧ����ɹ��յ�Ӧ��λ
	1 : δ�յ�Ӧ��λ
*/
extern uint8_t SOFTIIC_Send_Byte(uint8_t data, uint8_t wait_ack);

/*
��ȡһ���ֽ�
���� : 
	ack : 0 ����һ���ֽڳɹ���ظ�һ��Ӧ��, 1 ���ظ�Ӧ��
����ֵ : 
	��ȡ�����ֽ�
*/
extern uint8_t SOFTIIC_Recv_Byte(uint8_t ack);

#endif
