/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include "config.h"
#include "GPIO.h"
#include "delay.h"
#include "I2C.h"
#include "LCD1602.h"
#include "SHT30.h"
#include "UART.h"

#include "protocol.h"
#include "mcu_api.h"
#include "system.h"
#include "wifi.h"


/***************	功能说明	****************

Yang：用于涂鸦物联网的温湿度控制器。

本例程基于STC8H8K64U为主控芯片的实验箱8进行编写测试，STC8G、STC8H系列芯片可通用参考.

程序使用P6口来演示跑马灯，输出低驱动。

下载时, 选择时钟 24MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************	本地常量声明	**************/
#define Beep 	P12		//蜂鸣器
#define LED_B	P16		//WiFi指示灯，WHITE
#define LED_R	P17		//电源指示灯，RED
#define S1		P13		//继电器开关S1
#define S2		P54		//继电器开关S2
#define KEY_1	P34		//按键KEY_1
#define KEY_2	P35		//按键KEY_2
#define KEY_3	P36		//按键KEY_3
#define KEY_4	P37		//按键KEY_4

u8 table1[] 	= "TuYa & LCEDA    ";  //16
u8 table2[] 	= "Humiture Control";  //16
u8 Table_K1[] 	= "K1:";
u8 Table_K2[] 	= "K2:";
u8 Table_ON[] 	= "-";
u8 Table_OFF[] 	= "x";

/*************	本地变量声明	**************/
//u8 ledIndex;
u16 Set_Temp_Open = 250;	//设定温度值
u16 Set_Temp_Close = 250;	//设定温度值
u16 Set_Hum = 250;			//设定湿度值
u8 Key_Model=0;				//按键模式

bit Set_Handle_Flag=0;
bit Wifi_Reset_Flag=0;		//复位wifi成功标志

u8 Temperatur[6];		//温度数据转换字符串缓存
u8 Humidity[6];			//湿度数据转换字符串缓存
u8 Set_Value[5];		//int数据转换字符串缓存

/*************	本地函数声明	**************/
void Handle_Char_Temp(u16 dat);			//温度数据转换字符串函数
void Handle_Char_Hum(u16 dat);			//湿度数据转换字符串函数
void Handle_Char_SetTemp(u16 dat);		//int数据转换字符串函数
void LCD_Display();						//LCD1602显示函数
void Key_Handle();						//按键处理函数
void Set_Handle();						//温度设置处理函数

/*************  外部函数和变量声明 *****************/


/******************** IO配置函数 **************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//结构定义
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//初始化

	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//初始化
	Beep = 0;LED_B = 0;LED_R = 0;S1 = 0;
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;	//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//初始化
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;		//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);	//初始化
	S2 = 0;
}

/****************  I2C初始化函数 *****************/
void	I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C功能使能,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;			//主机使能自动发送,  ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_Interrupt = DISABLE;		//使能主机模式中断,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 16;				//总线速度=Fosc/2/(Speed*2+4),      0~63
	I2C_InitStructure.I2C_IoUse     = I2C_P14_P15;		//IO口切换   I2C_P14_P15, I2C_P24_P25, I2C_P33_P32

	I2C_Init(&I2C_InitStructure);
}

