
// 此按钮默认为闭合状态

#include "pitches.h"

// notes in the melody:
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

int led = 5;     // LED引脚
int button = 2; // 按键连接的引脚
int temp = 0;    // 用于读取按键状态的临时变量


void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);   // LED设置为输入
  pinMode(button, INPUT); // 按键设置为输出
}


void loop() {
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
}
