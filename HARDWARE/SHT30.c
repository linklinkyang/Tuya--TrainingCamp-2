#include <sht30.H>

u8 read_sht_data[7];	//数据读取缓存

u16 SHT30_Temperature;
u16 SHT30_Humidity;


//========================================================================
// 函数: void SHT30_Init(void)
// 描述: SHT30初始化
// 参数: 无.
// 返回: none.
//========================================================================
void SHT30_Init(void)
{	
	EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展SFR(XSFR) */
	Start();              //启动总线
	SendData(0x88);       //发送从器件写地址 
	RecvACK();            //器件应答检测
	SendData(0x20);       //low Repeatability,0.5mps
	RecvACK();            //器件应答检测
	SendData(0x32);       //low Repeatability,0.5mps
	RecvACK();            //器件应答检测
	Stop();
	EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展RAM(XRAM) */
	delay_ms(150);              //延时测量数据，为下一步读取做准备
}

//========================================================================
// 函数: void Read_SHT30(u8 *p,u8 number)
// 描述: SHT30读数据
// 参数: 无.
// 返回: none.
//========================================================================
void Read_SHT30(u8 *p,u8 number)
{
	EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展SFR(XSFR) */
	Start();              //启动总线
	SendData(0x88);       //发送从器件写地址 
	RecvACK();            //器件应答检测
	SendData(0xe0);       //数据获取命令
	RecvACK();            //器件应答检测
	SendData(0x00);       //数据获取命令
	RecvACK();            //器件应答检测
	
	Start();              //启动总线
	SendData(0x89);       //发送从器件读地址 
	RecvACK();            //器件应答检测
          do
		  {
		    *p = RecvData();	//开始逐个读出字节
			p++;
			if(number != 1)
			  SendACK();		//应答
		  }
		  while(--number);		//直至读完数据
		  SendNAK();			//总线非应答
	Stop();	
	EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展RAM(XRAM) */		  
}

//========================================================================
// 函数: int  CRC8_Compute(u8 *check_data, u8 num_of_data)
// 描述: CRC数据校验
// 参数: 无.
// 返回: none.
//========================================================================
int  CRC8_Compute(u8 *check_data, u8 num_of_data)
{
 	u8 i;        // bit mask
    u8 crc = 0xFF; // calculated checksum
    u8 byteCtr;    // byte counter

 // calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < num_of_data; byteCtr++) 
	{
     crc ^= (check_data[byteCtr]);//异或赋值，crc=crc^(check_data[byteCtr])
		
 	//crc校验，最高位是1就^0x31
     for(i = 8; i > 0; --i) 
		{
         if(crc & 0x80) 
			{
             crc = (crc << 1) ^ 0x31;
            }  
		 else 
			{
             crc = (crc << 1);
            }
        }
    }
 return crc;
}

//========================================================================
// 函数: int SHT30_CRC8_Check(u8 *p,u8 num_of_data,u8 CrcData)
// 描述: CRC数据校验缓冲
// 参数: 无.
// 返回: none.
//========================================================================
int SHT30_CRC8_Check(u8 *p,u8 num_of_data,u8 CrcData)
{
  u8 crc;
  crc = CRC8_Compute(p, num_of_data);// calculates 8-Bit checksum
  if(crc != CrcData) 
   {   
    return 1;           
   }
return 0;
}

//========================================================================
// 函数: void SHT30_Data_Process(void)
// 描述: SHT30温湿度数据处理
// 参数: 无.
// 返回: none.
//========================================================================
void SHT30_Data_Process(void)
{
	u8 temporary[3];           //用于暂时存放总线读出的温湿度值
    u16  Data_convert;         //用于数据转换
    u8 crc_result;             //用于CRC校验结果存放，为判断数据准确性做准备
	
	Read_SHT30(read_sht_data,6);  //读出数据放入缓存数组等待处理>>T高八位>>T低八位>>温度T的CRC校验位>>H高八位>>H低八位>>湿度H的CRC校验位
	temporary[0]=read_sht_data[0];//温度数据高八位
    temporary[1]=read_sht_data[1];//温度数据低八位
    temporary[2]=read_sht_data[2];//温度数据CRC校验位
	
    crc_result=SHT30_CRC8_Check(temporary,2,temporary[2]);	 //crc校验，crc校验要是不成功就返回1，
	                                                         //同时不会更新温度值
    if(crc_result==0)  
    {
    Data_convert=(u16)(temporary[0] << 8) | temporary[1];  //把2个8位数据拼接为一个16位的数据
    //温度转换，将16位温度数据转化为10进制的温度数据，
	//这里保留了一位小数，SHT30_Temperature这是一个全局变量，
	//计算温度值（u8强制转换，数据在超过八位范围后会丢失）
    SHT30_Temperature = (u16)((175.0 * (float)(0x7fff&Data_convert) / 65535.0 - 45.0) *10.0); 
    }

    temporary[0]=read_sht_data[3];//湿度数据高八位
    temporary[1]=read_sht_data[4];//湿度数据低八位
    temporary[2]=read_sht_data[5];//湿度数据CRC校验位
  //crc校验
    crc_result=SHT30_CRC8_Check(temporary,2,temporary[2]);   //crc校验，crc校验要是不成功就返回1，
	                                                         //同时不会更新湿度值
  	if(crc_result==0)
    {
    Data_convert = (u16)(temporary[0] << 8) | temporary[1];
    //湿度转换，将16位湿度数据转化为10进制的湿度数据，
	//这里保留了一位小数，SHT30_Humidity这是一个全局变量，
	//计算湿度值（u8强制转换，数据在超过八位范围后会丢失）
    SHT30_Humidity = (u16)((100.0 * (float)(0x7fff&Data_convert) / 65535.0) *10.0); 
    }	
}

