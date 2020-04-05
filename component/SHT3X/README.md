**2020.4.5**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**SHT30温湿度传感器驱动**

# 功能介绍
修改SHT3X的官方例程代码,更方便移植  
通过IIC接口对温湿度模块进行操作

Modify the official routine code of sht3x for more convenient migration

# 移植说明  
1. 添加头文件

2. 修改相关宏定义
```
// System Little Endian
#define LITTLE_ENDIAN		(1)

//=============================================================================
// I2C BUS S/P Condition /* -- use your own i2c function -- */
//-----------------------------------------------------------------------------
#define SHT3X_StartCondition()      SOFTIIC_Start()
#define SHT3X_StopCondition()       SOFTIIC_Stop()

//=============================================================================
// Delay Function /* -- use your own delay function -- */
//-----------------------------------------------------------------------------
#define SHT3X_Delay_Ms(nms)			delay_ms(nms)
#define SHT3X_Delay_Us(nus)			delay_us(nus)

//=============================================================================
// IO-Pins /* -- adapt the defines for your uC -- */
//-----------------------------------------------------------------------------
// Reset pin not use
#define RESET_LOW()     do{}while(0)  	// set Reset to low
#define RESET_HIGH()    do{}while(0)  	// set Reset to high
// Alert on port B, bit 10
#define ALERT_READ      PBin(5)  		// read Alert
```

3. 修改函数接口
```
//-----------------------------------------------------------------------------
// I2C Bus Write Byte And Wait Ack
static etError SHT3X_WriteByte(uint8_t data)
{//发送一个字节并等待应答
    uint8_t ret = SOFTIIC_Send_Byte(data, 1);//SEND A BYTE AND WAIT ACK
    if(ret) return ACK_ERROR;
    return NO_ERROR;
}
//-----------------------------------------------------------------------------
// I2C Bus Read Byte
static etError SHT3X_ReadByte(uint8_t *data, etI2cAck ack, uint8_t timeout)
{//接收一个字节 并根据参数应答或非应答!!!根据自己的IIC接口库自行实现!!!
	*data = SOFTIIC_Recv_Byte(ack);//READ A BYTE
    return NO_ERROR;
}
//-----------------------------------------------------------------------------
static GPIO_InitTypeDef GPIO_InitStructure;
void SHT3X_Init(uint16_t i2cAddress) /* -- adapt the init for your uC -- */
{//初始化相关引脚,IIC总线的初始化可以放在这里,也可以在main函数里面提前初始化
    // init I/O-pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);// I/O port B clock enabled
    // Alert on port B, bit 5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Reset pin not use
	
    ...
}
```

# 关键函数
> void SHT3X_Init(uint16_t i2cAddress);
```
初始化SHT3X模块,设置模块的地址
参数: SHT3X_ADDR_VSS/SHT3X_ADDR_VDD
```

> void SHT3X_SetI2cAdr(uint16_t i2cAddress);
```
设置SHT3X模块的地址
参数: SHT3X_ADDR_VSS/SHT3X_ADDR_VDD
```

> etError SHT3X_GetTempAndHumi(float* temperature, 
							 float* humiditiy,
							 etRepeatability repeatability, 
							 etMode mode,
							 uint8_t timeout);
```
获取温湿度值,单次测量
```

> etError SHT3X_StartPeriodicMeasurment(etRepeatability repeatability, 
									  etFrequency frequency);
```
设置循环测量模式
```

> etError SHT3X_ReadMeasurementBuffer(float* temperature, float* humidity);
```
循环测量模式下,读取温湿度数值
```
