#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Servo.h>

Servo servo_motor;

SoftwareSerial _serial(6, 7);
// #define comm_serial Serial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&_serial);

#define CS_RFID 10
#define RST_RFID 9

// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID);

LiquidCrystal lcd(A1, A2, A3, A4, A5, A0);

#define BTN_ADD 5
#define BTN_MINUS 3
#define BTN_OK 2

String card_one = "23A7832D";
String card_two = "638C81FB";

bool enrol = false;

#define SERVO_PIN 8

void setup() {
  servo_motor.attach(SERVO_PIN);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_ADD, INPUT_PULLUP);
  pinMode(BTN_MINUS, INPUT_PULLUP);
  // pinMode(ID_ONE, OUTPUT);
  // pinMode(ID_TWO, OUTPUT);
  lcd.begin(16, 2);
  // comm_serial.begin(9600);
  finger.begin(57600);
  if (!finger.verifyPassword()) {
    lcd.print(F("Did not find"));
    lcd.setCursor(0, 1);
    lcd.print(F("fingerprint"));
    while (1)
      ;
  }
  // comm_serial.print(F("hello"));
  // Init SPI bus
  SPI.begin();
  // Init MFRC522
  rfid.PCD_Init();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(F("Place Card     "));
  lcd.setCursor(0, 1);
  lcd.print(F("Please...      "));
  String card_uid = "";
  if (getID(card_uid)) {
    lcd.clear();
    lcd.print(F("Place Finger"));

    while (1) {
      int f_id = getFingerprintID();

      if (f_id == -2) {
        lcd.clear();
        lcd.print(F("Not found"));
        delay(2000);
        lcd.clear();
        break;
      } else if (f_id != -1) {
        if (f_id == 1 && card_uid == card_one) {
          // digitalWrite(ID_ONE, HIGH);
          lcd.clear();
          lcd.print("Attendance");
          lcd.setCursor(0, 1);
          lcd.print("Taken");
          open_servo();
          lcd.clear();
          // digitalWrite(ID_ONE, LOW);
        } else if (f_id == 2 && card_uid == card_two) {
          // digitalWrite(ID_TWO, HIGH);
          lcd.clear();
          lcd.print("Attendance");
          lcd.setCursor(0, 1);
          lcd.print("Taken");
          open_servo();
          lcd.clear();
          // digitalWrite(ID_TWO, LOW);
        } else {
          lcd.clear();
          lcd.print(F("Fingerprint &"));
          lcd.setCursor(0, 1);
          lcd.print(F("card don't match"));
          delay(3000);
          lcd.clear();
        }
        break;
      }
    }
  }

  if (!digitalRead(BTN_OK)) {
    enrol = true;
    lcd.clear();
    lcd.print(F("Place card to"));
    lcd.setCursor(0, 1);
    lcd.print(F("Enrol finger"));
  }

  while (enrol) {
    String card_uid = "";
    if (getID(card_uid)) {
      uint8_t id;
      if (card_uid == card_one) {
        id = 1;
      } else if (card_uid == card_two) {
        id = 2;
      }
      if (getFingerprintEnroll(id) == FINGERPRINT_OK) {
        lcd.clear();
        lcd.print(F("Enrolled"));
        lcd.setCursor(0, 1);
        lcd.print(F("Successfully"));
      } else {
        lcd.clear();
        lcd.print(F("Not Enrolled"));
      }
      delay(3000);
      lcd.clear();
      enrol = false;
    }
  }
}

void open_servo() {
  int pos = 0;
  for (pos = 0; pos < 180; pos++) {
    servo_motor.write(pos);
    delay(15);
  }

  for (pos = 180; pos > 0; pos--) {
    servo_motor.write(pos);
    delay(15);
  }
}

bool getID(String &tagID) {
  // Getting ready for Reading PICCs
  if (!rfid.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!rfid.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    //readCard[i] = mfrc522.uid.uidByte[i];
    //I might remove this ifelse statement after testing and leave only the unspaced condition

    tagID.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    tagID.concat(String(rfid.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  rfid.PICC_HaltA();  // Stop reading
  return true;
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_NOTFOUND)
    return -2;
  if (p != FINGERPRINT_OK)
    return -1;

  return finger.fingerID;
}

uint8_t getFingerprintEnroll(uint8_t id) {
  int p = FINGERPRINT_NOFINGER;
  lcd.clear();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.clear();
        lcd.print("Finger taken");
        delay(2000);
        lcd.clear();
        break;
      case FINGERPRINT_NOFINGER:
        lcd.setCursor(0, 0);
        lcd.print("Place Finger to");
        lcd.setCursor(0, 1);
        lcd.print("Enroll");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.clear();
        lcd.print("Communication");
        lcd.setCursor(0, 1);
        lcd.print("error");
        delay(2000);
        lcd.clear();
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.clear();
        lcd.print("Imaging error");
        delay(2000);
        lcd.clear();
        break;
      default:
        lcd.clear();
        lcd.print("Unknown error");
        delay(2000);
        lcd.clear();
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }

  lcd.print("Remove finger");
  delay(3000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.print("Place same");
  lcd.setCursor(0, 1);
  lcd.print("finger again");
  delay(3000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.clear();
        lcd.print("Finger taken");
        delay(2000);
        lcd.clear();
        break;
      case FINGERPRINT_NOFINGER:
        lcd.setCursor(0, 0);
        lcd.print("Place same");
        lcd.setCursor(0, 1);
        lcd.print("finger again");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.clear();
        lcd.print("Communication");
        lcd.setCursor(0, 1);
        lcd.print("error");
        delay(2000);
        lcd.clear();
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.clear();
        lcd.print("Imaging error");
        delay(2000);
        lcd.clear();
        break;
      default:
        lcd.clear();
        lcd.print("Unknown error");
        delay(2000);
        lcd.clear();
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      return p;
    default:
      // Serial.println("Unknown error");
      return p;
  }

  // OK converted!

  lcd.clear();
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.print("Prints matched");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication");
    lcd.setCursor(0, 1);
    lcd.print("error");
    delay(2000);
    lcd.clear();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print("Finger did");
    lcd.setCursor(0, 1);
    lcd.print("not match");
    delay(2000);
    lcd.clear();
    return p;
  } else {
    lcd.print("Unknown error");
    delay(2000);
    lcd.clear();
    return p;
  }
  lcd.clear();

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.print("Stored!");
    delay(3000);
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Communication");
    lcd.setCursor(0, 1);
    lcd.print("error");
    lcd.clear();
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.print("Bad location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.print("Error writing");
    return p;
  } else {
    lcd.print("Unknown error");
    delay(3000);
    lcd.clear();
    return p;
  }

  return FINGERPRINT_OK;
}
