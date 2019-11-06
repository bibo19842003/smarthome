#include <SimpleDHT.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "pitches.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


// pin 使用说明：
// D0(16): led buzzer 信号
// D1(5): SCL
// D2(4): SDA
// D3(0): dht11 信号
// D4(2): button 信号



U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
// https://blog.bbskali.cn/index.php/archives/1337/

// https://blog.csdn.net/dpjcn1990/article/details/92831760


// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 16;     // LED引脚
int button = 2; // 按键连接的引脚
int temp = 0;    // 用于读取按键状态的临时变量

int pinDHT11 = 0;
SimpleDHT11 dht11(pinDHT11);


void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print(); // enable UTF8 support for the print() function
  Serial.begin(9600);
  Serial.println("=================================");
//  Serial.println("Sample DHT11...");
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(button, INPUT); // 按键设置为输出
}


void loop() {
  
  // 获取温度 湿度
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
 
//  Serial.print((int)temperature); Serial.print(" °C, "); 
//  Serial.print((int)humidity); Serial.println(" H");

// 光强取值
//  int lightsensorValue = analogRead(A0); // 读取模拟引脚A0上的输入
//  float voltage = lightsensorValue * (5.0 / 1023.0); // 将模拟读数（从0 - 1023变为0）转换为电压（0 - 5V）
//  Serial.println(lightsensorValue); // 打印出模拟口读取的值

  u8g2.clearBuffer();         // 清除内部缓冲区
  
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // choose a suitable font u8g2_font_wqy12_t_gb2312 u8g2_font_ncenB08_tr

// 显示温度 湿度
  u8g2.setCursor(0,20); // display temerature
  u8g2.print("温度: ");
  u8g2.print((int)temperature);
  u8g2.print("度");
  
  u8g2.setCursor(0,40); // display humidity
  u8g2.print("湿度: ");
  u8g2.print((int)humidity);
  u8g2.print("%");

// 光强显示
//  u8g2.setCursor(0,60); // 光强
//  u8g2.print("光强: ");
//  u8g2.print(lightsensorValue);
//  u8g2.print(" xxx");

  u8g2.sendBuffer();          // transfer internal memory to the display

  temp = digitalRead(button);
  if (temp == LOW) {
    digitalWrite(led, HIGH);
    Serial.println("LED Turned ON, buzzer on");

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 8; thisNote++) {

      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(led, melody[thisNote], noteDuration);

      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
    }

  }
  else {
    digitalWrite(led, LOW);
    Serial.println("LED Turned OFF");
    delay(1000);
  }

//  delay(2000);

}
