#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>

// =====================================================
// OLED DISPLAY
// 0.96" SSD1306
// 128 x 64 I2C
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET);

uint8_t oledAddress = 0x3C;

// =====================================================
// RTC DS3231
// =====================================================

RTC_DS3231 rtc;

// =====================================================
// DFPLAYER MINI
//
// Arduino Nano:
// D10 = RX  <- DFPlayer TX
// D11 = TX  -> DFPlayer RX
//
// Use about 1K resistor between
// Nano D11 and DFPlayer RX
// =====================================================

SoftwareSerial dfSerial(10, 11);
DFRobotDFPlayerMini player;

bool playerReady = false;

// =====================================================
// BUTTON PINS
// =====================================================

#define SET_BUTTON 2
#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define STOP_BUTTON 5

// =====================================================
// BUZZER
// =====================================================

#define BUZZER_PIN 6

// =====================================================
// ALARM SETTINGS
// =====================================================

int alarmHour = 7;
int alarmMinute = 0;

bool alarmEnabled = true;
bool alarmRunning = false;
bool alarmTriggered = false;

unsigned long alarmStartTime = 0;

#define ALARM_DURATION 30000UL

// =====================================================
// EEPROM ADDRESSES
// =====================================================

#define EEPROM_HOUR 0
#define EEPROM_MINUTE 1
#define EEPROM_ENABLED 2

// =====================================================
// MENU
//
// 0 = Normal clock
// 1 = Clock hour
// 2 = Clock minute
// 3 = Alarm hour
// 4 = Alarm minute
// 5 = Alarm ON/OFF
// =====================================================

byte menuMode = 0;

// =====================================================
// HALF-HOUR ANNOUNCEMENT
// =====================================================

int lastAnnouncementKey = -1;

// =====================================================
// BUTTON DEBOUNCE
// =====================================================

// SET button
bool setStableState = HIGH;
bool setLastReading = HIGH;
unsigned long setDebounceTime = 0;

// STOP button
bool stopStableState = HIGH;
bool stopLastReading = HIGH;
unsigned long stopDebounceTime = 0;

// UP button
bool upStableState = HIGH;
bool upLastReading = HIGH;
unsigned long upDebounceTime = 0;

// DOWN button
bool downStableState = HIGH;
bool downLastReading = HIGH;
unsigned long downDebounceTime = 0;

const unsigned long debounceDelay = 40;

// =====================================================
// HOLD / AUTO REPEAT
// =====================================================

bool upHolding = false;
bool downHolding = false;

unsigned long upHoldStart = 0;
unsigned long downHoldStart = 0;

unsigned long lastUpRepeat = 0;
unsigned long lastDownRepeat = 0;

// Wait this long before automatic repeat
const unsigned long holdDelay = 500;

// Repeat every 150 ms after holding
const unsigned long repeatDelay = 150;

// =====================================================
// CHECK I2C DEVICE
// =====================================================

bool i2cDeviceFound(uint8_t address) {

  Wire.beginTransmission(address);

  return (Wire.endTransmission() == 0);
}

// =====================================================
// PRINT TWO DIGITS
// =====================================================

