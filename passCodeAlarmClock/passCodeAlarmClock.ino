#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int hours = 0;
int minutes = 0;
int minuteVal, hourVal, alarmSetVal;
const int minuteButton = 12;
const int hourButton = 11;
const int alarmSetButton = 10;

void setup() {
  Serial.begin(9600);
  pinMode(minuteButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(alarmSetButton, INPUT);
}

void loop() {
  minuteVal = digitalRead(minuteButton);
  hourVal = digitalRead(hourButton);
  alarmSetVal = digitalRead(alarmSetButton);
  if (minuteVal == HIGH) {
    Serial.println("Minute pressed");
  }
  if (hourVal == HIGH) {
    Serial.println("Hour pressed");
  }
  if (alarmSetVal == HIGH) {
    Serial.println("Alarm set pressed");
  }
}
