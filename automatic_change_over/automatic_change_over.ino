#include <LiquidCrystal.h>

LiquidCrystal lcd(A3, A2, A1, A0, 12, 13);

const int nepa = A4;
const int gen = A5;
const int relay1 = 7;
const int relay2 = 8;
const int gen_starter = 9;
const int gen_off = 11;

bool time = false;

// String number = "+2348031124574";
String number = "+2349022107944";

long previous_millis = 0;

bool nepa_once = true;
bool gen_once = true;
String jk = "";
String mass = "";

bool gen_stay_on = false;

int state = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(nepa, INPUT);
    pinMode(gen, INPUT);
    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(gen_starter, OUTPUT);
    pinMode(gen_off, OUTPUT);
    lcd.begin(16, 2);
    gsm_init();
    lcd.clear();
    lcd.print("Design and");
    lcd.setCursor(0, 1);
    lcd.print("Contstruction");
    delay(3000);
    lcd.clear();

    lcd.print("of a GSM based");
    lcd.setCursor(0, 1);
    lcd.print("automatic change");
    delay(3000);
    lcd.clear();

    lcd.print("Over switch");
    lcd.setCursor(0, 1);
    lcd.print("By");
    delay(3000);
    lcd.clear();

    lcd.print("Ibrahim Hussaini");
    lcd.setCursor(0, 1);
    lcd.print("NDA/FPTB/PGS/EE/PGD/0077/22");
    delay(3000);
    lcd.clear();

    lcd.print("Supervised by");
    lcd.setCursor(0, 1);
    lcd.print("Malam Muhammed Nazifi Dalladi");
    delay(3000);
    lcd.clear();
}

void loop()
{
    int gen_value = analogRead(gen);
    int nepa_value = analogRead(nepa);

    if (nepa_value > 400 && !gen_stay_on)
    {
        if (time)
        {
            lcd.setCursor(0, 0);
            lcd.print("POWER RESTORED");
            lcd.setCursor(0, 1);
            lcd.print("WAITING....");
        }
        if (nepa_once)
        {
            send_sms("POWER RESTORED\nWaiting for command");
            delay(500);
            receive_mode();
            nepa_once = false;
            gen_once = true;
            Serial.readString();
            digitalWrite(gen_off, HIGH);
            delay(2000);
            digitalWrite(gen_off, LOW);
        }
        previous_millis = millis();
        while (time)
        {
            if (millis() - previous_millis > 10000)
            {
                time = false;
                lcd.clear();
                Serial.readString();
            }

            if (Serial.available())
            {
                delay(2000);
                while (Serial.available())
                {
                    char massA = Serial.read();
                    mass += massA;
                }
                delay(1000);
                mass.trim();
                delay(30);
                mass.toUpperCase();
                delay(30);
                if (mass.indexOf("STAYON") > -1)
                {
                    state++;
                    Serial.readString();
                    lcd.clear();
                    lcd.print("Staying on");
                    lcd.setCursor(0, 1);
                    lcd.print("GEN");
                    delay(3000);
                    lcd.clear();
                    gen_stay_on = true;
                    time = false;
                    gen_once = false;
                    receive_mode();
                }
            }
        }
        if (!gen_stay_on)
        {
            mass = "";
            jk = "";
            lcd.setCursor(0, 0);
            lcd.print("NEPA");
            digitalWrite(relay1, HIGH);
            digitalWrite(relay2, HIGH);
        }
    }
    else
    {
        if (gen_once)
        {
            state = 0;
            time = true;
            gen_once = false;
            nepa_once = true;
            gen_stay_on = false;
            send_sms("GEN ON");
            lcd.clear();
            lcd.print("Starting GEN");
            digitalWrite(gen_starter, HIGH);
            delay(4000);
            digitalWrite(gen_starter, LOW);
            lcd.clear();
        }

        lcd.setCursor(0, 0);
        lcd.print("GEN ON");
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
    }

    if (Serial.available() && gen_stay_on)
    {
        // delay(2000);
        while (Serial.available())
        {
            char massA = Serial.read();
            mass += massA;
        }
        delay(1000);
        mass.trim();
        delay(30);
        mass.toUpperCase();
        delay(30);

        if (mass.indexOf("NEPA") > -1)
        {
            gen_stay_on = false;
            Serial.readString();
        }
    }
}

void gsm_init()
{
    int exit = 0;
    lcd.clear();
    lcd.print("Finding Module..");
    bool at_flag = 1;
    while (at_flag)
    {
        Serial.println("AT");
        exit++;
        while (Serial.available() > 0)
        {
            if (Serial.find("OK"))
            {
                at_flag = 0;
                exit = 0;
            }
        }
        if (exit > 10)
            break;
        delay(1000);
    }
    if (exit > 10)
        return;
    lcd.clear();
    lcd.print("Module Connected..");
    delay(500);
    lcd.clear();
    // lcd.print("Disabling ECHO");
    bool echo_flag = 1;
    while (echo_flag)
    {
        Serial.println("ATE0");
        exit++;
        while (Serial.available() > 0)
        {
            if (Serial.find("OK"))
            {
                echo_flag = 0;
                exit = 0;
            }
        }
        if (exit > 10)
            break;
        delay(1000);
    }
    if (exit > 10)
        return;
    //lcd.clear();
    // lcd.print("Echo OFF");
    delay(500);
    lcd.clear();
    lcd.print("Finding Network..");
    bool net_flag = 1;
    while (net_flag)
    {
        Serial.println("AT+CPIN?");
        while (Serial.available() > 0)
        {
            if (Serial.find("+CPIN: READY"))
                net_flag = 0;
        }
        delay(1000);
    }
    lcd.clear();
    lcd.print("Network Found..");
    delay(500);
    lcd.clear();
}

void send_sms(String message)
{
    Serial.println("AT+CMGF=1");
    delay(400);
    Serial.println("AT+CMGS=\"" + number + "\""); // use your 10 digit cell no. here
    delay(400);
    Serial.print(message);
    delay(200);
    Serial.write(26);
}

void receive_mode()
{
    Serial.println("AT+CLVL=0");
    delay(2000);
    Serial.println("AT+CRSL=0");
    delay(2000);
    Serial.println("AT+CMGDA=6");
    delay(5000);
    Serial.println("AT+CMGF=1");
    delay(1000);
    Serial.println("AT+CNMI=1,2,0,0,0");
}