void print2Digits(int value) {

  if (value < 10) {
    display.print('0');
  }

  display.print(value);
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  // ===================================================
  // BUTTONS
  // ===================================================

  pinMode(SET_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);

  // ===================================================
  // BUZZER
  // ===================================================

  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  // ===================================================
  // I2C
  // Nano:
  // SDA = A4
  // SCL = A5
  // ===================================================

  Wire.begin();

  delay(100);

  // ===================================================
  // FIND OLED ADDRESS
  // ===================================================

  if (i2cDeviceFound(0x3C)) {

    oledAddress = 0x3C;
  } else if (i2cDeviceFound(0x3D)) {

    oledAddress = 0x3D;
  } else {

    Serial.println(F("OLED NOT FOUND"));

    while (1)
      ;
  }

  // ===================================================
  // START OLED
  // ===================================================

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        oledAddress)) {

    Serial.println(F("OLED ERROR"));

    while (1)
      ;
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextWrap(false);

  // ===================================================
  // STARTUP SCREEN
  // ===================================================

  display.setTextSize(2);

  display.setCursor(34, 10);
  display.print(F("SMART"));

  display.setCursor(34, 35);
  display.print(F("CLOCK"));

  display.display();

  delay(1500);

  // ===================================================
  // START RTC
  // ===================================================

  if (!rtc.begin()) {

    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(20, 20);
    display.print(F("RTC NOT FOUND"));

    display.setCursor(10, 35);
    display.print(F("CHECK DS3231"));

    display.display();

    while (1)
      ;
  }

  // ===================================================
  // RTC LOST POWER
  // ===================================================

  if (rtc.lostPower()) {

    rtc.adjust(
      DateTime(
        F(__DATE__),
        F(__TIME__)));
  }

  // ===================================================
  // DFPLAYER
  // ===================================================

  dfSerial.begin(9600);

  delay(500);

  if (player.begin(dfSerial)) {

    playerReady = true;

    player.volume(25);

    player.EQ(DFPLAYER_EQ_NORMAL);

    Serial.println(F("DFPLAYER READY"));
  } else {

    playerReady = false;

    Serial.println(F("DFPLAYER NOT FOUND"));

    display.clearDisplay();

    display.setTextSize(1);

    display.setCursor(13, 20);
    display.print(F("DFPLAYER ERROR"));

    display.setCursor(8, 35);
    display.print(F("CLOCK STILL WORKS"));

    display.display();

    delay(1500);
  }

  // ===================================================
  // LOAD ALARM
  // ===================================================

  loadAlarmSettings();

  display.clearDisplay();
  display.display();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  DateTime now = rtc.now();

  handleButtons();

  if (menuMode == 0) {

    showNormalScreen(now);

    checkAlarm(now);

    // Half-hour voice announcement
    // only when alarm is not ringing

    if (!alarmRunning) {

      checkHalfHourAnnouncement(now);
    }

    // Automatic alarm stop after 30 seconds

    if (alarmRunning) {

      if (millis() - alarmStartTime >= ALARM_DURATION) {

        stopAlarm();
      }
    }
  }

  else {

    showSettingsScreen(rtc.now());
  }

  delay(20);
}

// =====================================================
// NORMAL SCREEN
// =====================================================

void showNormalScreen(DateTime now) {

  display.clearDisplay();

  // ===================================================
  // 12-HOUR CONVERSION
  // ===================================================

  int hour12 = now.hour();

  bool isPM = false;

  if (hour12 >= 12) {

    isPM = true;
  }

  if (hour12 == 0) {

    hour12 = 12;
  } else if (hour12 > 12) {

    hour12 -= 12;
  }

  // ===================================================
  // TIME
  // ===================================================

  display.setTextSize(2);

  display.setCursor(0, 0);

  print2Digits(hour12);

  display.print(':');

  print2Digits(now.minute());

  display.print(':');

  print2Digits(now.second());

  // AM / PM

  display.setTextSize(1);

  display.setCursor(108, 4);

  if (isPM) {

    display.print(F("PM"));
  } else {

    display.print(F("AM"));
  }

  // ===================================================
  // LINE
  // ===================================================

  display.drawLine(
    0,
    18,
    127,
    18,
    SSD1306_WHITE);

  // ===================================================
  // DATE
  // ===================================================

  display.setCursor(1, 22);

  print2Digits(now.day());

  display.print('/');

  print2Digits(now.month());

  display.print('/');

  display.print(now.year());

  // ===================================================
  // ALARM
  // ===================================================

  display.setCursor(1, 35);

  display.print(F("ALARM "));

  int alarmDisplayHour = alarmHour;

  bool alarmPM = false;

  if (alarmDisplayHour >= 12) {

    alarmPM = true;
  }

  if (alarmDisplayHour == 0) {

    alarmDisplayHour = 12;
  } else if (alarmDisplayHour > 12) {

    alarmDisplayHour -= 12;
  }

  print2Digits(alarmDisplayHour);

  display.print(':');

  print2Digits(alarmMinute);

  display.print(' ');

  if (alarmPM) {

    display.print(F("PM"));
  } else {

    display.print(F("AM"));
  }

  // ===================================================
  // ALARM STATUS
  // ===================================================

  display.setCursor(1, 48);

  if (alarmEnabled) {

    display.print(F("Alarm:ON"));
  } else {

    display.print(F("Alarm:OFF"));
  }

  if (alarmRunning) {

    display.setCursor(78, 48);

    display.print(F("RING!"));
  }

  // ===================================================
  // BUTTON HELP
  // ===================================================

  display.setCursor(1, 57);

  display.print(F("SET:Menu  STOP:OK"));

  display.display();
}

