#ifndef	__LCD1602_H
#define	__LCD1602_H

#include "delay.h"
#include "I2C.h"

void LCD_En(unsigned char value);							//LCD使能
void LCD_Write(unsigned char mode, unsigned char cmd);		//LCD写命令/数据函数
void LCD1602_ClearDisplay();								//LCD清屏
void LCD_Init();											//LCD初始化
void LCD1602_GotoXY(unsigned char x, unsigned char y);		//LCD显示指针位置
void LCD1602_Display_NoXY(unsigned char *str);				//LCD写入字符串

#endif
