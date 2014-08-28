/****************************************************************************/  
//
// Arduino Dormitory Assistant
//
// Author:  Miyeah Chen  
// Date:    August 27,2014
// Version: v1.1
//
// [https://github.com/Miyeah/]
//
/****************************************************************************/ 
// 
// [Lisence]: GPL
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
/****************************************************************************/

#include <EEPROM.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>
#include <Wire.h>
#include <DS1307.h>
#include <dht11.h>
#include <Adafruit_ssd1306syp.h>
#include <CN_SSD1306.h>

#define SDA_PIN 8
#define SCL_PIN 9
#define TrigPin 30
#define EchoPin 28 
#define buzzer 2
#define SD_ChipSelectPin 53
#define hongwai 24
#define DHT11PIN 26
#define TOUCH_SENSOR 22
#define LED 3
#define button 32
#define relay 34
  
Adafruit_ssd1306syp display(SDA_PIN,SCL_PIN);
CN_SSD1306 lucky(SDA_PIN, SCL_PIN);
dht11 DHT11;
TMRpcm tmrpcm;
DS1307 clock;
File myFile;

unsigned long times = 0;
String dictconfig;
int address = 0;

void setup() {

  pinMode(hongwai,INPUT);
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 
  pinMode(buzzer,OUTPUT);
  pinMode(TOUCH_SENSOR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(SD_ChipSelectPin, OUTPUT);
  pinMode(button,INPUT_PULLUP);

  clock.begin();
  display.initialize();
  SD.begin(SD_ChipSelectPin);
  tmrpcm.speakerPin = 5;
  dictconfig=readconfig("dict");

  randomSeed(analogRead(0));
  Serial.begin(9600);
  firststart();
  display.clear();
  delay(1000);

}

void loop() {

screen();
touch();
strikehour();
clock_alarm();
eyeprotect();
temp_hum_alarm();
warning_core();

}

//开机后声音提示
void firststart() {

  tmrpcm.speakerPin = 5;
  tmrpcm.play("start.ahm");
}

//显示时间屏
void displaytime() {

  clock.getTime();
  DHT11.read(DHT11PIN);

  int ye,mo,da,ho,mi,se;
  int hu,te;
  String dow;

  ye=clock.year+2000;
  mo=clock.month;
  da=clock.dayOfMonth;
  ho=clock.hour;
  mi=clock.minute;
  se=clock.second;

  switch (clock.dayOfWeek)
  {
    case MON:
      dow="MON";
      break;
    case TUE:
      dow="TUE";
      break;
    case WED:
      dow="WED";
      break;
    case THU:
      dow="THU";
      break;
    case FRI:
      dow="FRI";
      break;
    case SAT:
      dow="SAT";
      break;
    case SUN:
      dow="SUN";
      break;
  }

  hu=(float)DHT11.humidity;
  te=(float)DHT11.temperature;

  String topbar,footbar,content;
  String daycountstr;

  daycountstr=daycount();

  topbar="|"+dow+"|  "+daycountstr+"  "+ye+"-"+fill(String(mo,DEC))+"-"+fill(String(da,DEC));
  footbar="TEMP:"+String(te,DEC)+"'C     HUM:"+String(hu,DEC)+"%";
  content=fill(String(ho,DEC))+":"+fill(String(mi,DEC));

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(topbar);
  //display.println("        ");
  display.setTextSize(1);
  display.print(" ");
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.print(content);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println();
  display.println(footbar);

  display.update();  
  delay(800);
  display.clear();

}

//补充零之用，如2014.8.3->2014.08.03
String fill(String str) {
  String returnstr;
  returnstr=str;
  if(str.length() == 1){
      returnstr="0"+str;
  }
  return returnstr;
}

//从SD卡中读取单词信息
String readword(String name,int number) {

  String findstr;
  String filenames;
  String final;

  findstr = name;

  pinMode(SD_ChipSelectPin,OUTPUT);

  filenames="/dict/"+dictconfig+"/"+String(number,DEC)+".txt";
  const char* c_s = filenames.c_str();

  myFile = SD.open(c_s);

  if (myFile) {

    String sss;
    char ls;
    sss="";

    while (myFile.available()) {
      ls=myFile.read();
      sss=sss+ls;
    }

    int from,to;
    
    from=sss.lastIndexOf("<"+findstr+">");
    to=sss.lastIndexOf("</"+findstr+">");
    final=sss.substring(from+findstr.length()+2,to);
    //Serial.println(findstr+":"+final);
    sss="";
    from=0;
    to=0;
    //return final;
    myFile.close();

  } else {
    //Serial.println("error opening files");
  }
    return final;
}

//显示单词
void displayword() {

  unsigned char i;

  //display.clear();
  //int randomnum;
  // int randomnummax;

  // randomnummax=1000;
  // if(dictconfig == "G"){randomnummax=1357;};
  // if(dictconfig == "T"){randomnummax=1814;};
  // if(dictconfig == "6"){randomnummax=1213;};
  // if(dictconfig == "Y"){randomnummax=2551;};

  // randomnum=random(1, randomnummax);

  int randomnum;
  int g,s,b,q;
  q=EEPROM.read(2);
  b=EEPROM.read(3);
  s=EEPROM.read(4);
  g=EEPROM.read(5);
  randomnum=q*1000+b*100+s*10+g;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(readword("word",randomnum));//9
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.println("a break in friendly relations between people or groupssss");
  display.println(readword("meaning",randomnum));//62
  display.println("<------------------>");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(readword("example",randomnum)); //42
  //display.println("eg:a breach of their contractsss");

  display.update();
  delay(3000);
  display.clear();

}

//读取SD卡中的配置文件（一级配置信息）
String readconfig(String name) {

  String findstr;
  String final;

  findstr = name;

  myFile = SD.open("config.txt");

  if (myFile) {

    String sss;
    char ls;
    sss="";

    while (myFile.available()) {
      ls=myFile.read();
      sss=sss+ls;
    }

    int from,to;
    
    from=sss.lastIndexOf("<"+findstr+">");
    to=sss.lastIndexOf("</"+findstr+">");
    final=sss.substring(from+findstr.length()+2,to);
    //Serial.println(findstr+":"+final);
    sss="";
    from=0;
    to=0;
    //return final;
    myFile.close();

  } else {
    //Serial.println("error opening files");
  }
    return final;
}

//读取SD卡中配置文件的二级配置信息
String readsubconfig(String content,String name) {

  String findstr;
  String final;
  String sss;  
  char ls;  
  int from,to;  

  findstr = name;
  sss=content;
    
  from=sss.lastIndexOf("<"+findstr+">");
  to=sss.lastIndexOf("</"+findstr+">");
  final=sss.substring(from+findstr.length()+2,to);

  sss="";
  from=0;
  to=0;

  return final;
}

//考试周倒计时功能，返回剩余天数
String daycount() {

  String enable;
  int before,year,month,day;

  year=atoi(readsubconfig(readconfig("daycount"),"year").c_str());
  month=atoi(readsubconfig(readconfig("daycount"),"month").c_str());
  day=atoi(readsubconfig(readconfig("daycount"),"day").c_str());
  before=atoi(readsubconfig(readconfig("daycount"),"before").c_str());
  enable=readsubconfig(readconfig("daycount"),"enable");

  if(enable == "yes"){
     int mo,da,ye,days;
     clock.getTime();
     mo=clock.month;
     da=clock.dayOfMonth;
     ye=clock.year+2000;
     days=daycount_core(ye,mo,da,year,month,day);
     if(days>0){
        if(days<=before){
         String returnstr;
         returnstr=String(days,DEC);
         returnstr=fill(returnstr);
         return returnstr;          
       } else {return "  ";}
     } else {return "  ";}
  } else {
      return "  ";
  }

}

//计算两日期的相差天数（辅助函数）
long sumday(int y,int m,int d)
{

 int x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 long i,s=0;
 for(i=1;i<y;i++)
  if(i%4==0&&i%100!=0||i%400==0) s+=366;
  else s+=365;//整年份的天数
 if(y%4==0&&y%100!=0||y%400==0)x[2]=29;
 for(i=1;i<m;i++)
  s+=x[i];//整月的天数
 s+=d;//日的天数
 return s;//返回总天数,相对公元1年

}

//计算两日期的相差天数
int daycount_core(int y1,int m1,int d1,int y2,int m2,int d2)
{

 long s1,s2;
 s1=sumday(y1,m1,d1);
 s2=sumday(y2,m2,d2);
 return (s2-s1);

}

//主屏幕显示，显示时间屏或单词屏
void screen() {

  byte value;
  value = EEPROM.read(address);
  int screenstate,screenconfig;
  screenstate = value;
  screenconfig = atoi(readconfig("wordscreen").c_str());

  if(screenstate == 1){

      displaytime();

  } else {

      displayword();
  }

}

//整点报时功能
void strikehour() {

 clock.getTime();
 int inhour,insecond;
 inhour=clock.hour;
 insecond=clock.second;

 if((String(clock.minute,DEC)=="0")&&(insecond<10))
 {
    if((inhour<=23)&&(inhour>=7)){
      playmusic("clock");
    }
 }
 
}

//超声波传感器读取距离
float distance() {

	float distances; 

    // 产生一个10us的高脉冲去触发TrigPin 
    digitalWrite(TrigPin, LOW); 
    delayMicroseconds(2); 
    digitalWrite(TrigPin, HIGH); 
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW); 
    // 检测脉冲宽度，并计算出距离
    distances = pulseIn(EchoPin, HIGH) / 58.00;
    delay(200); 
    return distances;
}

