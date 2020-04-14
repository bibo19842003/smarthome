#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10); // CSN, CE
const byte address[6] = "00001";

// 电机控制线

//A1 接高电平，A2接低电平，电机正转；
//A1 接低电平，A2接高电平，电机反转；

// D1D2 左前 0 1 m1
// C1C2 左后 2 3 m2
// B1B2 右前 4 5 m3
// A1A2 右后 6 7 m4

/////////////////////////

int m11=0;
int m12=1;
int m21=2;
int m22=3;
int m31=4;
int m32=5;
int m41=6;
int m42=7;


// motor motor motor motor motor motor motor motor motor
// 单轮待机
void mstop(int in1, int in2){
  digitalWrite(in1,0);
  digitalWrite(in2,0); 
}

// 单轮正转最快
void mfront(int in1, int in2){
  digitalWrite(in1,1);
  digitalWrite(in2,0);
}

// 单轮反转最快
void mbehind(int in1, int in2){
  digitalWrite(in1,0);
  digitalWrite(in2,1);
}

// 单轮正转 - 加速
// s 模拟数值 0 - 1024
void mfa(int in1, int in2){
  int s;
  for(s=0;s<1024;s=s+10){
    analogWrite(in1,s);
    digitalWrite(in2,0);
    delay(50);
  }
}

// 单轮反转 - 加速
// s 模拟数值 0 - 1024
void mra(int in1, int in2){
  int s;
  for(s=1024;s>0;s=s-10){
    analogWrite(in1,0);
    digitalWrite(in2,s);
    delay(50);
  }
}



// car car car car car car car car car car car car car car car car car car car car
// car front  up
void carfront(){
  delay(200);
  mfront(m11, m12);
  mfront(m21, m22);
  mfront(m31, m32);
  mfront(m41, m42);
}



// car behind down
void carbehind(){
  delay(200);
  mbehind(m11, m12);
  mbehind(m21, m22);
  mbehind(m31, m32);
  mbehind(m41, m42);
}



// car front turn left , left up
void carleftfront(){
  delay(200);
  mfront(m11, m12);
  mfront(m21, m22);
  mbehind(m31, m32);
  mfront(m41, m42);

  delay(150);
//  carfront();  
}



// car front turn right, right up
void carrightfront(){
  delay(200);
  mbehind(m11, m12);
  mfront(m21, m22);
  mfront(m31, m32);
  mfront(m41, m42);

  delay(150);
//  carfront();
}


// car eversion turn left, left down
void carleftbehind(){
  delay(200);
  mbehind(m11, m12);
  mbehind(m21, m22);
  mbehind(m31, m32);
  mfront(m41, m42);

  delay(100);
//  carbehind();
}



// car eversion turn right, right down
void carrightbehind(){
  delay(100);
  mbehind(m11, m12);
  mfront(m21, m22);
  mbehind(m31, m32);
  mbehind(m41, m42);

  delay(100);
//  carbehind();
}


// car stop
void carstop(){
  mstop(m11, m12);
  mstop(m21, m22);
  mstop(m31, m32);
  mstop(m41, m42);
}


// nnnnnnnnnnnnnnnnnnnnnnn begin
// car left 
void carleft(){
  delay(200);
  mbehind(m11, m12);
  mfront(m21, m22);
  mfront(m31, m32);
  mfront(m41, m42);

  delay(150);
//  carfront();
}


// car right
void carright(){
  delay(200);
  mfront(m11, m12);
  mfront(m21, m22);
  mbehind(m31, m32);
  mfront(m41, m42);

  delay(150);
//  carfront();
}

// nnnnnnnnnnnnnnnnnnnnnnn end

void setup() {

  pinMode(m11,OUTPUT);
  pinMode(m12,OUTPUT);
  pinMode(m21,OUTPUT);
  pinMode(m22,OUTPUT);
  pinMode(m31,OUTPUT);
  pinMode(m32,OUTPUT);
  pinMode(m41,OUTPUT);
  pinMode(m42,OUTPUT);
  
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

}

void loop() {
  
  if (radio.available()) {

    int msg=0;
    Serial.println("========================");
    radio.read(&msg, sizeof(msg));
    Serial.print(msg);
    Serial.println("\\\\\\\\\\\\\\");

//    Serial.print("   ");
    if (msg == 5){//
      Serial.println("houhouhou");
     carfront();
    } else if (msg == 1){//
      Serial.println("qianqianqian");
     carbehind();
    } else if (msg == 0){//
      Serial.println("tingtingting");
     carstop();
    } else if (msg == 7){//
      Serial.println("zuohouzuohouzuohou");
     carleftbehind();
    } else if (msg == 3){//
      Serial.println("youhouyouhouyouhou");
     carrightbehind();
    } else {
      Serial.println("88888888888888888");
    }

    Serial.println(".............................");

  }
   delay(100);
}

  