// =====================================================
// BUTTON HANDLING
// =====================================================

void handleButtons() {

  unsigned long currentMillis = millis();

  // ===================================================
  // SET BUTTON
  // ===================================================

  bool setReading = digitalRead(SET_BUTTON);

  if (setReading != setLastReading) {

    setDebounceTime = currentMillis;

    setLastReading = setReading;
  }

  if ((currentMillis - setDebounceTime) > debounceDelay) {

    if (setReading != setStableState) {

      setStableState = setReading;

      // Button pressed

      if (setStableState == LOW) {

        if (!alarmRunning) {

          menuMode++;

          if (menuMode > 5) {

            menuMode = 0;

            saveAlarmSettings();
          }
        }
      }
    }
  }

  // ===================================================
  // STOP BUTTON
  // ===================================================

  bool stopReading = digitalRead(STOP_BUTTON);

  if (stopReading != stopLastReading) {

    stopDebounceTime = currentMillis;

    stopLastReading = stopReading;
  }

  if ((currentMillis - stopDebounceTime) > debounceDelay) {

    if (stopReading != stopStableState) {

      stopStableState = stopReading;

      if (stopStableState == LOW) {

        // Alarm currently ringing

        if (alarmRunning) {

          stopAlarm();

          return;
        }

        // Exit menu

        if (menuMode != 0) {

          menuMode = 0;

          saveAlarmSettings();

          return;
        }
      }
    }
  }

  // ===================================================
  // UP BUTTON
  // ===================================================

  bool upReading = digitalRead(UP_BUTTON);

  if (upReading != upLastReading) {

    upDebounceTime = currentMillis;

    upLastReading = upReading;
  }

  if ((currentMillis - upDebounceTime) > debounceDelay) {

    if (upReading != upStableState) {

      upStableState = upReading;

      // First press

      if (upStableState == LOW) {

        if (menuMode != 0) {

          increaseValue(rtc.now());

          upHolding = true;

          upHoldStart = currentMillis;

          lastUpRepeat = currentMillis;
        }
      }

      // Released

      else {

        upHolding = false;
      }
    }
  }

  // ===================================================
  // UP HOLD AUTO REPEAT
  // ===================================================

  if (upHolding && upStableState == LOW && menuMode != 0) {

    if (currentMillis - upHoldStart >= holdDelay) {

      if (currentMillis - lastUpRepeat >= repeatDelay) {

        lastUpRepeat = currentMillis;

        increaseValue(rtc.now());
      }
    }
  }

  // ===================================================
  // DOWN BUTTON
  // ===================================================

  bool downReading = digitalRead(DOWN_BUTTON);

  if (downReading != downLastReading) {

    downDebounceTime = currentMillis;

    downLastReading = downReading;
  }

  if ((currentMillis - downDebounceTime) > debounceDelay) {

    if (downReading != downStableState) {

      downStableState = downReading;

      // First press

      if (downStableState == LOW) {

        if (menuMode != 0) {

          decreaseValue(rtc.now());

          downHolding = true;

          downHoldStart = currentMillis;

          lastDownRepeat = currentMillis;
        }
      }

      // Released

      else {

        downHolding = false;
      }
    }
  }

  // ===================================================
  // DOWN HOLD AUTO REPEAT
  // ===================================================

  if (downHolding && downStableState == LOW && menuMode != 0) {

    if (currentMillis - downHoldStart >= holdDelay) {

      if (currentMillis - lastDownRepeat >= repeatDelay) {

        lastDownRepeat = currentMillis;

        decreaseValue(rtc.now());
      }
    }
  }
}

// =====================================================
// INCREASE VALUE
// =====================================================

void increaseValue(DateTime now) {

  // ===================================================
  // CLOCK HOUR
  // ===================================================

  if (menuMode == 1) {

    int newHour = now.hour() + 1;

    if (newHour >= 24) {

      newHour = 0;
    }

    rtc.adjust(
      DateTime(
        now.year(),
        now.month(),
        now.day(),
        newHour,
        now.minute(),
        now.second()));
  }

  // ===================================================
  // CLOCK MINUTE
  // ===================================================

  else if (menuMode == 2) {

    int newMinute = now.minute() + 1;

    if (newMinute >= 60) {

      newMinute = 0;
    }

    rtc.adjust(
      DateTime(
        now.year(),
        now.month(),
        now.day(),
        now.hour(),
        newMinute,
        0));
  }

  // ===================================================
  // ALARM HOUR
  // ===================================================

  else if (menuMode == 3) {

    alarmHour++;

    if (alarmHour >= 24) {

      alarmHour = 0;
    }
  }

  // ===================================================
  // ALARM MINUTE
  // ===================================================

  else if (menuMode == 4) {

    alarmMinute++;

    if (alarmMinute >= 60) {

      alarmMinute = 0;
    }
  }

  // ===================================================
  // ALARM STATUS
  // ===================================================

  else if (menuMode == 5) {

    alarmEnabled = !alarmEnabled;

    // Prevent rapid ON/OFF when holding button

    upHolding = false;
  }
}

