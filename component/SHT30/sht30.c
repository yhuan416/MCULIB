/*
sht30 driver (IIC)

Author:yHuan
Date:2019.10.27
Version:V1.0

using 	i2c driver
		crc8 check data

*/
#include "sht30.h"

/* SHT30模块的地址 */ 
#define SHT30_ADDRESS				(0x44<<1)

/* 设置周期为1秒 */
#define SHT30_PERIODIC_SET_MSB  	(0x21)
#define SHT30_PERIODIC_SET_LSB  	(0x30)

/* 开始转换 */
#define SHT30_READ_MSB  			(0xe0)
#define SHT30_READ_LSB  			(0x00)

/* SHT30校验模块 */
#define SHT30_CRC_INIT				(0xFF)
#define SHT30_CRC_NUM				(0x31)
#define SHT30_CRC_8					crc8

/* 接口操作 */
#include "soft_iic.h"
#define SHT30_START					SOFTIIC_Start
#define SHT30_SEND_BYTE				SOFTIIC_Send_Byte
#define SHT30_RECV_BYTE				SOFTIIC_Recv_Byte
#define SHT30_STOP					SOFTIIC_Stop
#define SHT30_WR_BIT				IIC_WR_BIT
#define SHT30_RD_BIT				IIC_RD_BIT

//如果使用CRC8校验
#if SHT30_CRC8_CHECK
//CRC8校验函数
static uint8_t crc8(uint8_t crc_num, uint8_t crc_init_num, uint8_t *data, uint8_t num)
{
	uint8_t crc = crc_init_num;
	
	uint8_t i, j, temp;

	for (i = 0; i < num; ++i)
	{
		temp = data[i];
		
		crc ^= temp;
		
		for (j = 0; j < 8; ++j)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ crc_num;
			}
			else
			{
				crc = (crc << 1);
			}
		}
	}

	return crc;
}

#endif

//CRC8校验
static uint8_t sht30_crc8(uint8_t *data, uint8_t data_crc)
{	
	#if SHT30_CRC8_CHECK
								 //用于校验的式子	校验的初始值   数据头 长度
	return !(data_crc == SHT30_CRC_8(SHT30_CRC_NUM, SHT30_CRC_INIT, data, 2));

	#else

	return 0;

	#endif
}

//配置传感器模式
uint8_t SHT30_Init(void)
{
	uint8_t ret;
	
	SHT30_START();
	ret = SHT30_SEND_BYTE(SHT30_ADDRESS + SHT30_WR_BIT, 1);
	if(ret)
	{
		goto exti;
	}		
	
	ret = SHT30_SEND_BYTE(SHT30_PERIODIC_SET_MSB, 1);
	if(ret)
	{
		goto exti;
	}	
	ret = SHT30_SEND_BYTE(SHT30_PERIODIC_SET_LSB, 1);
	if(ret)
	{
		goto exti;
	}	
	
	SOFTIIC_Stop();
    return 0;
	
exti:
	SOFTIIC_Stop();
    return 1;
}

//读取传感器数据
uint8_t SHT30_Read(sht30_data_t *result, uint8_t check_data)
{
    uint8_t data[6] = {0};
    
    SHT30_START();
	SHT30_SEND_BYTE(SHT30_ADDRESS + SHT30_WR_BIT, 1);//先找设备
	SHT30_SEND_BYTE(SHT30_READ_MSB, 1);//写地址
	SHT30_SEND_BYTE(SHT30_READ_LSB, 1);//写地址
	
	SHT30_START();
	SHT30_SEND_BYTE(SHT30_ADDRESS + SHT30_RD_BIT, 1);//读模式
	data[0] = SHT30_RECV_BYTE(1);//读数据
	data[1] = SHT30_RECV_BYTE(1);
	data[2] = SHT30_RECV_BYTE(1);
	data[3] = SHT30_RECV_BYTE(1);
	data[4] = SHT30_RECV_BYTE(1);
	data[5] = SHT30_RECV_BYTE(0);
    SHT30_STOP();
    
	//判断是否需要校验
    if (check_data && (sht30_crc8(&data[0], data[2]) || sht30_crc8(&data[3], data[5])))
    {/* 需要校验且校验不通过 */
    	return 1;
    }
    
	//计算数据
    result->temp = (175.0 * ((double)((data[0] << 8) | data[1])) / 65535.0) - 45.0;
    result->humi = (100.0 * ((double)((data[3] << 8) | data[4])) / 65535.0);
    
    return 0;
}


