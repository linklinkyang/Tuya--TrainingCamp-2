/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
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


/***************	����˵��	****************

Yang������Ϳѻ����������ʪ�ȿ�������

�����̻���STC8H8K64UΪ����оƬ��ʵ����8���б�д���ԣ�STC8G��STC8Hϵ��оƬ��ͨ�òο�.

����ʹ��P6������ʾ����ƣ������������

����ʱ, ѡ��ʱ�� 24MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************	���س�������	**************/
#define Beep 	P12		//������
#define LED_B	P16		//WiFiָʾ�ƣ�WHITE
#define LED_R	P17		//��Դָʾ�ƣ�RED
#define S1		P13		//�̵�������S1
#define S2		P54		//�̵�������S2
#define KEY_1	P34		//����KEY_1
#define KEY_2	P35		//����KEY_2
#define KEY_3	P36		//����KEY_3
#define KEY_4	P37		//����KEY_4

u8 table1[] 	= "TuYa & LCEDA    ";  //16
u8 table2[] 	= "Humiture Control";  //16
u8 Table_K1[] 	= "K1:";
u8 Table_K2[] 	= "K2:";
u8 Table_ON[] 	= "-";
u8 Table_OFF[] 	= "x";

/*************	���ر�������	**************/
//u8 ledIndex;
u16 Set_Temp_Open = 250;	//�趨�¶�ֵ
u16 Set_Temp_Close = 250;	//�趨�¶�ֵ
u16 Set_Hum = 250;			//�趨ʪ��ֵ
u8 Key_Model=0;				//����ģʽ

bit Set_Handle_Flag=0;
bit Wifi_Reset_Flag=0;		//��λwifi�ɹ���־

u8 Temperatur[6];		//�¶�����ת���ַ�������
u8 Humidity[6];			//ʪ������ת���ַ�������
u8 Set_Value[5];		//int����ת���ַ�������

/*************	���غ�������	**************/
void Handle_Char_Temp(u16 dat);			//�¶�����ת���ַ�������
void Handle_Char_Hum(u16 dat);			//ʪ������ת���ַ�������
void Handle_Char_SetTemp(u16 dat);		//int����ת���ַ�������
void LCD_Display();						//LCD1602��ʾ����
void Key_Handle();						//����������
void Set_Handle();						//�¶����ô�����

/*************  �ⲿ�����ͱ������� *****************/


/******************** IO���ú��� **************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��
	Beep = 0;LED_B = 0;LED_R = 0;S1 = 0;
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_PullUp;	//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��
	
	GPIO_InitStructure.Pin  = GPIO_Pin_4;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);	//��ʼ��
	S2 = 0;
}

/****************  I2C��ʼ������ *****************/
void	I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;
	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//����ѡ��   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C����ʹ��,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;			//����ʹ���Զ�����,  ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_Interrupt = DISABLE;		//ʹ������ģʽ�ж�,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 16;				//�����ٶ�=Fosc/2/(Speed*2+4),      0~63
	I2C_InitStructure.I2C_IoUse     = I2C_P14_P15;		//IO���л�   I2C_P14_P15, I2C_P24_P25, I2C_P33_P32

	I2C_Init(&I2C_InitStructure);
}

/***************  ���ڳ�ʼ������ *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = Polity_0;			//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
	UART_Configuration(UART1, &COMx_InitStructure);			//��ʼ������1 UART1,UART2,UART3,UART4

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = Polity_0;			//ָ���ж����ȼ�(�͵���) Polity_0,Polity_1,Polity_2,Polity_3
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//�л��˿�,   UART2_SW_P10_P11,UART2_SW_P46_P47
	UART_Configuration(UART2, &COMx_InitStructure);			//��ʼ������2 UART1,UART2,UART3,UART4
	
	//PrintString1("STC8H8K64U UART1 Test Programme!\r\n");	//UART1����һ���ַ���
	//PrintString2("STC8H8K64U UART2 Test Programme!\r\n");	//UART2����һ���ַ���
}


/******************** ������ **************************/
void main(void)
{
	delay_ms(100);			//�ϵ�ȴ�ϵͳ�ȶ�
	delay_us(50);
	GPIO_config();			//IO��ʼ��
	I2C_config();			//IIC��ʼ��
	UART_config();			//���ڳ�ʼ��
	LCD_Init();				//LCD1602��ʼ��
	SHT30_Init(); 			//SHT30��ʼ��
	wifi_protocol_init();	//Э�鴮�ڳ�ʼ������ mcu_api.c
	EA = 1;					//�����ж�
		
	//Beep = 1;delay_ms(250);Beep = 0;	//�ϵ��������ʾ
	LED_R = 1;							//�ϵ�򿪵�Դָʾ��
	
	LCD1602_GotoXY(0, 0);			//��ʾ�ַ���
	LCD1602_Display_NoXY(table1);
	LCD1602_GotoXY(1, 0);			//��ʾ�ַ���
	LCD1602_Display_NoXY(table2);
	delay_s(2);						//��ʾ2��
	LCD1602_ClearDisplay();			//LCD����

	while(1)
	{	
		wifi_uart_service();		//wifi�������ݴ������ mcu_api.c
		
		switch(mcu_get_wifi_work_state())
		{
			case SMART_CONFIG_STATE:
                LED_B =! LED_B;delay_ms(1);
				//���� Smart ����״̬���� LED ����
            break;
            case AP_STATE:
				LED_B =! LED_B;delay_ms(50);
                //���� AP ����״̬���� LED ����
            break;
            case WIFI_NOT_CONNECTED:
                LED_B = 0;
				//Wi-Fi ������ɣ���������·�������� LED ����
            break;
            case WIFI_CONNECTED:
                LED_B = 1;delay_ms(50);LED_B = 0;delay_ms(5);LED_B = 1;
				//·�������ӳɹ����� LED ������
            break;
			case WIFI_CONN_CLOUD:
				LED_B = 1;
				//WIFI�Ѿ��������Ʒ��������� LED ����
			break;
            default:break;
		}
		
		SHT30_Data_Process();		//��ȡsht30���ݲ�����õ���ʪ��ֵ
		all_data_update();			//���������ϱ��ƶ�
		
//		TX1_write2buff(SHT30_Temperature);		//���ڴ�ӡ�¶�
//		TX1_write2buff(SHT30_Humidity);			//���ڴ�ӡʪ��
		
		Handle_Char_Temp(SHT30_Temperature);	//�¶�����ת����ʵʱ��ʾ
		Handle_Char_Hum(SHT30_Humidity);		//ʪ������ת����ʵʱ��ʾ
		
		LCD_Display();		//����״̬��ʾ
		Key_Handle();		//����������
		Set_Handle();		//�¶����ô�����
	}
}


