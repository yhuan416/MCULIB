**2020.03.22**  
**Author : [YHUAN](https://github.com/yhuan416)**  
**简易管道**

*当前版本仅供参考*

# 功能介绍
可应用于单片机的简易管道
实现管道的读写操作

# 函数接口说明
> void FIFO_Init(void);

管道初始化
> int FIFO_Clear(uint8_t fd);

清空指定管道
> int FIFO_Write(uint8_t fd, char *buf, uint16_t len);
> int FIFO_Read(uint8_t fd, char *buf, uint16_t len);

读写指定管道
> int FIFO_Is_Empty(uint8_t fd);
> int FIFO_Is_Full(uint8_t fd);

判断管道是否为空/是否已满

# 移植说明
配置以下两个宏定义  
//缓冲区大小  
#define FIFO_BUF_SIZE   (1024)  

//通道数  
#define FIFO_CH_NUM     (1)  