/***************  串口初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = Polity_0;			//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART_Configuration(UART1, &COMx_InitStructure);			//初始化串口1 UART1,UART2,UART3,UART4

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = Polity_0;			//指定中断优先级(低到高) Polity_0,Polity_1,Polity_2,Polity_3
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);			//初始化串口2 UART1,UART2,UART3,UART4
	
	//PrintString1("STC8H8K64U UART1 Test Programme!\r\n");	//UART1发送一个字符串
	//PrintString2("STC8H8K64U UART2 Test Programme!\r\n");	//UART2发送一个字符串
}


/******************** 主函数 **************************/
void main(void)
{
	delay_ms(100);			//上电等待系统稳定
	delay_us(50);
	GPIO_config();			//IO初始化
	I2C_config();			//IIC初始化
	UART_config();			//串口初始化
	LCD_Init();				//LCD1602初始化
	SHT30_Init(); 			//SHT30初始化
	wifi_protocol_init();	//协议串口初始化函数 mcu_api.c
	EA = 1;					//开总中断
		
	//Beep = 1;delay_ms(250);Beep = 0;	//上电蜂鸣器提示
	LED_R = 1;							//上电打开电源指示灯
	
	LCD1602_GotoXY(0, 0);			//显示字符串
	LCD1602_Display_NoXY(table1);
	LCD1602_GotoXY(1, 0);			//显示字符串
	LCD1602_Display_NoXY(table2);
	delay_s(2);						//显示2秒
	LCD1602_ClearDisplay();			//LCD清屏

	while(1)
	{	
		wifi_uart_service();		//wifi串口数据处理服务 mcu_api.c
		
		switch(mcu_get_wifi_work_state())
		{
			case SMART_CONFIG_STATE:
                LED_B =! LED_B;delay_ms(1);
				//处于 Smart 配置状态，即 LED 快闪
            break;
            case AP_STATE:
				LED_B =! LED_B;delay_ms(50);
                //处于 AP 配置状态，即 LED 慢闪
            break;
            case WIFI_NOT_CONNECTED:
                LED_B = 0;
				//Wi-Fi 配置完成，正在连接路由器，即 LED 常暗
            break;
            case WIFI_CONNECTED:
                LED_B = 1;delay_ms(50);LED_B = 0;delay_ms(5);LED_B = 1;
				//路由器连接成功，即 LED 心跳闪
            break;
			case WIFI_CONN_CLOUD:
				LED_B = 1;
				//WIFI已经连接上云服务器，即 LED 常亮
			break;
            default:break;
		}
		
		SHT30_Data_Process();		//读取sht30数据并处理得到温湿度值
		all_data_update();			//所有数据上报云端
		
//		TX1_write2buff(SHT30_Temperature);		//串口打印温度
//		TX1_write2buff(SHT30_Humidity);			//串口打印湿度
		
		Handle_Char_Temp(SHT30_Temperature);	//温度数据转换并实时显示
		Handle_Char_Hum(SHT30_Humidity);		//湿度数据转换并实时显示
		
		LCD_Display();		//开关状态显示
		Key_Handle();		//按键处理函数
		Set_Handle();		//温度设置处理函数
	}
}


//========================================================================
// 函数: void Handle_Char_Temp(u16 dat)
// 描述: 温度数据转换字符串函数。
// 参数: 无.
// 返回: none.
//========================================================================
void Handle_Char_Temp(u16 dat)
{
	Temperatur[0] = (u8)((dat/100)%10)+0x30;	//温度十位
	Temperatur[1] = (u8)((dat/10)%10)+0x30;		//温度个位
	Temperatur[2] = '.';
	Temperatur[3] = (u8)((dat/1)%10)+0x30;		//温度小数位
	Temperatur[4] = 'C';
	Temperatur[5] = '\0';
	
	LCD1602_GotoXY(0, 0);						//实时温度显示
	LCD1602_Display_NoXY(Temperatur);
}

//========================================================================
// 函数: void Handle_Char_Hum(u16 dat)
// 描述: 湿度数据转换字符串函数。
// 参数: 无.
// 返回: none.
//========================================================================
void Handle_Char_Hum(u16 dat)
{
	Humidity[0] = (u8)((dat/100)%10)+0x30;		//湿度十位
	Humidity[1] = (u8)((dat/10)%10)+0x30;		//湿度个位
	Humidity[2] = '.';
	Humidity[3] = (u8)((dat/1)%10)+0x30;		//湿度小数位
	Humidity[4] = '%';
	Humidity[5] = '\0';
	
	LCD1602_GotoXY(1, 0);						//实时湿度显示
	LCD1602_Display_NoXY(Humidity);
}

//========================================================================
// 函数: void Handle_Char_SetTemp(u16 dat)
// 描述: int数据转换字符串函数。
// 参数: 无.
// 返回: none.
//========================================================================
void Handle_Char_SetTemp(u16 dat)
{
	Set_Value[0] = (u8)(dat/100)+0x30;
	Set_Value[1] = (u8)(dat%100/10)+0x30;
	Set_Value[2] = '.';
	Set_Value[3] = (u8)(dat%10)+0x30;
	Set_Value[4] = '\0';
}

