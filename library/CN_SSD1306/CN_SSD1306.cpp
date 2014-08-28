/***************************************************************************
* 最新版本的“I2C_wtih_Wire”会发布在 http://www.14blog.com/archives/1358
* 
* 建议和问题反馈，请发邮件至 hello14blog@gmail.com
***************************************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "CN_SSD1306.h"
#include "codetab.c"

//定义类
CN_SSD1306::CN_SSD1306(int sda, int scl)
{
  _sda = sda;
  _scl = scl;
  pinMode(sda, OUTPUT);
  pinMode(scl, OUTPUT);
}


//开启IIC总线
void CN_SSD1306::IIC_Start()
{
  digitalWrite(_scl, HIGH);
  digitalWrite(_sda, HIGH);
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, LOW);
}


//停止IIC总线
void CN_SSD1306::IIC_Stop()
{
  digitalWrite(_scl, LOW);
  digitalWrite(_sda, LOW);
  digitalWrite(_scl, HIGH);
  digitalWrite(_sda, HIGH);
}


//通过IIC写一个8位的数据,比如0xff
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


//IIC写命令 -- 命令操作很少，对速度影响不大
void CN_SSD1306::Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);  //Slave address,SA0=0
   Write_IIC_Byte(0x00);	//write command
   Write_IIC_Byte(IIC_Command);
   IIC_Stop();
}


//开始IIC写数据 -- 这样可以让一组数据发送完成后再关闭IIC，能很大程度提速
void CN_SSD1306::Begin_IIC_Data()
{
   IIC_Start();
   Write_IIC_Byte(0x78);
   Write_IIC_Byte(0x40);	//write data
}


//设置起始点坐标
void CN_SSD1306::IIC_SetPos(unsigned char x, unsigned char y)
{
  IIC_Start();
  Write_IIC_Byte(0x78);  //Slave address,SA0=0
  Write_IIC_Byte(0x00);	//write command
  
  Write_IIC_Byte(0xb0+y);
  Write_IIC_Byte(((x&0xf0)>>4)|0x10);//|0x10
  Write_IIC_Byte((x&0x0f)|0x01);//|0x01
  
  IIC_Stop();//SetPos函数经常被使用,所以采用了这种发送一组命令再关闭IIC总线的方式
}


//全屏显示 -- Fill_Screen(0x00)可用作清屏
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

//显示16x16的中文

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

//SSD1306初始化
void CN_SSD1306::Initial()
{
  Write_IIC_Command(0xAE);//display off
  
  Write_IIC_Command(0x00);//set lower column address
  Write_IIC_Command(0x10);//set higher column address
  
  Write_IIC_Command(0x40);//set display start line
  
  Write_IIC_Command(0xB0);//set page address
  
  Write_IIC_Command(0x81);//对比度设置
  Write_IIC_Command(0xCF);//0~255（对比度值……效果不是特别明显）
  
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