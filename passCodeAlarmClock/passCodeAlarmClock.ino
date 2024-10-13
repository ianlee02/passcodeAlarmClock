#include <Key.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <RTClib.h>
#include <string.h>

const byte numRows = 4;
const byte numCols = 4;
char keys[numRows][numCols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
byte rowPin[numRows] = {9, 8, 7, 6};
byte colPin[numCols] = {5, 4, 3, 2};
Keypad passcodekeypad = Keypad(makeKeymap(keys), rowPin, colPin, numRows, numCols);
int buzzer = 13;

int helperVal = 0;
int hours = 0;
int minutes = 0;
int alarmSetMode, passcodeSetMode;
bool alarmArmed;
bool passCodeIsRight = true;
bool passCodeEntered = true;
int minuteVal, hourVal, alarmSetVal;
int alarmMinVal, alarmHrVal;
int timeInMin, timeInHr, timeInSec;
const int minuteButton = 12;
const int hourButton = 11;
const int alarmSetButton = 10;
unsigned long timeToPass;
String passcodeToDisable;
String verifyCode;
int verifyLetter;
String hashBeforeCode, promptedCode;
int verifyMode = 1;
LiquidCrystal_I2C lcd(0x27, 20, 4); //
RTC_DS3231 timerKeeper;
DateTime currentTime;

unsigned long timeForCycle, beginTime;

void savePasscode(int startingAddress, String &passcodeToSave) {
  byte lengthOfCode = passcodeToSave.length(); // Gets the length of the code to know how long it is for when we read it after power loss.
  EEPROM.write(startingAddress, lengthOfCode); // Saves the

  for (int i = 0; i < lengthOfCode; i++) {
    EEPROM.write(startingAddress + i + 1, passcodeToSave[i]);
  }
}

String readSavedPasscode(int savedAddress) {
  int lenOfCode = EEPROM.read(savedAddress);
  char saveCode[lenOfCode + 1];

  for (int j = 0; j < lenOfCode; j++) {
    saveCode[j] = EEPROM.read(savedAddress + j + 1);
  }
  saveCode[lenOfCode] = '\0';
  return String(saveCode);

}

void setup() {
  Serial.begin(9600);
  pinMode(minuteButton, INPUT);
  pinMode(hourButton, INPUT);
  pinMode(alarmSetButton, INPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin();
  alarmHrVal = EEPROM.read(1000);
  alarmMinVal = EEPROM.read(1001);
  Serial.println("Initialize DS3231");
  timerKeeper.begin();
  if (timerKeeper.lostPower()) {
    timerKeeper.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void displayWarningAndSoundAlarm() {
  lcd.setCursor(0, 4);
  lcd.print("ENTER PASSCODE");
}

void soundAlarmAndDisplayCodeRequest() {
  tone(buzzer, 1000);
  delay(70);
  noTone(buzzer);
  delay(70);
  tone(buzzer, 1500);
}

void noSoundAlarm() {
  noTone(buzzer);
}

void setAlarmTime(int minSet, int hrSet) {
  noSoundAlarm();
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
  if (alarmMinVal >= 60) {
    alarmHrVal++;
    alarmMinVal = 0;
  }
  if (alarmHrVal >= 24) {
    alarmHrVal = 0;
  }
  if (hrSet == 1) {
    alarmHrVal++;
    lcd.clear();
    timeInHr = timeInHr;
  }
  lcd.setCursor(6, 2);
  lcd.print("SET ALARM MODE");
  EEPROM.write(1000, alarmHrVal);
  EEPROM.write(1001, alarmMinVal);
}

void setPasscode(char enteredKey) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set code: ");
  if (enteredKey != '#') {
    passcodeToDisable += enteredKey;
  }
  if (enteredKey == '*') {
    passcodeToDisable = "";
  }
  savePasscode(0, passcodeToDisable);
}

void loop() {
  currentTime = timerKeeper.now();
  lcd.clear();
  beginTime = millis();
  passcodeToDisable = readSavedPasscode(0);
  minuteVal = digitalRead(minuteButton);
  hourVal = digitalRead(hourButton);
  alarmSetVal = digitalRead(alarmSetButton);
  char firstKey = passcodekeypad.getKey();
  if (firstKey == '#') {
    passcodeSetMode = 1 - passcodeSetMode;
  }
  if (passcodeSetMode == 1) {
    setPasscode(firstKey);
  } else {
    if (alarmSetVal == HIGH || firstKey == 'A') {
      alarmSetMode = 1 - alarmSetMode;
    }
    if (alarmSetMode == 1) {
      timeInHr = timeInHr;
      timeInMin = timeInMin;
      setAlarmTime(minuteVal, hourVal);
      if (alarmSetVal == HIGH || firstKey == 'A') {
        if (!alarmArmed) {
          alarmArmed = true;
        } else {
          alarmArmed = false;
        }
      }
    } else {
      lcd.setCursor(6, 1);
      if (currentTime.hour() < 10) {
        lcd.print("0");
      }
      lcd.print(currentTime.hour());
      lcd.print(":");
      if (currentTime.minute() < 10) {
        lcd.print("0");
      }
      lcd.print(currentTime.minute());
      lcd.print(":");
      if (currentTime.second() < 10) {
        lcd.print("0");
      }
      lcd.print(currentTime.second());
      helperVal++;
      if (helperVal % 5 == 0) {
        timeInSec++;

        Serial.print("Alarm time: ");
        Serial.print(alarmHrVal);
        Serial.print(":");
        Serial.println(alarmMinVal);
        Serial.print("Real time: ");
        Serial.print(currentTime.hour());
        Serial.print(":");
        Serial.println(currentTime.minute());
        Serial.print("Alarm Armed: ");
        Serial.println(alarmArmed);
        Serial.print("Passcode: ");
        Serial.println(passcodeToDisable);

      }
      if (alarmArmed) {
        lcd.setCursor(0, 0);
        lcd.print("A");
        if (currentTime.minute() == alarmMinVal && currentTime.hour() == alarmHrVal && currentTime.second() == 0) {
          passCodeIsRight = false;
          passCodeEntered = false;
        }
        if (timeInMin == alarmMinVal + 1) {
          passCodeEntered = true;
        }
      }
      if (!passCodeIsRight || !passCodeEntered) {
        soundAlarmAndDisplayCodeRequest();
        Serial.print("Passcode entered length: ");
        Serial.println(verifyLetter);
        Serial.print("PassCode is right length: ");
        Serial.println(passcodeToDisable.length());
        Serial.println("PASSCODE INCORRECT!");
        if (firstKey == passcodeToDisable[verifyLetter]) {
          verifyLetter++;
        }
        lcd.setCursor(0, 3);
        lcd.print("Enter Passcode");
        if (verifyLetter == passcodeToDisable.length()) {
          verifyLetter = 0;
          passCodeIsRight = true;
          passCodeEntered = true;
          Serial.println("PASSCODE IS CORRECT!!");
        }
      } else {
        noSoundAlarm();
      }
    }
  }

  delay(100);
  timeForCycle = millis() - beginTime;
  Serial.print("Time taken: ");
  Serial.print(timeForCycle);
  Serial.println(" ms");
}
