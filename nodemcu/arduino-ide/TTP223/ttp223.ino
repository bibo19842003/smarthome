// 触碰开关

int ttppin = D8;

void setup() {
  
  Serial.begin(115200);
  pinMode(ttppin,INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(D8)){
    Serial.println("111111");
  }
  else {
    Serial.println("000000");
  }

  delay(1000);
}