//蜂鸣器发生噪声（赖床闹钟用）
void fmtone() {

 unsigned char i,j;

  for(int k=0; k<5; k++){
  
    for(i=0;i<80;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(1);
      digitalWrite(buzzer,LOW);
      delay(1);
    }
    for(i=0;i<100;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(2);
      digitalWrite(buzzer,LOW);
      delay(2);
    }

  }

}

//播放SD卡中的指定音乐
void playmusic(String name) {

String filename;
filename=name+".ahm";
char cstr1[80];
strcpy(cstr1,filename.c_str());
tmrpcm.play(cstr1);

}

//闹钟功能
void clock_alarm() {

String menable,nenable;
float distances;
int mhour,mminute,mtolerance;
int nhour,nminute,ntolerance;
int ho,mi,to,di,dlimit;

distances=distance();
di=(int)distances;
dlimit=atoi(readconfig("distance").c_str());

mhour=atoi(readsubconfig(readconfig("morningclock"),"hour").c_str());
mminute=atoi(readsubconfig(readconfig("morningclock"),"minute").c_str());
mtolerance=atoi(readsubconfig(readconfig("morningclock"),"tolerance").c_str());
menable=readsubconfig(readconfig("morningclock"),"enable");

nhour=atoi(readsubconfig(readconfig("nightclock"),"hour").c_str());
nminute=atoi(readsubconfig(readconfig("nightclock"),"minute").c_str());
ntolerance=atoi(readsubconfig(readconfig("nightclock"),"tolerance").c_str());
nenable=readsubconfig(readconfig("nightclock"),"enable");

clock.getTime();
ho=clock.hour;
mi=clock.minute;

int clockmm,clocknm,now;
clockmm=mhour*60+mminute;
clocknm=nhour*60+nminute;
now=ho*60+mi;

if((now>=clockmm)&&(now<=720)){
  if(menable=="yes"){
    if(di<dlimit){
        
        to=(ho*60+mi)-(mhour*60+mminute);
        if(to<=mtolerance){
            if(daycount()!="  "){
                playmusic("exam");
                delay(3000);
            }
            playmusic("morning");
        } else {
            fmtone();
        }
      }
   }

}

if((now>=clocknm)&&(now<=1440)||(now>=0)&&(now<=240)){
  if(nenable=="yes"){
    if(di>dlimit){

        to=abs((ho*60+mi)-(nhour*60+nminute));
        if(to<=ntolerance){
            if(daycount()!="  "){
                playmusic("exam");
                delay(3000);
            }
            playmusic("night");
        } else {
            fmtone();
        }
      }
   }
}


}

