/***************************************************************************
* 最新版本的“I2C_wtih_Wire”会发布在 http://www.14blog.com/archives/1358
* 
* 建议和问题反馈，请发邮件至 hello14blog@gmail.com
***************************************************************************/

#ifndef _CN_SSD1306_H
#define _CN_SSD1306_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#include <Adafruit_GFX.h>

#define BLACK 0
#define WHITE 1

class CN_SSD1306
{
 public:
  CN_SSD1306(int sda, int scl);
  
  void IIC_Start();
  void IIC_Stop();
  void Write_IIC_Byte(unsigned char IIC_Byte);
  void Write_IIC_Command(unsigned char IIC_Command);
  void Begin_IIC_Data();
  void IIC_SetPos(unsigned char x, unsigned char y);
  void Fill_Screen(unsigned char fill_Data);
  void ShowCN(unsigned char x, unsigned char y, unsigned char N);
  void Initial();
  
  private:
  int _sda, _scl;
};
#endif