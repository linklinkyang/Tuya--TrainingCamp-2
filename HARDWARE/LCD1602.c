
#include "LCD1602.h"

//#define	LCD_I2C_ADDRESS		0x27 << 1	//I2C地址，需要左移一位
#define	LCD_I2C_ADDRESS		0x20 << 1	//I2C地址，需要左移一位
#define BACKLIGHT			0x08		//PCF8574 P3口，背光开关
#define En 					0x04		//使能位，PCF8574 P2口
#define Rw 					0x02		//读/写位，PCF8574 P1口
#define Rs 					0x01		//选择位，PCF8574 P0口

#define setbit(x,y)  x |= (1 << y)		//指定的某一位数置1
#define clrbit(x,y)  x &= ~(1 << y)		//指定的某一位数置0
#define reversebit(x,y)  x ^= (1 << y)	//指定的某一位数取反


//========================================================================
// 函数: void LCD_En(unsigned char value)
// 描述: LCD使能
// 参数: 无.
// 返回: none.
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
// 函数: void LCD_Write(unsigned char mode, unsigned char cmd)
// 描述: LCD写命令/数据函数。mode=0命令，mode=1数据
// 参数: 无.
// 返回: none.
//========================================================================
void LCD_Write(unsigned char mode, unsigned char cmd)
{
	unsigned char high, low;
	unsigned char temp;
	high = cmd & 0xF0 | BACKLIGHT;			//高四位
	low = (cmd << 4) & 0xF0 | BACKLIGHT;	//低四位变高四位

	temp = Readbyte(LCD_I2C_ADDRESS);		//获取引脚电平
	temp = temp & 0xF0;		//保留高四位电平

	Writebyte(LCD_I2C_ADDRESS, temp | BACKLIGHT);//en = 0，写引脚时保持高四位电平不变
	delay_ms(1);

	if(mode == 0) {
		Writebyte(LCD_I2C_ADDRESS, temp | BACKLIGHT);//rs = 0，写引脚时保持高四位电平不变
	} else {
		Writebyte(LCD_I2C_ADDRESS, temp | Rs | BACKLIGHT);//rs = 1，写引脚时保持高四位电平不变
	}

	temp = Readbyte(LCD_I2C_ADDRESS);		//获取引脚电平
	temp = temp & 0x0F;		//保留低四位电平
	
	Writebyte(LCD_I2C_ADDRESS, high | temp);//写高四位数据时保持低四位引脚电平不变
	delay_ms(1);
	LCD_En(high | temp);

	Writebyte(LCD_I2C_ADDRESS, low | temp);//写高四位数据时保持低四位引脚电平不变
	delay_ms(1);
	LCD_En(low | temp);

}

//========================================================================
// 函数: void LCD1602_ClearDisplay()
// 描述: LCD清屏
// 参数: 无.
// 返回: none.
//========================================================================
void LCD1602_ClearDisplay()
{
	LCD_Write(0,0x01);
}


//========================================================================
// 函数: void LCD_Init()
// 描述: LCD初始化
// 参数: 无.
// 返回: none.
//========================================================================
void LCD_Init()
{
	unsigned char temp;
	delay_ms(200);			//等待供电稳定
	LCD_Write(0, 0x30);
	delay_ms(5);
	LCD_Write(0, 0x30);
	delay_ms(5);
	LCD_Write(0, 0x30);
	delay_ms(1);
	LCD_Write(0, 0x20);
	delay_ms(1);

	//用四线时，1602的初始化只需要高四位数据就可以完成，在初始化完成之后必须再传入四位数据，
	//执行完“LCD_Write(0, 0x28);”之后液晶已经初始化，其实在执行了一半的时候就已经初始化完成，
	//此时又传入了四位数据（一个写语句会传入8位数据），这时候如果直接写数据的话，就会形成乱码，所以需要两次功能性初始化。
	LCD_Write(0, 0x28);   //4位数据端口,2行显示,5*7点阵
	temp = Readbyte(LCD_I2C_ADDRESS);		//获取引脚电平
	LCD_En(temp);

	LCD_Write(0, 0x28);   //4位数据端口,2行显示,5*7点阵
	LCD_Write(0, 0x0c);	  //开启显示, 无光标
	LCD_Write(0, 0x06);	  //AC递增, 画面不动
	LCD_Write(0, 0x01);   //清屏
	delay_ms(50);
}

//========================================================================
// 函数: void LCD1602_GotoXY(unsigned char x, unsigned char y)
// 描述: LCD显示指针位置。X：0-1，Y：0-15
// 参数: x、y.
// 返回: none.
//========================================================================
void LCD1602_GotoXY(unsigned char x, unsigned char y)
{
	if(x == 0)
		LCD_Write(0, 0x80 + y);

	if(x == 1)
		LCD_Write(0, (0x80 + 0x40 + y));
}

//========================================================================
// 函数: void LCD1602_Display_NoXY(unsigned char *str)
// 描述: LCD写入字符串
// 参数: *str.
// 返回: none.
//========================================================================
void LCD1602_Display_NoXY(unsigned char *str)
{
	while(*str != '\0') {
		LCD_Write(1, *str);
		str++;
	}
}