//蜂鸣器发出警报（小偷探测用）
void warning() {

  for(int i=0;i<100;i++){
  digitalWrite(buzzer,HIGH); //发声音
  delay(2);
  digitalWrite(buzzer,LOW);  //不发声音
  delay(2);                  //修改延时时间，改变发声频率
  }

}

//读取人体红外传感器状态，判断是否有人闯入
int hwsensor() {

 int val=digitalRead(hongwai);  //定义参数存储人体红外传感器读到的状态
 if(val == 1)
 {
  return 1;
 }
 else
  {
    return 0;
  }
 //delay(100);
}

//触摸开关，切换屏幕用
void touch() {

  int touchaddress=1;
  int sensorValue = digitalRead(TOUCH_SENSOR);
  int screenstate,screenconfig,lasttouch;
  byte value;
  value = EEPROM.read(address);  

  lasttouch = EEPROM.read(touchaddress);
  screenstate = value;
  screenconfig = atoi(readconfig("wordscreen").c_str());
  // Serial.println(screenstate);
  // Serial.println();
  // Serial.println(lasttouch);
  if(sensorValue != lasttouch){

    int randomnummax,randomnum;
    randomnummax=1000;
    if(dictconfig == "G"){randomnummax=1357;};
    if(dictconfig == "T"){randomnummax=1814;};
    if(dictconfig == "6"){randomnummax=1213;};
    if(dictconfig == "Y"){randomnummax=2551;};
    randomnum=random(1, randomnummax);
    int g,s,b,q;
    q=randomnum/1000;
    b=(randomnum-q*1000)/100;
    s=(randomnum-q*1000-b*100)/10;
    g=randomnum-q*1000-b*100-s*10-g;

    EEPROM.write(2, q);
    EEPROM.write(3, b);
    EEPROM.write(4, s);
    EEPROM.write(5, g);

    if(screenstate == screenconfig){
        EEPROM.write(address, 1);
    } else {
        EEPROM.write(address, screenstate+1);
    }

    EEPROM.write(touchaddress, sensorValue);
  }

}

