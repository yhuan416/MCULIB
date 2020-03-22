#include "fifo.h"

//管道控制块
static fifo_t fifo[FIFO_CH_NUM];

//管道缓冲区
static char ch[FIFO_CH_NUM][FIFO_BUF_SIZE];

//初始化管道
void FIFO_Init(void)
{
    uint8_t i;
    for(i = 0; i < FIFO_CH_NUM; i++)
    {
        fifo[i].buf = ch[i];
        fifo[i].font = 0;
        fifo[i].tail = 0;
    }
}

//不判断通道数直接写入
static int FIFO_putc(uint8_t fd, int c)
{
    if(FIFO_Is_Full(fd) == 0) return -1;//通道已满

    fifo[fd].buf[fifo[fd].tail] = c;//添加一个字符
    fifo[fd].tail++;

    if(fifo[fd].tail >= FIFO_BUF_SIZE) fifo[fd].tail = 0;//处理越界

    return c;
}

//不判断通道数直接读出
static int FIFO_getc(uint8_t fd)
{
    char c;
    if(FIFO_Is_Empty(fd) == 0) return -1;//通道已空

    c = fifo[fd].buf[fifo[fd].font];//获取一个字符
    fifo[fd].font++;

    if(fifo[fd].font >= FIFO_BUF_SIZE) fifo[fd].font = 0;//处理越界

    return c;
}

//往管道写入数据
int FIFO_Write(uint8_t fd, char *buf, uint16_t len)
{
    uint16_t i;
    int ret;

    if(fd >= FIFO_CH_NUM) return FIFO_CH_ERR;//判断通道数是否合法

    //循环写入
    for(i = 0 ; i < len ; i++)
    {
        ret = FIFO_putc(fd, buf[i]);
        if(ret == -1) break;//写入失败
    }

    return i;//返回写入成功的个数
}

//从管道内读数据
int FIFO_Read(uint8_t fd, char *buf, uint16_t len)
{
    uint16_t i;
    int ret;

    if(fd >= FIFO_CH_NUM) return FIFO_CH_ERR;

    for(i = 0 ; i < len ; i++)
    {
        ret = FIFO_getc(fd);//获取一个字符
        if(ret == -1)//如果管道为空,一直阻塞直到读取到数据
        {
            i--;
            continue;
        } 
        buf[i] = ret;//保存数据
    }

    return i;//返回读取到的字节
}

//清空某一个通道
int FIFO_Clear(uint8_t fd)
{
    if(fd >= FIFO_CH_NUM) return FIFO_CH_ERR;
    fifo[fd].font = 0;
    fifo[fd].tail = 0;

    return 0;
}

//判断管道是否为空
int FIFO_Is_Empty(uint8_t fd)
{
    if(fd >= FIFO_CH_NUM) return FIFO_CH_ERR;

    if(fifo[fd].font == fifo[fd].tail) return 0;

    return 1;
}

//判断管道是否已满
int FIFO_Is_Full(uint8_t fd)
{
    uint16_t t;

    if(fd >= FIFO_CH_NUM) return FIFO_CH_ERR;

    t = fifo[fd].tail + 1;
    if(t >= FIFO_BUF_SIZE) t = 0;

    if(fifo[fd].font == t) return 0;

    return 1;
}
