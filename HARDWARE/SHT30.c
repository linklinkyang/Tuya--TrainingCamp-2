#include <sht30.H>

u8 read_sht_data[7];	//���ݶ�ȡ����

u16 SHT30_Temperature;
u16 SHT30_Humidity;


//========================================================================
// ����: void SHT30_Init(void)
// ����: SHT30��ʼ��
// ����: ��.
// ����: none.
//========================================================================
void SHT30_Init(void)
{	
	EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չSFR(XSFR) */
	Start();              //��������
	SendData(0x88);       //���ʹ�����д��ַ 
	RecvACK();            //����Ӧ����
	SendData(0x20);       //low Repeatability,0.5mps
	RecvACK();            //����Ӧ����
	SendData(0x32);       //low Repeatability,0.5mps
	RecvACK();            //����Ӧ����
	Stop();
	EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չRAM(XRAM) */
	delay_ms(150);              //��ʱ�������ݣ�Ϊ��һ����ȡ��׼��
}

//========================================================================
// ����: void Read_SHT30(u8 *p,u8 number)
// ����: SHT30������
// ����: ��.
// ����: none.
//========================================================================
void Read_SHT30(u8 *p,u8 number)
{
	EAXSFR();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չSFR(XSFR) */
	Start();              //��������
	SendData(0x88);       //���ʹ�����д��ַ 
	RecvACK();            //����Ӧ����
	SendData(0xe0);       //���ݻ�ȡ����
	RecvACK();            //����Ӧ����
	SendData(0x00);       //���ݻ�ȡ����
	RecvACK();            //����Ӧ����
	
	Start();              //��������
	SendData(0x89);       //���ʹ���������ַ 
	RecvACK();            //����Ӧ����
          do
		  {
		    *p = RecvData();	//��ʼ��������ֽ�
			p++;
			if(number != 1)
			  SendACK();		//Ӧ��
		  }
		  while(--number);		//ֱ����������
		  SendNAK();			//���߷�Ӧ��
	Stop();	
	EAXRAM();		/* MOVX A,@DPTR/MOVX @DPTR,Aָ��Ĳ�������Ϊ��չRAM(XRAM) */		  
}

//========================================================================
// ����: int  CRC8_Compute(u8 *check_data, u8 num_of_data)
// ����: CRC����У��
// ����: ��.
// ����: none.
//========================================================================
int  CRC8_Compute(u8 *check_data, u8 num_of_data)
{
 	u8 i;        // bit mask
    u8 crc = 0xFF; // calculated checksum
    u8 byteCtr;    // byte counter

 // calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < num_of_data; byteCtr++) 
	{
     crc ^= (check_data[byteCtr]);//���ֵ��crc=crc^(check_data[byteCtr])
		
 	//crcУ�飬���λ��1��^0x31
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
// ����: int SHT30_CRC8_Check(u8 *p,u8 num_of_data,u8 CrcData)
// ����: CRC����У�黺��
// ����: ��.
// ����: none.
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
// ����: void SHT30_Data_Process(void)
// ����: SHT30��ʪ�����ݴ���
// ����: ��.
// ����: none.
//========================================================================
void SHT30_Data_Process(void)
{
	u8 temporary[3];           //������ʱ������߶�������ʪ��ֵ
    u16  Data_convert;         //��������ת��
    u8 crc_result;             //����CRCУ������ţ�Ϊ�ж�����׼ȷ����׼��
	
	Read_SHT30(read_sht_data,6);  //�������ݷ��뻺������ȴ�����>>T�߰�λ>>T�Ͱ�λ>>�¶�T��CRCУ��λ>>H�߰�λ>>H�Ͱ�λ>>ʪ��H��CRCУ��λ
	temporary[0]=read_sht_data[0];//�¶����ݸ߰�λ
    temporary[1]=read_sht_data[1];//�¶����ݵͰ�λ
    temporary[2]=read_sht_data[2];//�¶�����CRCУ��λ
	
    crc_result=SHT30_CRC8_Check(temporary,2,temporary[2]);	 //crcУ�飬crcУ��Ҫ�ǲ��ɹ��ͷ���1��
	                                                         //ͬʱ��������¶�ֵ
    if(crc_result==0)  
    {
    Data_convert=(u16)(temporary[0] << 8) | temporary[1];  //��2��8λ����ƴ��Ϊһ��16λ������
    //�¶�ת������16λ�¶�����ת��Ϊ10���Ƶ��¶����ݣ�
	//���ﱣ����һλС����SHT30_Temperature����һ��ȫ�ֱ�����
	//�����¶�ֵ��u8ǿ��ת���������ڳ�����λ��Χ��ᶪʧ��
    SHT30_Temperature = (u16)((175.0 * (float)(0x7fff&Data_convert) / 65535.0 - 45.0) *10.0); 
    }

    temporary[0]=read_sht_data[3];//ʪ�����ݸ߰�λ
    temporary[1]=read_sht_data[4];//ʪ�����ݵͰ�λ
    temporary[2]=read_sht_data[5];//ʪ������CRCУ��λ
  //crcУ��
    crc_result=SHT30_CRC8_Check(temporary,2,temporary[2]);   //crcУ�飬crcУ��Ҫ�ǲ��ɹ��ͷ���1��
	                                                         //ͬʱ�������ʪ��ֵ
  	if(crc_result==0)
    {
    Data_convert = (u16)(temporary[0] << 8) | temporary[1];
    //ʪ��ת������16λʪ������ת��Ϊ10���Ƶ�ʪ�����ݣ�
	//���ﱣ����һλС����SHT30_Humidity����һ��ȫ�ֱ�����
	//����ʪ��ֵ��u8ǿ��ת���������ڳ�����λ��Χ��ᶪʧ��
    SHT30_Humidity = (u16)((100.0 * (float)(0x7fff&Data_convert) / 65535.0) *10.0); 
    }	
}