// =====================================================
// DECREASE VALUE
// =====================================================

void decreaseValue(DateTime now) {

  // ===================================================
  // CLOCK HOUR
  // ===================================================

  if (menuMode == 1) {

    int newHour = now.hour() - 1;

    if (newHour < 0) {

      newHour = 23;
    }

    rtc.adjust(
      DateTime(
        now.year(),
        now.month(),
        now.day(),
        newHour,
        now.minute(),
        now.second()));
  }

  // ===================================================
  // CLOCK MINUTE
  // ===================================================

  else if (menuMode == 2) {

    int newMinute = now.minute() - 1;

    if (newMinute < 0) {

      newMinute = 59;
    }

    rtc.adjust(
      DateTime(
        now.year(),
        now.month(),
        now.day(),
        now.hour(),
        newMinute,
        0));
  }

  // ===================================================
  // ALARM HOUR
  // ===================================================

  else if (menuMode == 3) {

    alarmHour--;

    if (alarmHour < 0) {

      alarmHour = 23;
    }
  }

  // ===================================================
  // ALARM MINUTE
  // ===================================================

  else if (menuMode == 4) {

    alarmMinute--;

    if (alarmMinute < 0) {

      alarmMinute = 59;
    }
  }

  // ===================================================
  // ALARM STATUS
  // ===================================================

  else if (menuMode == 5) {

    alarmEnabled = !alarmEnabled;

    downHolding = false;
  }
}

// =====================================================
// SETTINGS SCREEN
// =====================================================

void showSettingsScreen(DateTime now) {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(8, 0);

  display.print(F("SMART CLOCK SET"));

  display.drawLine(
    0,
    10,
    127,
    10,
    SSD1306_WHITE);

  // ===================================================
  // CLOCK HOUR
  // ===================================================

  if (menuMode == 1) {

    display.setCursor(30, 15);

    display.print(F("CLOCK HOUR"));

    display.setTextSize(3);

    display.setCursor(45, 28);

    print2Digits(now.hour());
  }

  // ===================================================
  // CLOCK MINUTE
  // ===================================================

  else if (menuMode == 2) {

    display.setCursor(24, 15);

    display.print(F("CLOCK MINUTE"));

    display.setTextSize(3);

    display.setCursor(45, 28);

    print2Digits(now.minute());
  }

  // ===================================================
  // ALARM HOUR
  // ===================================================

  else if (menuMode == 3) {

    display.setCursor(30, 15);

    display.print(F("ALARM HOUR"));

    display.setTextSize(3);

    display.setCursor(45, 28);

    print2Digits(alarmHour);
  }

  // ===================================================
  // ALARM MINUTE
  // ===================================================

  else if (menuMode == 4) {

    display.setCursor(24, 15);

    display.print(F("ALARM MINUTE"));

    display.setTextSize(3);

    display.setCursor(45, 28);

    print2Digits(alarmMinute);
  }

  // ===================================================
  // ALARM STATUS
  // ===================================================

  else if (menuMode == 5) {

    display.setCursor(28, 15);

    display.print(F("ALARM STATUS"));

    display.setTextSize(2);

    if (alarmEnabled) {

      display.setCursor(50, 31);

      display.print(F("ON"));
    } else {

      display.setCursor(44, 31);

      display.print(F("OFF"));
    }
  }

  // ===================================================
  // HELP
  // ===================================================

  display.setTextSize(1);

  display.setCursor(0, 56);

  display.print(F("UP/DN SET  STOP OK"));

  display.display();
}

// =====================================================
// HALF-HOUR ANNOUNCEMENT
// =====================================================

