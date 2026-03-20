#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== LCD (I2C) =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== RFID =====
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ===== RELAY =====
#define RELAY_PIN 8
bool RELAY_ACTIVE_LOW = true;

// ===== KEYPAD 4x4 =====
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, A0, A1};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ===== EEPROM DB =====
// Record = UID(4) + status(1) + balance(2) + pin(4) = 11 bytes
// status: 0 empty, 1 active, 2 blocked
const int MAX_CARDS = 20;
const int REC_SIZE = 11;

// EEPROM signature to know "formatted"
const int SIG_ADDR = EEPROM.length() - 2;
const byte SIG1 = 0xA5;
const byte SIG2 = 0x5A;

// ===== SETTINGS =====
const char ADMIN_PIN[] = "1234";
const int RS_PER_SEC = 10;

// ---------- Relay ----------
void pumpOn()  { digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH); }
void pumpOff() { digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW); }

// ---------- EEPROM helpers ----------
int recAddr(int index) { 
  return index * REC_SIZE; 
}

void readRecord(int index, byte uid[4], byte &status, int &balance, char pinOut[5]) {
  int a = recAddr(index);

  for (int i = 0; i < 4; i++) uid[i] = EEPROM.read(a + i);

  status = EEPROM.read(a + 4);
  if (status == 0xFF) status = 0;

  byte hi = EEPROM.read(a + 5);
  byte lo = EEPROM.read(a + 6);
  balance = ((int)hi << 8) | lo;

  for (int i = 0; i < 4; i++) {
    byte b = EEPROM.read(a + 7 + i);
    pinOut[i] = (b == 0xFF) ? '0' : (char)b;
  }
  pinOut[4] = '\0';
}

void writeRecord(int index, const byte uid[4], byte status, int balance, const char pinIn[5]) {
  int a = recAddr(index);

  for (int i = 0; i < 4; i++) EEPROM.update(a + i, uid[i]);
  EEPROM.update(a + 4, status);

  byte hi = (balance >> 8) & 0xFF;
  byte lo = balance & 0xFF;
  EEPROM.update(a + 5, hi);
  EEPROM.update(a + 6, lo);

  for (int i = 0; i < 4; i++) EEPROM.update(a + 7 + i, pinIn[i]);
}

bool uidEqual(const byte a[4], const byte b[4]) {
  for (int i = 0; i < 4; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

int findCardIndex(const byte uid[4]) {
  for (int i = 0; i < MAX_CARDS; i++) {
    byte ru[4], st;
    int bal;
    char pin[5];
    readRecord(i, ru, st, bal, pin);

    if (st != 0 && uidEqual(ru, uid)) return i;
  }
  return -1;
}

int findCardIndexByPIN(const char pinToFind[5]) {
  for (int i = 0; i < MAX_CARDS; i++) {
    byte ru[4], st;
    int bal;
    char pin[5];
    readRecord(i, ru, st, bal, pin);

    if (st != 0 && strcmp(pin, pinToFind) == 0) return i;
  }
  return -1;
}

int findEmptyIndex() {
  for (int i = 0; i < MAX_CARDS; i++) {
    byte ru[4], st;
    int bal;
    char pin[5];
    readRecord(i, ru, st, bal, pin);

    if (st == 0) return i;
  }
  return -1;
}

void formatEEPROM() {
  for (int i = 0; i < MAX_CARDS; i++) {
    byte zeroUID[4] = {0, 0, 0, 0};
    writeRecord(i, zeroUID, 0, 0, "0000");
  }

  EEPROM.update(SIG_ADDR, SIG1);
  EEPROM.update(SIG_ADDR + 1, SIG2);

  lcd.clear();
  lcd.print("EEPROM Formatted");
  delay(1500);
}

void initEEPROMIfNeeded() {
  byte a = EEPROM.read(SIG_ADDR);
  byte b = EEPROM.read(SIG_ADDR + 1);

  if (a != SIG1 || b != SIG2) {
    lcd.clear();
    lcd.print("Init Memory...");
    delay(800);
    formatEEPROM();
  }
}

// ---------- RFID ----------
bool readCard(byte uidOut[4]) {
  if (!mfrc522.PICC_IsNewCardPresent()) return false;
  if (!mfrc522.PICC_ReadCardSerial()) return false;

  for (int i = 0; i < 4; i++) uidOut[i] = mfrc522.uid.uidByte[i];

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return true;
}

// ---------- Keypad ----------
bool checkAdminPIN() {
  lcd.clear();
  lcd.print("Admin PIN:");
  lcd.setCursor(0, 1);

  String pin = "";
  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k >= '0' && k <= '9') {
      if (pin.length() < 4) {
        pin += k;
        lcd.print("*");
      }
    } 
    else if (k == '*') {
      pin = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
    } 
    else if (k == '#') {
      return pin.equals(ADMIN_PIN);
    }
  }
}

