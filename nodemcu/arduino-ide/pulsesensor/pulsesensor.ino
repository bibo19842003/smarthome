// https://github.com/Lisylfn/pulseNode/blob/master/pulse.c

#include <Adafruit_SSD1306.h>     // 显示屏库
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>

#define OLED_RESET 0     // rst假管脚
Adafruit_SSD1306 display(OLED_RESET);

// 设置显示屏属性
const int WIDTH = 128;
const int HEIGHT = 64;
const int LENGTH = WIDTH;

Ticker flipper;

int blinkPin = D8;               // 设置点亮led的pin 15
int fadePin = D6;                // 设置灭led的pin 12
int fadeRate = 0;

volatile int BPM;         // 心率
volatile int Signal;      // 接收信号
volatile int IBI = 600;   // 脉搏间隔
volatile boolean Pulse = false;     // 脉搏高峰为true
volatile boolean QS = false;        // 确定为一次心跳时为true

volatile int rate[10];                    // 用于保存上十次IBI
volatile unsigned long sampleCounter = 0; // 采样计数器
volatile unsigned long lastBeatTime = 0;  // 记录上次脉搏时间
volatile int P = 512;                     // 记录峰值
volatile int T = 512;                     // 记录谷值
volatile int thresh = 512;                // 阈值
volatile int amp = 100;                   // 幅值
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;


// 显示函数
int x;
int y[LENGTH];

void clearY() {
  for (int i = 0; i < LENGTH; i++) {
    y[i] = -1;
  }
}

void drawY() {
  display.drawPixel(0, y[0], WHITE);
  for (int i = 1; i < LENGTH; i++) {
    if (y[i] != -1) {
      display.drawLine(i - 1, y[i - 1], i, y[i], WHITE);
    } else {
      break;
    }
  }
}


// nodemcu上电时
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // 初始化显示屏
  delay(20);
  display.clearDisplay();

  x = 0;
  clearY();
  // 显示心跳
  pinMode(blinkPin, OUTPUT);
  pinMode(fadePin, OUTPUT);
  Serial.begin(115200);       // 波特率115200, 这样快
  interruptSetup();           //中断, 2ms一次读取
}


// nodemcu死循环
void loop() {
  y[x] = map(Signal, 0, 1023, HEIGHT - 14, 0);
  drawY();
  x++;
  if (x >= WIDTH) {
    display.clearDisplay();
    display.setTextSize(0);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(" BPM = ");
    display.print(BPM);
    display.print("  IBI = ");
    display.print(IBI);
    display.print("  ");       // 在显示屏打印BPM和IBI
    x = 0;
    clearY();
  }

  sendDataToProcessing('S', Signal);     // 发送Signal
  if (QS == true) {                      // QS为true才是一次心跳, 发送相应数据
    fadeRate = 255;
    sendDataToProcessing('B', BPM);  // 发送BPM
    sendDataToProcessing('Q', IBI);  // 发送IBI
    QS = false;                      // 重置QS,以便检测下次心跳

  }

  ledFadeToBeat();
  display.display();
  delay(10);
}


// xintiao显示
void ledFadeToBeat() {
  fadeRate -= 15;
  fadeRate = constrain(fadeRate, 0, 255);
  analogWrite(fadePin, fadeRate);
}


// 发送数据函数
void sendDataToProcessing(char symbol, int data ) {
  Serial.print(symbol);
  Serial.println(data);
}


// 中断入口,每2ms进行检测
void interruptSetup() {
  flipper.attach_ms(2, ISRTr);
}


// 中断服务程序
void ISRTr() {
  cli();                               // 关中断
  Signal = analogRead(A0);              // 读取A0管脚, 即Pulsesensor所在管脚
  sampleCounter += 2;
  int N = sampleCounter - lastBeatTime;     // 设置一个检测阈值

  if (Signal < thresh && N > (IBI / 5) * 3) { // 避免噪声影响
    if (Signal < T) {                       // 找到谷值
      T = Signal;
    }
  }

  if (Signal > thresh && Signal > P) {     // 找到峰值
    P = Signal;
  }


  if (N > 250) {                                  // 避免高频噪声
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) ) {
      Pulse = true;
      digitalWrite(blinkPin, HIGH);
      IBI = sampleCounter - lastBeatTime;         // 计算IBI
      lastBeatTime = sampleCounter;               // 更新lastNeatTime flag

      if (secondBeat) {                      // 当是secondBeat
        secondBeat = false;                  // 更新flag
        for (int i = 0; i <= 9; i++) {       // 存十个IBI
          rate[i] = IBI;
        }
      }

      if (firstBeat) {                       // 当是firstBeat
        firstBeat = false;
        secondBeat = true;                   // 更新flag
        sei();                               // 中断
        return;
      }


      // 将rate数组里面9个旧IBI数据相加
      word runningTotal = 0;

      for (int i = 0; i <= 8; i++) {
        rate[i] = rate[i + 1];
        runningTotal += rate[i];
      }
      // 将新读取的IBI存入rate[9]
      rate[9] = IBI;
      runningTotal += rate[9];
      runningTotal /= 10;                     // 连续10个IBI取平均
      BPM = 60000 / runningTotal;             // 利用IBI求BPM
      QS = true;
    }
  }

  if (Signal < thresh && Pulse == true) {  // 判断一次是否心跳结束
    digitalWrite(blinkPin, LOW);
    Pulse = false;
    amp = P - T;
    thresh = amp / 2 + T;
    P = thresh;
    T = thresh;
  }

  if (N > 2500) {                          // 当2.5s内没有心跳,重置
    thresh = 512;
    P = 512;
    T = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = false;
  }

  sei();
}


