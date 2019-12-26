/*
 *     - s +
 * 距离      响应时间
 * 
 * 距离 顺时针加大 逆时针减小
 * 响应时间：顺时针加大 逆时针减小
 * 
 * 
 * 
 * 
 */






int PIR_sensor = D8;    //指定PIR模拟端口 A5
int LED = D3;           //指定LED端口 13
int val = 0;            //存储获取到的PIR数值
 
void setup()
{
  pinMode(PIR_sensor, INPUT);   //设置PIR模拟端口为输入模式
  pinMode(LED, OUTPUT);         //设置端口D0为输出模式
  Serial.begin(115200);          //设置串口波特率为115200
}
 
void loop()  {
  if(digitalRead(PIR_sensor) == HIGH){
    Serial.println("Someone here!");
    digitalWrite(LED,HIGH); //不发声音
    delay(1000);
  }   
  else {
    Serial.println("Nobody");
    digitalWrite(LED,LOW); //不发声音
  }
  delay(1000);
}