//眼保健操功能
void eyeprotect() {

 int buttonState;
 buttonState = digitalRead(button);
  if (buttonState == LOW) {    
        delay(10); //延时，消除抖动的影响
        buttonState = digitalRead(button);
        if (buttonState == LOW) { 
          playmusic("eye");
        }  
  } 

}

//湿度温度报警功能
void temp_hum_alarm() {

  String tenable,henable,ts,hs;
  int maxtem,mintem,maxhum,minhum,finaltemp,finalhum;
  int hu,te;

  maxtem=atoi(readsubconfig(readconfig("tempalarm"),"max").c_str());
  mintem=atoi(readsubconfig(readconfig("tempalarm"),"min").c_str());
  tenable=readsubconfig(readconfig("tempalarm"),"enable");

  maxhum=atoi(readsubconfig(readconfig("humalarm"),"max").c_str());
  minhum=atoi(readsubconfig(readconfig("humalarm"),"min").c_str());
  henable=readsubconfig(readconfig("humalarm"),"enable");

  DHT11.read(DHT11PIN);
  hu=(int)DHT11.humidity;
  te=(int)DHT11.temperature;
  ts=String(te,DEC);
  hs=String(hu,DEC);

  finaltemp=atoi(ts.c_str());
  finalhum=atoi(hs.c_str());

  clock.getTime();
  int second;
  second=clock.second;
  
  if(((String(clock.minute,DEC)=="0")||(String(clock.minute,DEC)=="15")||(String(clock.minute,DEC)=="30")||(String(clock.minute,DEC)=="45"))&&(second<10)){
      
    if(tenable=="yes"){
        if(finaltemp>maxtem){
           playmusic("temhigh"); 
        }
        if(finaltemp<mintem){
           playmusic("temlow"); 
        }
    }
    if(henable=="yes"){
        if(finalhum>maxhum){
           playmusic("humhigh"); 
        }
        if(finalhum<minhum){
           playmusic("humlow"); 
        }
    }

  }

}

//小偷入侵时屏幕显示WARNING
void displayalarm() {

  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0,0);
  display.print(" WARNING!                                                                             ");

  display.update();
  delay(900);
  display.clear();

}

//入侵报警核心函数
void warning_core() {

  int starts,ends;
  String wenable,lenable;

  starts=atoi(readsubconfig(readconfig("warning"),"start").c_str());
  ends=atoi(readsubconfig(readconfig("warning"),"end").c_str());
  wenable=readsubconfig(readconfig("warning"),"enable");
  lenable=readsubconfig(readconfig("log"),"enable");

  if(hwsensor()==1){

  clock.getTime();
  int ye,mo,da,ho,mi,se;
  String logs;
  ye=clock.year+2000;
  mo=clock.month;
  da=clock.dayOfMonth;
  ho=clock.hour;
  mi=clock.minute;
  se=clock.second;
  logs="Detected intrusion - "+String(ye,DEC)+"-"+fill(String(mo,DEC))+"-"+fill(String(da,DEC))+" "+fill(String(ho,DEC))+":"+fill(String(mi,DEC))+":"+fill(String(se,DEC));

  myFile = SD.open("log.txt", FILE_WRITE);
  if (myFile) {
    if(lenable=="yes"){
      myFile.println(logs);        
    }

    myFile.close();
  } else {
    //Serial.println("error opening log.txt");
  }

  if(wenable=="yes"){
      if((ho>=starts)&&(ho<=ends)){  
            for(int i=0; i<8; i++){
              displayalarm();
              playmusic("alarm");
              for(int i=0; i<12; i++){
                digitalWrite(LED, HIGH);
                digitalWrite(relay, HIGH);
                warning();
              }      
          }
      }      
  }

  digitalWrite(LED, LOW);
  digitalWrite(relay, LOW);

  }

}