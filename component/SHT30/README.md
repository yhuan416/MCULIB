**2019.10.27**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**SHT30温湿度传感器简易驱动**

# 功能介绍
通过IIC接口对温湿度模块进行操作,实现每秒读取一次数据

# 函数接口

## 初始化
> uint8_t SHT30_Init(void);  
```
初始化SHT30的工作状态
```

## 读取数据
> uint8_t SHT30_Read(sht30_data_t *result, uint8_t check_data);  
```
读取一次温湿度数据  
result : 温湿度数据结构体指针  
check_data : 是否需要进行校验  
    0 : 不校验  
    1 : 需要校验 

返回值 : 
    0 : 读取成功
    1 : 读取失败或校验不通过

注意 : 当前只支持每秒钟更新一次温湿度数据,高于该频率会导致读取到错误的数据
```

# 移植说明  
修改相关宏定义
```
/* SHT30校验模块 */
#include "crc8.h"
#define SHT30_CRC_INIT				(0xFF)
#define SHT30_CRC_NUM				(0x31)
#define SHT30_CRC_8					crc8

/* 接口操作 */
#define SHT30_START					SOFTIIC_Start
#define SHT30_SEND_BYTE				SOFTIIC_Send_Byte
#define SHT30_RECV_BYTE				SOFTIIC_Recv_Byte
#define SHT30_STOP					SOFTIIC_Stop
```
