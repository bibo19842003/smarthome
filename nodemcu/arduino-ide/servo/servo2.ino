#include
Servo myservo; // 创建一个 servo object
int potpin = 0; // analog pin 用来连接电位计
int val; // val存储analog pin的值

void setup() {
myservo.attach(9); // 连接舵机控制信号（黄或白）至 pin 9
}

void loop() {
val = analogRead(potpin);
// 读出可变电位计的值 (范围 0 - 1023)
val = map(val, 0, 1023, 0, 180);
// 按舵机角度进行设置 (范围 0 - 180)
myservo.write(val); //根据val值设置舵机位置
delay(15);
}
