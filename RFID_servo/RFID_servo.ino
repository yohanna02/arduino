#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>

Servo servo_motor;

#define CS_RFID 10
#define RST_RFID 9

// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID);

LiquidCrystal lcd(A0, A5, A1, A2, A3, A4);

#define SERVO_PIN 7

char *card_ids[] = {
  "63B935F6",
  "786f87"
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo_motor.attach(SERVO_PIN);
  lcd.begin(16, 2);
  // Init SPI bus
  SPI.begin();
  // Init MFRC522
  rfid.PCD_Init();

  lcd.print(F("Design and"));
  lcd.setCursor(0, 1);
  lcd.print(F("Construction"));
  delay(3000);
  lcd.clear();

  lcd.print(F("of dual"));
  lcd.setCursor(0, 1);
  lcd.print(F("verification"));
  delay(3000);
  lcd.clear();

  lcd.print(F("door lock"));
  lcd.setCursor(0, 1);
  lcd.print(F("using ESP32-CAM"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Abba Zanna Malilima"));
  lcd.setCursor(0, 1);
  lcd.print(F("19/05/04/227"));
  delay(3000);
  lcd.clear();

  lcd.print(F("Supervised by"));
  lcd.setCursor(0, 1);
  lcd.print(F("Engr.Mohammed Abatcha"));
  delay(3000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  String cardId = "";
  if (getID(cardId)) {
    lcd.clear();

    if (cardId != String(card_ids[0])) {
      lcd.clear();
      lcd.print(F("Card not"));
      lcd.setCursor(0, 1);
      lcd.print(F("found"));
      delay(3000);
      lcd.clear();
      return;
    }

    unsigned long start = millis();

    lcd.print(F("Waiting for"));
    lcd.setCursor(0, 1);
    lcd.print(F("face"));

    while (!Serial.available() || millis() - start < 30000) {
    }
    lcd.clear();
    if (Serial.readString() == "OK") {
      lcd.print(F("Door open"));
      open_servo();
    } else {
      lcd.print(F("can't find"));
      lcd.setCursor(0, 1);
      lcd.print(F("face"));
      delay(3000);
    }
    lcd.clear();
  }

  lcd.setCursor(0, 0);
  lcd.print(F("Place Card..."));
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
