#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

int helperVal = 0;
int hours = 0;
int minutes = 0;
bool alarmSetMode, alarmArmed;
bool passCodeIsRight = true;
int minuteVal, hourVal, alarmSetVal;
int alarmMinVal, alarmHrVal;
int timeInMin, timeInHr, timeInSec;
const int minuteButton = 12;
const int hourButton = 11;
const int alarmSetButton = 10;
unsigned long timeToPass;
LiquidCrystal_I2C lcd(0x27, 20, 4); //

void setup() {
  Serial.begin(9600);
  pinMode(minuteButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(alarmSetButton, INPUT);
  lcd.begin();
}

void displayWarningAndSoundAlarm() {
  lcd.setCursor(0, 4);
  lcd.print("ENTER PASSCODE");
}

void soundAlarmAndDisplayCodeRequest() {
  if (!passCodeIsRight) {
    lcd.setCursor(0, 3);
    lcd.print("Enter Passcode");
  }
}

void setAlarmTime(int minSet, int hrSet) {
  lcd.clear();
  lcd.setCursor(6, 1);
  if (alarmHrVal < 10) {
    lcd.print("0");
  }
  lcd.print(alarmHrVal);
  lcd.print(":");
  if (alarmMinVal < 10) {
    lcd.print("0");
  }
  lcd.print(alarmMinVal);
  if (minSet == 1) {
    alarmMinVal++;
    lcd.clear();
    timeInMin = timeInMin;
  }
  if (alarmMinVal == 60) {
    alarmHrVal++;
    alarmMinVal = 0;
  }
  if (alarmHrVal == 24) {
    alarmHrVal = 0;
  }
  if (hrSet == 1) {
    alarmHrVal++;
    lcd.clear();
    timeInHr = timeInHr;
  }
  lcd.setCursor(6, 2);
  lcd.print("SET ALARM MODE");
}

void loop() {
  minuteVal = digitalRead(minuteButton);
  hourVal = digitalRead(hourButton);
  alarmSetVal = digitalRead(alarmSetButton);
  lcd.clear();
  if (timeInSec == 60) {
    timeInMin++;
    timeInSec = timeInSec % 60;
  }
  if (timeInMin >= 60) {
    timeInHr++;
    timeInMin = timeInMin % 60;
  }
  if (timeInHr >= 24) {
    timeInHr = 0;
  }
  lcd.setCursor(6, 1);
  if (timeInHr < 10) {
    lcd.print("0");
  }
  lcd.print(timeInHr);
  lcd.print(":");
  if (timeInMin < 10) {
    lcd.print("0");
  }
  lcd.print(timeInMin);
  lcd.print(":");
  if (timeInSec < 10) {
    lcd.print("0");
  }
  lcd.print(timeInSec);
  if (alarmSetVal == HIGH) {
    alarmSetMode = true;
    while (alarmSetMode) {
      timeInHr = timeInHr;
      timeInMin = timeInMin;
      setAlarmTime(minuteVal, hourVal);
      if (alarmSetVal == HIGH) {
        if (!alarmArmed) {
          alarmArmed = true;
        } else {
          alarmArmed = false;
        }
        break;
      }
    }
  }
  if (minuteVal == HIGH) {
    timeInMin++;
    if (helperVal % 5 == 0) {
      timeInSec--;
    }
    timeInHr = timeInHr;
    Serial.println("Minute pressed");
  }
  if (hourVal == HIGH) {
    timeInMin = timeInMin;
    if (helperVal % 5 == 0) {
      timeInSec--;
    }
    timeInHr++;
    Serial.println("Hour pressed");
  }
  helperVal++;
  if (helperVal % 5 == 0) {
    timeInSec++;
  }
  if (alarmArmed) {
    lcd.setCursor(0, 0);
    lcd.print("A");
    if (timeInMin == alarmMinVal && timeInHr == alarmHrVal) {
      passCodeIsRight = false;
    }
  }
  soundAlarmAndDisplayCodeRequest();
  delay(200);
  Serial.print("Alarm time: ");
  Serial.print(alarmHrVal);
  Serial.print(":");
  Serial.println(alarmMinVal);
  Serial.print("Real time: ");
  Serial.print(timeInHr);
  Serial.print(":");
  Serial.println(timeInMin);
  Serial.print("Alarm Armed: ");
  Serial.println(alarmArmed);
}
