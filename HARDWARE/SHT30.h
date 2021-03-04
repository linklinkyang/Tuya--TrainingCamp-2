#ifndef __SHT30_H__
#define __SHT30_H__

#include "delay.h"
#include "I2C.h"
#include "STC8xxxx.h"
	
extern u8 read_sht_data[7];

//struct data_process
//{
extern	u16 SHT30_Temperature;		//SHT30温度值
extern	u16 SHT30_Humidity;			//SHT30湿度值
//};

extern void SHT30_Init(void);										//SHT30初始化
extern void Read_SHT30(u8 *p,u8 number);							//SHT30读数据
extern void SHT30_Data_Process(void);								//SHT30温湿度数据处理
extern int  CRC8_Compute(u8 *check_data, u8 num_of_data);			//CRC数据校验
extern int  SHT30_CRC8_Check(u8 *p,u8 num_of_data,u8 CrcData);		//CRC数据校验缓冲

#endif
