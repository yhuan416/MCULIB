#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>

//缓冲区大小
#define FIFO_BUF_SIZE   (1024)

//通道数
#define FIFO_CH_NUM     (1)

enum FIFO_ERR {
    FIFO_OK = 0,        //成功
    FIFO_CH_ERR = -1,    //通道数错误
};

typedef struct {
    uint16_t font;//首
    uint16_t tail;//尾

    char *buf;//缓冲区首地址
} fifo_t;

void FIFO_Init(void);

int FIFO_Write(uint8_t fd, char *buf, uint16_t len);
int FIFO_Read(uint8_t fd, char *buf, uint16_t len);

int FIFO_Clear(uint8_t fd);

int FIFO_Is_Empty(uint8_t fd);
int FIFO_Is_Full(uint8_t fd);

#endif
