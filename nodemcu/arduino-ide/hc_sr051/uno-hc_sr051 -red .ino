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


int PIR_sensor = 4;    //指定PIR模拟端口 A5
int LED = 2;           //指定LED端口 13
int val = 0;            //存储获取到的PIR数值
int i;
 
void setup()
{
  pinMode(PIR_sensor, INPUT);   //设置PIR模拟端口为输入模式
  pinMode(LED, OUTPUT);         //设置端口D0为输出模式
  Serial.begin(115200);          //设置串口波特率为115200
}
 
void loop()  {
  digitalWrite(LED,val); 
  
  if(digitalRead(PIR_sensor) == HIGH){
    Serial.println("turn on");
    val = 1;
  }   
  else {
    for(i=1;i<13;i++){
      if(digitalRead(PIR_sensor) == LOW){
        Serial.println("Nobody");
        Serial.println(i);
        delay(1000);
      }
      else {
        Serial.println("Someone here again!!!");
        Serial.println(i);
        break;      
      }
    }
    if (i == 13){
      val =0; 
      Serial.println("turn off");
      }
    else {
      val =1;
      Serial.println("turn on again and again!!!");
    }
  }
  
  delay(1000);
}