//========================================================================
// ����: void Handle_Char_Temp(u16 dat)
// ����: �¶�����ת���ַ���������
// ����: ��.
// ����: none.
//========================================================================
void Handle_Char_Temp(u16 dat)
{
	Temperatur[0] = (u8)((dat/100)%10)+0x30;	//�¶�ʮλ
	Temperatur[1] = (u8)((dat/10)%10)+0x30;		//�¶ȸ�λ
	Temperatur[2] = '.';
	Temperatur[3] = (u8)((dat/1)%10)+0x30;		//�¶�С��λ
	Temperatur[4] = 'C';
	Temperatur[5] = '\0';
	
	LCD1602_GotoXY(0, 0);						//ʵʱ�¶���ʾ
	LCD1602_Display_NoXY(Temperatur);
}

//========================================================================
// ����: void Handle_Char_Hum(u16 dat)
// ����: ʪ������ת���ַ���������
// ����: ��.
// ����: none.
//========================================================================
void Handle_Char_Hum(u16 dat)
{
	Humidity[0] = (u8)((dat/100)%10)+0x30;		//ʪ��ʮλ
	Humidity[1] = (u8)((dat/10)%10)+0x30;		//ʪ�ȸ�λ
	Humidity[2] = '.';
	Humidity[3] = (u8)((dat/1)%10)+0x30;		//ʪ��С��λ
	Humidity[4] = '%';
	Humidity[5] = '\0';
	
	LCD1602_GotoXY(1, 0);						//ʵʱʪ����ʾ
	LCD1602_Display_NoXY(Humidity);
}

//========================================================================
// ����: void Handle_Char_SetTemp(u16 dat)
// ����: int����ת���ַ���������
// ����: ��.
// ����: none.
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
// ����: void LCD_Display()
// ����: LCD1602��ʾ�������趨�¶ȡ��̵���״̬��ʾ��
// ����: ��.
// ����: none.
//========================================================================
void LCD_Display()
{
	Handle_Char_SetTemp(Set_Temp_Open);
	LCD1602_GotoXY(0, 6);					//�¶ȸ��趨��ʾ
	LCD1602_Display_NoXY(Set_Value);
	Handle_Char_SetTemp(Set_Temp_Close);
	LCD1602_GotoXY(1, 6);					//�¶ȸ��趨��ʾ
	LCD1602_Display_NoXY(Set_Value);
	
	if(Key_Model==0)						//����ѡ��״̬��ʾ
	{
		LCD1602_GotoXY(0, 10);				
		LCD1602_Display_NoXY("*");
		LCD1602_GotoXY(1, 10);				
		LCD1602_Display_NoXY(" ");
	}
	if(Key_Model==1)						//����ѡ��״̬��ʾ
	{
		LCD1602_GotoXY(0, 10);
		LCD1602_Display_NoXY(" ");
		LCD1602_GotoXY(1, 10);
		LCD1602_Display_NoXY("*");
	}
	
	LCD1602_GotoXY(0, 12);					//�̵���K1״̬��ʾ
	LCD1602_Display_NoXY(Table_K1);
	LCD1602_GotoXY(0, 15);
	if(S1==0)
	LCD1602_Display_NoXY(Table_OFF);
	else
	LCD1602_Display_NoXY(Table_ON);
	
	LCD1602_GotoXY(1, 12);					//�̵���K2״̬��ʾ
	LCD1602_Display_NoXY(Table_K2);
	LCD1602_GotoXY(1, 15);
	if(S2==0)
	LCD1602_Display_NoXY(Table_OFF);
	else
	LCD1602_Display_NoXY(Table_ON);
}

//========================================================================
// ����: void Key_Handle()
// ����: ���������������������л��Ӽ�����
// ����: ��.
// ����: none.
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
				mcu_reset_wifi();	//MCU��������wifi����ģʽ
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
	if(Key_Model==0)		//�����¶��趨ֵ
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
	if(Key_Model==1)		//ֹͣ�¶��趨ֵ
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
// ����: void Set_Handle()
// ����: �����¶ȴ������������¡���������ģʽ
// ����: ��.
// ����: none.
//========================================================================
void Set_Handle()
{
	if(Set_Temp_Open == Set_Temp_Close)		//�����¶ȿ���=�����¶ȹرգ�����ģʽ�ر�
	{
		S1 = 0;
		Set_Handle_Flag=0;
	}
	if(Set_Temp_Open > Set_Temp_Close)		//�����¶ȿ���>�����¶ȹرգ�����ģʽ
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
	if(Set_Temp_Open < Set_Temp_Close)		//�����¶ȿ���<�����¶ȹرգ�����ģʽ
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


