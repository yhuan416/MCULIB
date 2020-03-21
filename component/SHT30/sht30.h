#ifndef _SHT30_H_
#define _SHT30_H_

#include "sys.h"

//温湿度数据
typedef struct 
{
	double temp;
	double humi;
} sht30_data_t;

extern uint8_t SHT30_Init(void);
extern uint8_t SHT30_Read(sht30_data_t *result, uint8_t check_data);

#endif

