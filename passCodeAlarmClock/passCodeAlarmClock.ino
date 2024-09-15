#include <Key.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <Keypad.h>
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
String hashBeforeCode, promptedCode;
int verifyMode = 1;
LiquidCrystal_I2C lcd(0x27, 20, 4); //

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
  lcd.begin();
  alarmHrVal = EEPROM.read(1000);
  alarmMinVal = EEPROM.read(1001);
}

void displayWarningAndSoundAlarm() {
  lcd.setCursor(0, 4);
  lcd.print("ENTER PASSCODE");
}

void soundAlarmAndDisplayCodeRequest(char codeEntered) {
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
  lcd.clear();
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
    if (alarmSetVal == HIGH) {
      alarmSetMode = 1 - alarmSetMode;
    }
    if (alarmSetMode == 1) {
      timeInHr = timeInHr;
      timeInMin = timeInMin;
      setAlarmTime(minuteVal, hourVal);
      if (alarmSetVal == HIGH) {
        if (!alarmArmed) {
          alarmArmed = true;
        } else {
          alarmArmed = false;
        }
      }
    } else {
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
        /*
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
          Serial.print("Passcode: ");
          Serial.println(passcodeToDisable);
        */
      }
      if (alarmArmed) {
        lcd.setCursor(0, 0);
        lcd.print("A");
        if (timeInMin == alarmMinVal && timeInHr == alarmHrVal) {
          passCodeIsRight = false;
          passCodeEntered = false;
        }
        if (timeInMin == alarmMinVal + 1) {
          passCodeEntered = true;
        }
      }
      if (!passCodeIsRight || !passCodeEntered) {
        Serial.print("Passcode entered: ");
        Serial.println(passCodeEntered);
        Serial.print("PassCode is right: ");
        Serial.println(passCodeIsRight);
        promptedCode += firstKey;
        if (firstKey == '*') {
          promptedCode = "";
          passCodeIsRight = true;
          passCodeEntered = true;
        }
        Serial.print("Set code: ");
        Serial.println(passcodeToDisable);
        Serial.print("Entered code: ");
        Serial.println(promptedCode);
        lcd.setCursor(0, 3);
        lcd.print("Enter Passcode");
        if (passcodeToDisable == promptedCode) {
          promptedCode = "";
          passCodeIsRight = true;
          passCodeEntered = true;
        }
      } else {
        lcd.setCursor(0, 3);
        lcd.print("");
      }
    }
  }

  delay(200);
}