void checkHalfHourAnnouncement(DateTime now) {

  if (!playerReady) {

    return;
  }

  // Only XX:00 and XX:30

  if (now.minute() != 0 && now.minute() != 30) {

    return;
  }

  // Unique number for current hour/minute

  int currentKey =
    now.hour() * 60 + now.minute();

  // Prevent repeated announcement

  if (currentKey == lastAnnouncementKey) {

    return;
  }

  lastAnnouncementKey = currentKey;

  announceTime(now);
}

// =====================================================
// VOICE TIME ANNOUNCEMENT
// =====================================================

void announceTime(DateTime now) {

  if (!playerReady) {

    return;
  }

  int hour12 = now.hour();

  if (hour12 == 0) {

    hour12 = 12;
  }

  else if (hour12 > 12) {

    hour12 -= 12;
  }

  // 0001.mp3 = "It is"

  playFile(1);

  delay(700);

  // 0002 = One
  // 0003 = Two
  // ...
  // 0013 = Twelve

  playFile(hour12 + 1);

  delay(700);

  // ===================================================
  // FULL HOUR
  // ===================================================

  if (now.minute() == 0) {

    // 0014.mp3 = "O'clock"

    playFile(14);
  }

  // ===================================================
  // HALF HOUR
  // ===================================================

  else if (now.minute() == 30) {

    // 0015.mp3 = "And"

    playFile(15);

    delay(500);

    // 0016.mp3 = "Thirty minutes"

    playFile(16);
  }
}

// =====================================================
// CHECK ALARM
// =====================================================

void checkAlarm(DateTime now) {

  if (!alarmEnabled) {

    return;
  }

  // Alarm time reached

  if (now.hour() == alarmHour && now.minute() == alarmMinute) {

    if (!alarmTriggered) {

      alarmTriggered = true;

      // Prevent simultaneous half-hour message

      lastAnnouncementKey =
        now.hour() * 60 + now.minute();

      startAlarm(now);
    }
  }

  // Alarm minute passed

  else {

    if (!alarmRunning) {

      alarmTriggered = false;
    }
  }
}

// =====================================================
// START ALARM
// =====================================================

void startAlarm(DateTime now) {

  alarmRunning = true;

  alarmStartTime = millis();

  // ===================================================
  // BUZZER ON
  // ===================================================

  digitalWrite(
    BUZZER_PIN,
    HIGH);

  // ===================================================
  // DFPLAYER ALARM
  // ===================================================

  if (playerReady) {

    // 0017.mp3 = Alarm

    playFile(17);

    delay(1000);

    // Voice time only for :00 / :30

    if (now.minute() == 0 || now.minute() == 30) {

      announceTime(now);
    }
  }
}

// =====================================================
// STOP ALARM
// =====================================================

void stopAlarm() {

  alarmRunning = false;

  digitalWrite(
    BUZZER_PIN,
    LOW);

  if (playerReady) {

    player.stop();
  }
}

// =====================================================
// PLAY MP3
// =====================================================

void playFile(int fileNumber) {

  if (!playerReady) {

    return;
  }

  player.playMp3Folder(
    fileNumber);

  delay(100);
}

// =====================================================
// SAVE ALARM SETTINGS
// =====================================================

void saveAlarmSettings() {

  EEPROM.update(
    EEPROM_HOUR,
    alarmHour);

  EEPROM.update(
    EEPROM_MINUTE,
    alarmMinute);

  EEPROM.update(
    EEPROM_ENABLED,
    alarmEnabled ? 1 : 0);
}

// =====================================================
// LOAD ALARM SETTINGS
// =====================================================

void loadAlarmSettings() {

  byte savedHour =
    EEPROM.read(EEPROM_HOUR);

  byte savedMinute =
    EEPROM.read(EEPROM_MINUTE);

  byte savedEnabled =
    EEPROM.read(EEPROM_ENABLED);

  // ===================================================
  // HOUR
  // ===================================================

  if (savedHour <= 23) {

    alarmHour = savedHour;
  } else {

    alarmHour = 7;
  }

  // ===================================================
  // MINUTE
  // ===================================================

  if (savedMinute <= 59) {

    alarmMinute = savedMinute;
  } else {

    alarmMinute = 0;
  }

  // ===================================================
  // STATUS
  // ===================================================

  if (savedEnabled == 0) {

    alarmEnabled = false;
  }

  else if (savedEnabled == 1) {

    alarmEnabled = true;
  }

  else {

    alarmEnabled = true;
  }
}