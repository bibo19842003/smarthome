#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// nRF24L01
RF24 radio(9, 10); // CSN, CE
const byte address[6] = "00001";

// PCA9685 servo
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define SERVOMIDDLE  300 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// zy 0, sj 1, ql 2, ss 3
uint16_t pulselenzy = SERVOMIDDLE;
uint16_t pulselensj = SERVOMIDDLE;
uint16_t pulselenql = SERVOMIDDLE;
uint16_t pulselenss = SERVOMIDDLE;

// our servo # counter
uint8_t servonum = 0;


// 电机控制线

//OUT1 接高电平，OUT2 接低电平，电机正转；
//OUT1 接低电平，OUT2 接高电平，电机反转；

// 芯片前 爪子后
// L298N  电机 UNO 电机代号
// OUT1   左   5    m1
// OUT2   左   4    m1
// OUT3   右   6    m2
// OUT4   右   7    m2

/////////////////////////

int m11=5;
int m12=4;
int m21=6;
int m22=7;

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

// car car car car car car car car car car car car car car car car car car car car
// car front  up
void carfront(){
  delay(100);
  mfront(m11, m12);
  mfront(m21, m22);
}



// car behind down
void carbehind(){
  delay(100);
  mbehind(m11, m12);
  mbehind(m21, m22);
}



// car front turn left , left up
void carleftfront(){
  delay(100);
  mbehind(m11, 132);
  mfront(m21, m22);

  delay(50);
  carfront();  
}



// car front turn right, right up
void carrightfront(){
  delay(100);
  mbehind(m21, m22);
  mfront(m11, m12);

  delay(50);
  carfront();
}


// car eversion turn left, left down
void carleftbehind(){
  delay(100);
  mbehind(m21, m22);
  mfront(m11, m12);

  delay(50);
  carbehind();
}



// car eversion turn right, right down
void carrightbehind(){
  delay(100);
  mbehind(m11, m12);
  mfront(m21, m22);

  delay(50);
  carbehind();
}


// car stop
void carstop(){
  mstop(m11, m12);
  mstop(m21, m22);
}


// nnnnnnnnnnnnnnnnnnnnnnn begin
// car left 
void carleft(){
  delay(100);
  mbehind(m11, m12);
  mfront(m21, m22);

  delay(50);
//  carfront();
}


// car right
void carright(){
  delay(100);
  mbehind(m21, m22);
  mfront(m11, m12);

  delay(50);
//  carfront();
}


// servo ++
uint16_t servoplus(uint16_t pulselen){
  pulselen = pulselen + 5;
  
  if ( pulselen > SERVOMAX) {
    return(SERVOMAX);
  }
  else {
    return(pulselen);
  }
}


// servo --
uint16_t servominus(uint16_t pulselen){
  pulselen = pulselen - 5;
  
  if ( pulselen > SERVOMIN) {
    return(pulselen);
  }
  else {
    return(SERVOMIN);
  }
}
// nnnnnnnnnnnnnnnnnnnnnnn end

void setup() {

//初始化各IO,模式为OUTPUT 输出模式
  pinMode(m11,OUTPUT);
  pinMode(m12,OUTPUT);
  pinMode(m21,OUTPUT);
  pinMode(m22,OUTPUT);
//默认电机停止转动，全部设为低电平
  digitalWrite(m11,LOW);
  digitalWrite(m12,LOW);
  digitalWrite(m21,LOW);
  digitalWrite(m22,LOW);

  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  for(servonum; servonum < 4;servonum++) {
    pwm.setPWM(servonum, 0, SERVOMIDDLE);
  }

  delay(10);
}

void loop() {
  
  if (radio.available()) {

    int msg=0;
    Serial.println("========================");
    radio.read(&msg, sizeof(msg));
    Serial.print(msg);
    Serial.println("\\\\\\\\\\\\\\");

//    Serial.print("   ");
    if (msg == 7) {
      Serial.println("zuozuozuo");
      carleft();
    } else if (msg == 3){
      Serial.println("youyouyou");
     carright();
    } else if (msg == 1){
      Serial.println("qianqianqian");
     carfront();
    } else if (msg == 5){
      Serial.println("houhouhou");
     carbehind();
    } else if (msg == 0){
      Serial.println("tingtingting");
     carstop();
    } else if (msg == 8){
      Serial.println("zuoqianzuoqianzuoqian");
     carleftfront();
    } else if (msg == 2){
      Serial.println("youqianyouqianyouqian");
     carrightfront();
    } else if (msg == 6){
      Serial.println("zuohouzuohouzuohou");
     carleftbehind();
    } else if (msg == 4){
      Serial.println("youhouyouhouyouhou");
     carrightbehind();
    } else if (msg == 11){
      Serial.println("diao che zuoyou zuoyou zuoyou zy +");
      pulselenzy = servoplus(pulselenzy);
      pwm.setPWM(0, 0, pulselenzy);
    } else if (msg == 12){
      Serial.println("diaoche songjin songjin songjin zhuazi sj +");
      pulselensj = servoplus(pulselensj);
      pwm.setPWM(1, 0, pulselensj);
    } else if (msg == 13){
      Serial.println("diaoche qiluo qiluo qiluo ql +");
      pulselenql = servoplus(pulselenql);
      pwm.setPWM(2, 0, pulselenql);
    } else if (msg == 14){
      Serial.println("dianche shensou shensou shensou ss +");
      pulselenss = servoplus(pulselenss);
      pwm.setPWM(3, 0, pulselenss);
    } else if (msg == 15){
      Serial.println("diao che zuoyou zuoyou zuoyou zy -");
      pulselenzy = servominus(pulselenzy);
      pwm.setPWM(0, 0, pulselenzy);
    } else if (msg == 16){
      Serial.println("diaoche songjin songjin songjin zhuazi sj -");
      pulselensj = servominus(pulselensj);
      pwm.setPWM(1, 0, pulselensj);
    } else if (msg == 17){
      Serial.println("diaoche qiluo qiluo qiluo ql -");
      pulselenql = servominus(pulselenql);
      pwm.setPWM(2, 0, pulselenql);
    } else if (msg == 18){
      Serial.println("dianche shensou shensou shensou ss -");
      pulselenss = servominus(pulselenss);
      pwm.setPWM(3, 0, pulselenss);
    } else {
      Serial.println("88888888888888888");
    }
    Serial.println(".............................");
  }

   delay(100);
}