//========================================================================
// 函数: void LCD_Display()
// 描述: LCD1602显示函数。设定温度、继电器状态显示。
// 参数: 无.
// 返回: none.
//========================================================================
void LCD_Display()
{
	Handle_Char_SetTemp(Set_Temp_Open);
	LCD1602_GotoXY(0, 6);					//温度高设定显示
	LCD1602_Display_NoXY(Set_Value);
	Handle_Char_SetTemp(Set_Temp_Close);
	LCD1602_GotoXY(1, 6);					//温度高设定显示
	LCD1602_Display_NoXY(Set_Value);
	
	if(Key_Model==0)						//设置选项状态显示
	{
		LCD1602_GotoXY(0, 10);				
		LCD1602_Display_NoXY("*");
		LCD1602_GotoXY(1, 10);				
		LCD1602_Display_NoXY(" ");
	}
	if(Key_Model==1)						//设置选项状态显示
	{
		LCD1602_GotoXY(0, 10);
		LCD1602_Display_NoXY(" ");
		LCD1602_GotoXY(1, 10);
		LCD1602_Display_NoXY("*");
	}
	
	LCD1602_GotoXY(0, 12);					//继电器K1状态显示
	LCD1602_Display_NoXY(Table_K1);
	LCD1602_GotoXY(0, 15);
	if(S1==0)
	LCD1602_Display_NoXY(Table_OFF);
	else
	LCD1602_Display_NoXY(Table_ON);
	
	LCD1602_GotoXY(1, 12);					//继电器K2状态显示
	LCD1602_Display_NoXY(Table_K2);
	LCD1602_GotoXY(1, 15);
	if(S2==0)
	LCD1602_Display_NoXY(Table_OFF);
	else
	LCD1602_Display_NoXY(Table_ON);
}

//========================================================================
// 函数: void Key_Handle()
// 描述: 按键处理函数。两个数据切换加减处理。
// 参数: 无.
// 返回: none.
//========================================================================
void Key_Handle()
{
	if(KEY_1==0)
	{
		delay_s(1);
		if(KEY_1==0)
		{
			delay_s(1);
			if(KEY_1==0)
			{	
				mcu_reset_wifi();	//MCU主动重置wifi工作模式
				while(KEY_1==0);
			}
		}
	}
	
	if(KEY_3==0)
	{
		delay_ms(5);
		if(KEY_3==0)
		Key_Model++;
		if(Key_Model==2)
			Key_Model=0;
	}
	if(Key_Model==0)		//启动温度设定值
	{
		if(KEY_4==0)
		{
			delay_ms(5);
			if(KEY_4==0)
			Set_Temp_Open = Set_Temp_Open+10;
		}
		
			if(KEY_2==0)
		{
			delay_ms(5);
			if(KEY_2==0)
			Set_Temp_Open = Set_Temp_Open-10;
		}
	}
	if(Key_Model==1)		//停止温度设定值
	{
		if(KEY_4==0)
		{
			delay_ms(5);
			if(KEY_4==0)
			Set_Temp_Close = Set_Temp_Close+10;
		}
		
			if(KEY_2==0)
		{
			delay_ms(5);
			if(KEY_2==0)
			Set_Temp_Close = Set_Temp_Close-10;
		}
	}
}

//========================================================================
// 函数: void Set_Handle()
// 描述: 设置温度处理函数。处理降温、加热两种模式
// 参数: 无.
// 返回: none.
//========================================================================
void Set_Handle()
{
	if(Set_Temp_Open == Set_Temp_Close)		//设置温度开启=设置温度关闭，两种模式关闭
	{
		S1 = 0;
		Set_Handle_Flag=0;
	}
	if(Set_Temp_Open > Set_Temp_Close)		//设置温度开启>设置温度关闭，降温模式
	{
		if((SHT30_Temperature >= Set_Temp_Open) && (Set_Handle_Flag == 0))
		{
			S1 = 1;
			Set_Handle_Flag=1;
		}
		if((SHT30_Temperature <= Set_Temp_Close) && (Set_Handle_Flag == 1))
		{
			S1 = 0;
			Set_Handle_Flag=0;
		}
	}
	if(Set_Temp_Open < Set_Temp_Close)		//设置温度开启<设置温度关闭，加热模式
	{
		if((SHT30_Temperature < Set_Temp_Open) && (Set_Handle_Flag == 0))
		{
			S1 = 1;
			Set_Handle_Flag=1;
		}
		if((SHT30_Temperature > Set_Temp_Close) && (Set_Handle_Flag == 1))
		{
			S1 = 0;
			Set_Handle_Flag=0;
		}
	}
}