void readPIN4(const char *msg, char outPin[5]) {
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0, 1);

  String p = "";
  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k >= '0' && k <= '9') {
      if (p.length() < 4) {
        p += k;
        lcd.print("*");
      }
    } 
    else if (k == '*') {
      p = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
    } 
    else if (k == '#') {
      if (p.length() == 4) {
        for (int i = 0; i < 4; i++) outPin[i] = p[i];
        outPin[4] = '\0';
        return;
      } else {
        lcd.clear();
        lcd.print("PIN must 4 digit");
        delay(1200);
        lcd.clear();
        lcd.print(msg);
        lcd.setCursor(0, 1);
        p = "";
      }
    }
  }
}

String readNumber(const char *msg) {
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0, 1);

  String s = "";
  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k >= '0' && k <= '9') {
      if (s.length() < 6) {
        s += k;
        lcd.print(k);
      }
    } 
    else if (k == '*') {
      s = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
    } 
    else if (k == '#') {
      return s;
    }
  }
}

// ---------- Admin ----------
void adminRegisterCard() {
  lcd.clear();
  lcd.print("Scan to Register");

  byte uid[4];
  while (!readCard(uid)) { }

  if (findCardIndex(uid) >= 0) {
    lcd.clear();
    lcd.print("Already Regd");
    delay(1500);
    return;
  }

  int empty = findEmptyIndex();
  if (empty < 0) {
    lcd.clear();
    lcd.print("Memory Full");
    delay(1500);
    return;
  }

  char newPin[5];
  readPIN4("Set Card PIN:", newPin);

  writeRecord(empty, uid, 1, 0, newPin);

  lcd.clear();
  lcd.print("Card Registered");
  delay(1500);
}

void adminSetBlock(bool blockIt) {
  lcd.clear();
  lcd.print(blockIt ? "Scan to BLOCK" : "Scan to UNBLOCK");

  byte uid[4];
  while (!readCard(uid)) { }

  int idx = findCardIndex(uid);
  if (idx < 0) {
    lcd.clear();
    lcd.print("Card Not Found");
    delay(1500);
    return;
  }

  byte ru[4], st;
  int bal;
  char pin[5];
  readRecord(idx, ru, st, bal, pin);

  st = blockIt ? 2 : 1;
  writeRecord(idx, uid, st, bal, pin);

  lcd.clear();
  lcd.print(blockIt ? "BLOCKED OK" : "ACTIVE OK");
  delay(1500);
}

void adminBlockByPIN() {
  char enteredPin[5];
  readPIN4("Block Card PIN", enteredPin);

  int idx = findCardIndexByPIN(enteredPin);
  if (idx < 0) {
    lcd.clear();
    lcd.print("PIN Not Found");
    delay(1500);
    return;
  }

  byte uid[4], st;
  int bal;
  char pin[5];
  readRecord(idx, uid, st, bal, pin);

  writeRecord(idx, uid, 2, bal, pin);

  lcd.clear();
  lcd.print("Blocked by PIN");
  delay(1500);
}

void adminMenu() {
  while (true) {
    lcd.clear();
    lcd.print("1R 2B 3U");
    lcd.setCursor(0, 1);
    lcd.print("4F 5PIN DEx");

    char k = 0;
    while (!k) k = keypad.getKey();

    if (k == '1') adminRegisterCard();
    else if (k == '2') adminSetBlock(true);
    else if (k == '3') adminSetBlock(false);
    else if (k == '4') formatEEPROM();
    else if (k == '5') adminBlockByPIN();
    else if (k == 'D') break;
  }
}

// ---------- User ----------
void showBalance(int balance) {
  lcd.clear();
  lcd.print("Balance Rs ");
  lcd.setCursor(0, 1);
  lcd.print(balance);
}

