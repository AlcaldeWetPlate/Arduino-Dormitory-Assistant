/***************************************************************************
* ���°汾�ġ�I2C_wtih_Wire���ᷢ���� http://www.14blog.com/archives/1358
* 
* ��������ⷴ�����뷢�ʼ��� hello14blog@gmail.com
***************************************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "CN_SSD1306.h"
#include "codetab.c"

//������
CN_SSD1306::CN_SSD1306(int sda, int scl)
{
  _sda = sda;
  _scl = scl;
  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
}


//����IIC����
void CN_SSD1306::IIC_Start()
{
  digitalWrite(_scl, HIGH);
  digitalWrite(_sda, HIGH);
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, LOW);
}


//ֹͣIIC����
void CN_SSD1306::IIC_Stop()
{
  digitalWrite(_scl, LOW);
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, HIGH);
  digitalWrite(_sda, HIGH);
}


//ͨ��IICдһ��8λ������,����0xff
void CN_SSD1306::Write_IIC_Byte(unsigned char IIC_Byte)
{
  unsigned char i;
  for(i=0;i<8;i++)
  {
    if((IIC_Byte << i) & 0x80)digitalWrite(_sda, HIGH);
    else
      digitalWrite(_sda, LOW);
    digitalWrite(_scl, HIGH);
    digitalWrite(_scl, LOW);
//    IIC_Byte<<=1;
   }
  digitalWrite(_sda, HIGH);
  digitalWrite(_scl, HIGH);
  digitalWrite(_scl, LOW);
}


//IICд���� -- ����������٣����ٶ�Ӱ�첻��
void CN_SSD1306::Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);  //Slave address,SA0=0
   Write_IIC_Byte(0x00);	//write command
   Write_IIC_Byte(IIC_Command);
   IIC_Stop();
}


//��ʼIICд���� -- ����������һ�����ݷ�����ɺ��ٹر�IIC���ܴܺ�̶�����
void CN_SSD1306::Begin_IIC_Data()
{
   IIC_Start();
   Write_IIC_Byte(0x78);
   Write_IIC_Byte(0x40);	//write data
}


//������ʼ������
void CN_SSD1306::IIC_SetPos(unsigned char x, unsigned char y)
{
  IIC_Start();
  Write_IIC_Byte(0x78);  //Slave address,SA0=0
  Write_IIC_Byte(0x00);	//write command
  
  Write_IIC_Byte(0xb0+y);
  Write_IIC_Byte(((x&0xf0)>>4)|0x10);//|0x10
  Write_IIC_Byte((x&0x0f)|0x01);//|0x01
  
  IIC_Stop();//SetPos����������ʹ��,���Բ��������ַ���һ�������ٹر�IIC���ߵķ�ʽ
}


//ȫ����ʾ -- Fill_Screen(0x00)����������
void CN_SSD1306::Fill_Screen(unsigned char fill_Data)
{
  unsigned char m,n;
  for(m=0;m<8;m++)
  {
    Write_IIC_Command(0xb0+m);	//page0-page1
    Write_IIC_Command(0x00);		//low column start address
    Write_IIC_Command(0x10);		//high column start address
    Begin_IIC_Data();
    for(n=0;n<128;n++)
    {
      Write_IIC_Byte(fill_Data);
    }
    IIC_Stop();
  }
}

//��ʾ16x16������

void CN_SSD1306::ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	IIC_SetPos(x , y);
	Begin_IIC_Data();
	for(wm = 0;wm < 16;wm++)
	{
		Write_IIC_Byte(CN16x16[adder]);
		adder += 1;
	}
	IIC_Stop();
	IIC_SetPos(x,y + 1);
	Begin_IIC_Data();
	for(wm = 0;wm < 16;wm++)
	{
		Write_IIC_Byte(CN16x16[adder]);
		adder += 1;
	}
	IIC_Stop();
}

//SSD1306��ʼ��
void CN_SSD1306::Initial()
{
  Write_IIC_Command(0xAE);//display off
  
  Write_IIC_Command(0x00);//set lower column address
  Write_IIC_Command(0x10);//set higher column address
  
  Write_IIC_Command(0x40);//set display start line
  
  Write_IIC_Command(0xB0);//set page address
  
  Write_IIC_Command(0x81);//�Աȶ�����
  Write_IIC_Command(0xCF);//0~255���Աȶ�ֵ����Ч�������ر����ԣ�
  
  Write_IIC_Command(0xA1);//set segment remap
  
  Write_IIC_Command(0xA6);//normal / reverse
  
  Write_IIC_Command(0xA8);//multiplex ratio
  Write_IIC_Command(0x3F);//duty = 1/64
  
  Write_IIC_Command(0xC8);//Com scan direction
  
  Write_IIC_Command(0xD3);//set display offset
  Write_IIC_Command(0x00);
  
  Write_IIC_Command(0xD5);//set osc division
  Write_IIC_Command(0x80);
  
  Write_IIC_Command(0xD9);//set pre-charge period
  Write_IIC_Command(0xF1);
  
  Write_IIC_Command(0xDA);//set COM pins
  Write_IIC_Command(0x12);
  
  Write_IIC_Command(0xDB);//set vcomh
  Write_IIC_Command(0x40);
  
  Write_IIC_Command(0x8D);//set charge pump enable
  Write_IIC_Command(0x14);
  
  Write_IIC_Command(0xAF);//display ON
}