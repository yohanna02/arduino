#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 5, 6, 8, 7 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 2, 3, 4 };     //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define CS_RFID 10
#define RST_RFID 9

// Instance of the class for RFID
MFRC522 rfid(CS_RFID, RST_RFID);

#define ESP_SERIAL Serial

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

struct Voter {
  String cardId;
  String name;
};

#define NUM_OF_VOTERS 3

Voter voters[NUM_OF_VOTERS] = {
  Voter{ .cardId = "B3267A06", .name = "John" },
  Voter{ .cardId = "938F8D03", .name = "Musa" },
  Voter{ .cardId = "D06EFF58", .name = "Ibrahim" }
};

String scannedCard = "";

void setup() {
  // put your setup code here, to run once:
  ESP_SERIAL.begin(9600);
  lcd.begin(16, 2);
  // Init SPI bus
  SPI.begin();
  // Init MFRC522
  rfid.PCD_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print(F("1. Vote"));
  lcd.setCursor(0, 1);
  lcd.print(F("2.Result 3.Clear"));

  char key = keypad.getKey();

  if (key == '1') {
    lcd.clear();
    lcd.print(F("Select party"));
    lcd.setCursor(0, 1);
    lcd.print(F("A    B     C"));
    char party_selected;

    while (party_selected = keypad.waitForKey()) {
      if (party_selected >= '1' && party_selected <= '3') {
        break;
      }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Place card to"));
    lcd.setCursor(0, 1);
    lcd.print(F("vote"));

    while (!getID(scannedCard))
      ;
    
    lcd.clear();
    lcd.print(F("Loading..."));

    int i = 0;
    for (i = 0; i < NUM_OF_VOTERS; i++) {
      if (voters[i].cardId == scannedCard) {
        String data = "vote:" + String(i) + ":" + String(party_selected);
        Serial.print(data);
        break;
      }

      if (NUM_OF_VOTERS - 1 == i) {
        lcd.clear();
        lcd.print(F("Card not"));
        lcd.setCursor(0, 1);
        lcd.print(F("registered"));
        delay(3000);
        lcd.clear();
        return;
      }
    }

    while (!Serial.available())
      ;

    String res = Serial.readString();

    lcd.clear();
    lcd.print(res);
    delay(3000);
    lcd.clear();
    return;
  }

  if (key == '2') {
    lcd.clear();
    lcd.print(F("Getting results"));
    Serial.print(F("result"));
    while (!Serial.available())
      ;

    String res = Serial.readString();

    lcd.clear();
    lcd.print(res);
    delay(5000);
    lcd.clear();
    return;
  }

  if (key == '3') {
    lcd.clear();
    lcd.print(F("Clearing results"));
    Serial.print(F("clear"));
    while (!Serial.available())
      ;

    String res = Serial.readString();

    lcd.clear();
    lcd.print(res);
    delay(3000);
    lcd.clear();
    return;
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
