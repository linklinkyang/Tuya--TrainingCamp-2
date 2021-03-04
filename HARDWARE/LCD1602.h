#ifndef	__LCD1602_H
#define	__LCD1602_H

#include "delay.h"
#include "I2C.h"

void LCD_En(unsigned char value);							//LCDʹ��
void LCD_Write(unsigned char mode, unsigned char cmd);		//LCDд����/���ݺ���
void LCD1602_ClearDisplay();								//LCD����
void LCD_Init();											//LCD��ʼ��
void LCD1602_GotoXY(unsigned char x, unsigned char y);		//LCD��ʾָ��λ��
void LCD1602_Display_NoXY(unsigned char *str);				//LCDд���ַ���

#endif
