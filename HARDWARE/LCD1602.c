
#include "LCD1602.h"

//#define	LCD_I2C_ADDRESS		0x27 << 1	//I2C��ַ����Ҫ����һλ
#define	LCD_I2C_ADDRESS		0x20 << 1	//I2C��ַ����Ҫ����һλ
#define BACKLIGHT			0x08		//PCF8574 P3�ڣ����⿪��
#define En 					0x04		//ʹ��λ��PCF8574 P2��
#define Rw 					0x02		//��/дλ��PCF8574 P1��
#define Rs 					0x01		//ѡ��λ��PCF8574 P0��

#define setbit(x,y)  x |= (1 << y)		//ָ����ĳһλ����1
#define clrbit(x,y)  x &= ~(1 << y)		//ָ����ĳһλ����0
#define reversebit(x,y)  x ^= (1 << y)	//ָ����ĳһλ��ȡ��


//========================================================================
// ����: void LCD_En(unsigned char value)
// ����: LCDʹ��
// ����: ��.
// ����: none.
//========================================================================
void LCD_En(unsigned char value)
{
	Writebyte(LCD_I2C_ADDRESS, value | En);	//EN = 1
	delay_ms(1);
	clrbit(value, 2);
	Writebyte(LCD_I2C_ADDRESS, value);	//EN = 0
	delay_ms(1);
}

//========================================================================
// ����: void LCD_Write(unsigned char mode, unsigned char cmd)
// ����: LCDд����/���ݺ�����mode=0���mode=1����
// ����: ��.
// ����: none.
//========================================================================
void LCD_Write(unsigned char mode, unsigned char cmd)
{
	unsigned char high, low;
	unsigned char temp;
	high = cmd & 0xF0 | BACKLIGHT;			//����λ
	low = (cmd << 4) & 0xF0 | BACKLIGHT;	//����λ�����λ

	temp = Readbyte(LCD_I2C_ADDRESS);		//��ȡ���ŵ�ƽ
	temp = temp & 0xF0;		//��������λ��ƽ

	Writebyte(LCD_I2C_ADDRESS, temp | BACKLIGHT);//en = 0��д����ʱ���ָ���λ��ƽ����
	delay_ms(1);

	if(mode == 0) {
		Writebyte(LCD_I2C_ADDRESS, temp | BACKLIGHT);//rs = 0��д����ʱ���ָ���λ��ƽ����
	} else {
		Writebyte(LCD_I2C_ADDRESS, temp | Rs | BACKLIGHT);//rs = 1��д����ʱ���ָ���λ��ƽ����
	}

	temp = Readbyte(LCD_I2C_ADDRESS);		//��ȡ���ŵ�ƽ
	temp = temp & 0x0F;		//��������λ��ƽ
	
	Writebyte(LCD_I2C_ADDRESS, high | temp);//д����λ����ʱ���ֵ���λ���ŵ�ƽ����
	delay_ms(1);
	LCD_En(high | temp);

	Writebyte(LCD_I2C_ADDRESS, low | temp);//д����λ����ʱ���ֵ���λ���ŵ�ƽ����
	delay_ms(1);
	LCD_En(low | temp);

}

//========================================================================
// ����: void LCD1602_ClearDisplay()
// ����: LCD����
// ����: ��.
// ����: none.
//========================================================================
void LCD1602_ClearDisplay()
{
	LCD_Write(0,0x01);
}


//========================================================================
// ����: void LCD_Init()
// ����: LCD��ʼ��
// ����: ��.
// ����: none.
//========================================================================
void LCD_Init()
{
	unsigned char temp;
	delay_ms(200);			//�ȴ������ȶ�
	LCD_Write(0, 0x30);
	delay_ms(5);
	LCD_Write(0, 0x30);
	delay_ms(5);
	LCD_Write(0, 0x30);
	delay_ms(1);
	LCD_Write(0, 0x20);
	delay_ms(1);

	//������ʱ��1602�ĳ�ʼ��ֻ��Ҫ����λ���ݾͿ�����ɣ��ڳ�ʼ�����֮������ٴ�����λ���ݣ�
	//ִ���ꡰLCD_Write(0, 0x28);��֮��Һ���Ѿ���ʼ������ʵ��ִ����һ���ʱ����Ѿ���ʼ����ɣ�
	//��ʱ�ִ�������λ���ݣ�һ��д���ᴫ��8λ���ݣ�����ʱ�����ֱ��д���ݵĻ����ͻ��γ����룬������Ҫ���ι����Գ�ʼ����
	LCD_Write(0, 0x28);   //4λ���ݶ˿�,2����ʾ,5*7����
	temp = Readbyte(LCD_I2C_ADDRESS);		//��ȡ���ŵ�ƽ
	LCD_En(temp);

	LCD_Write(0, 0x28);   //4λ���ݶ˿�,2����ʾ,5*7����
	LCD_Write(0, 0x0c);	  //������ʾ, �޹��
	LCD_Write(0, 0x06);	  //AC����, ���治��
	LCD_Write(0, 0x01);   //����
	delay_ms(50);
}

//========================================================================
// ����: void LCD1602_GotoXY(unsigned char x, unsigned char y)
// ����: LCD��ʾָ��λ�á�X��0-1��Y��0-15
// ����: x��y.
// ����: none.
//========================================================================
void LCD1602_GotoXY(unsigned char x, unsigned char y)
{
	if(x == 0)
		LCD_Write(0, 0x80 + y);

	if(x == 1)
		LCD_Write(0, (0x80 + 0x40 + y));
}

//========================================================================
// ����: void LCD1602_Display_NoXY(unsigned char *str)
// ����: LCDд���ַ���
// ����: *str.
// ����: none.
//========================================================================
void LCD1602_Display_NoXY(unsigned char *str)
{
	while(*str != '\0') {
		LCD_Write(1, *str);
		str++;
	}
}



