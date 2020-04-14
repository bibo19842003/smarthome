// https://www.arduino.cn/thread-92180-1-1.html

//（A）2.4G NRF24L01使用
//模板的RF SPI接口对应
//IRQ-->D8;CE-->D9;CSN-->D10;MOSI -->D11;MISO-->D12;CLK -->D13;

//（B）Bluetooh蓝牙使用
//模板的蓝牙接口对应 RXD-->D1(TX);TXD-->D0(RX);
//注意：当蓝牙写AT命令时，把板载上APC&BT附近的拨动开关拨到“H”，表示蓝牙的KEY脚置高，或把板载上的“KEY”脚跳线到IO口，通过软件控制蓝牙的KEY。在蓝牙其他情况下，拨动开关拨到“L”。

//（C）Nokia 5110 LCD使用
//模板的Nokia 5110接口对应 SCE-->D13;RST-->D12;DC-->D11;MOSI-->D10;SCLK-->D9;

//（D）板载的按键参考代码：
//模板的按键接口对应
//A-->D2;B-->D3;C-->D4;D-->D5;E-->D6;F-->D7;X-->A0;Y-->A1; ???k=D8???
//S-->悬空（该键为摇杆的Z键，如果需要，可以跳线接入IO口）

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CSN, CE
const byte address[6] = "00001";

int x_key = A0;                                               
int y_key = A1;                                               
int x_pos;
int y_pos;
int tempa;
int tempb;
int tempc;
int tempd;


// 0 stop
// 1 qian
// 2 youqian
// 3 you
// 4 youhou
// 5 hou
// 6 zuohou
// 7 zuo
// 8 zuoqian

void setup() {
  Serial.begin(115200);
  Serial.println("=================================");
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(x_key, INPUT);
  pinMode(y_key, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
}

void loop() {
  x_pos = analogRead(x_key) ;
  y_pos = analogRead(y_key) ;
  tempa = digitalRead(2);
  tempb = digitalRead(3);
  tempc = digitalRead(4);
  tempd = digitalRead(5);

  int msg=0;
  
  if(x_pos < 410 and y_pos > 610){
    msg = 8;
  }
  else if(x_pos > 610 and y_pos > 610){
    msg = 2;
  }
  else if(x_pos < 410 and y_pos < 410){
    msg = 6;
  }
  else if(x_pos > 610 and y_pos < 410){
    msg = 4;
  }
  else if(y_pos < 410){
    msg = 5;
  }
  else if(y_pos > 610){
    msg = 1;
  }
  else if(x_pos < 410){
    msg = 7;
  }
  else if(x_pos > 610){
    msg = 3;
  }
  else if(tempa == 0){
    msg = 0;
  }


  radio.write(&msg, sizeof(msg));
//  radio.write(&x_pos, sizeof(x_pos));
//  radio.write(&y_pos, sizeof(y_pos));
//  radio.write(&tempa, sizeof(tempa));
//  radio.write(&tempb, sizeof(tempb));
//  radio.write(&tempc, sizeof(tempc));
//  radio.write(&tempd, sizeof(tempd));

//  Serial.println("");
//  Serial.println("x , y , button-a , button-b , button-c , button-d");
//  Serial.print(x_pos);
//  Serial.print(" , ");
//  Serial.print(y_pos);
//  Serial.print(" , ");
//  Serial.print(tempa);
//  Serial.print(" , ");
//  Serial.print(tempb);
//  Serial.print(" , ");
//  Serial.print(tempc);
//  Serial.print(" , ");
//  Serial.print(tempd);

  Serial.println("");
  Serial.println("x , y , button-a , button-b , button-c , button-d");
  Serial.println(msg);
  delay(100);
}