void doFuel(int idx, const byte uid[4]) {
  byte ru[4], st;
  int bal;
  char pin[5];
  readRecord(idx, ru, st, bal, pin);

  int amount = readNumber("Fuel Amt Rs?").toInt();

  if (amount <= 0 || (amount % 10 != 0)) {
    lcd.clear();
    lcd.print("Invalid x10 only");
    delay(1500);
    return;
  }

  if (amount > bal) {
    lcd.clear();
    lcd.print("Low Balance");
    delay(1500);
    return;
  }

  int seconds = amount / RS_PER_SEC;
  if (seconds < 1) seconds = 1;

  bal -= amount;
  writeRecord(idx, uid, 1, bal, pin);

  lcd.clear();
  lcd.print("Pump ON ");
  lcd.print(seconds);
  lcd.print("s");

  pumpOn();
  unsigned long t0 = millis();
  while (millis() - t0 < (unsigned long)seconds * 1000UL) {
    if (keypad.getKey() == 'C') break;
  }
  pumpOff();

  lcd.clear();
  lcd.print("STOP Bal Rs");
  lcd.setCursor(0, 1);
  lcd.print(bal);
  delay(2000);
}

void doRecharge(int idx, const byte uid[4]) {
  byte ru[4], st;
  int bal;
  char pin[5];
  readRecord(idx, ru, st, bal, pin);

  int addAmt = readNumber("Recharge Rs?").toInt();

  if (addAmt <= 0) {
    lcd.clear();
    lcd.print("Invalid Amount");
    delay(1500);
    return;
  }

  long nb = (long)bal + addAmt;
  if (nb > 65000) nb = 65000;
  bal = (int)nb;

  writeRecord(idx, uid, 1, bal, pin);

  lcd.clear();
  lcd.print("Recharged OK");
  lcd.setCursor(0, 1);
  lcd.print("Bal Rs ");
  lcd.print(bal);
  delay(2000);
}

void userMenuActive(int idx, const byte uid[4]) {
  while (true) {
    lcd.clear();
    lcd.print("1Fuel 2Rchg");
    lcd.setCursor(0, 1);
    lcd.print("3Block DExit");

    char k = keypad.getKey();
    if (!k) continue;

    if (k == '1') doFuel(idx, uid);
    else if (k == '2') doRecharge(idx, uid);
    else if (k == '3') {
      byte ru[4], st;
      int bal;
      char pin[5];
      readRecord(idx, ru, st, bal, pin);

      writeRecord(idx, uid, 2, bal, pin);

      lcd.clear();
      lcd.print("Card BLOCKED");
      delay(1500);
      return;
    } 
    else if (k == 'D') {
      return;
    }
  }
}

void userMenuBlocked(int idx, const byte uid[4]) {
  byte ru[4], st;
  int bal;
  char pin[5];
  readRecord(idx, ru, st, bal, pin);

  char inPin[5];
  readPIN4("PIN to UNBLOCK", inPin);

  if (strcmp(inPin, pin) == 0) {
    writeRecord(idx, uid, 1, bal, pin);
    lcd.clear();
    lcd.print("UNBLOCKED");
    delay(1500);
  } else {
    lcd.clear();
    lcd.print("Wrong PIN");
    delay(1500);
  }
}

// ================== SETUP & LOOP ==================
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pumpOff();

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();

  initEEPROMIfNeeded();

  lcd.clear();
  lcd.print("Pump System");
  lcd.setCursor(0, 1);
  lcd.print("Ready...");
  delay(1200);

  lcd.clear();
  lcd.print("A Admin / Scan");
}

void loop() {
  // Admin
  char k = keypad.getKey();
  if (k == 'A') {
    if (checkAdminPIN()) {
      lcd.clear();
      lcd.print("Admin OK");
      delay(800);
      adminMenu();
    } else {
      lcd.clear();
      lcd.print("Wrong Admin PIN");
      delay(1200);
    }

    lcd.clear();
    lcd.print("A Admin / Scan");
  }

  // User scan
  byte uid[4];
  if (readCard(uid)) {
    int idx = findCardIndex(uid);

    if (idx < 0) {
      lcd.clear();
      lcd.print("Not Registered");
      delay(1500);
      lcd.clear();
      lcd.print("A Admin / Scan");
      return;
    }

    byte ru[4], st;
    int bal;
    char savedPin[5];
    readRecord(idx, ru, st, bal, savedPin);

    char inPin[5];
    readPIN4("Enter Card PIN", inPin);

    if (strcmp(inPin, savedPin) != 0) {
      lcd.clear();
      lcd.print("Wrong PIN");
      delay(1500);
      lcd.clear();
      lcd.print("A Admin / Scan");
      return;
    }

    if (st == 2) {
      userMenuBlocked(idx, uid);
    } else {
      showBalance(bal);
      delay(1200);
      userMenuActive(idx, uid);
    }

    lcd.clear();
    lcd.print("A Admin / Scan");
  }
}