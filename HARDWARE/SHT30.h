#ifndef __SHT30_H__
#define __SHT30_H__

#include "delay.h"
#include "I2C.h"
#include "STC8xxxx.h"
	
extern u8 read_sht_data[7];

//struct data_process
//{
extern	u16 SHT30_Temperature;		//SHT30�¶�ֵ
extern	u16 SHT30_Humidity;			//SHT30ʪ��ֵ
//};

extern void SHT30_Init(void);										//SHT30��ʼ��
extern void Read_SHT30(u8 *p,u8 number);							//SHT30������
extern void SHT30_Data_Process(void);								//SHT30��ʪ�����ݴ���
extern int  CRC8_Compute(u8 *check_data, u8 num_of_data);			//CRC����У��
extern int  SHT30_CRC8_Check(u8 *p,u8 num_of_data,u8 CrcData);		//CRC����У�黺��

#endif
