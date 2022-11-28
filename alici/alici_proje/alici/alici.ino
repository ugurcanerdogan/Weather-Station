#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>
#include <Servo.h>
////////////////////////// CUSTOM CHAR FOR DEGREE SYMBOL ON LCD
byte customChar[8] = {
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
unsigned long lastEvent = 0;
////////////////////////// LCD & NRF
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 receive(9, 8);  //CE,CSN
const byte address[10] = "robotistan100";
Servo servo;
////////////////////////// DATA PACKAGE STRUCT
struct package {
  bool lightningStroke = false;
  double t = 0;
  double a = 0;
  double ra = 0;
  double p = 0;
  double p0 = 0;
  float temperature = 0;
  float humidity = 0;
};
typedef struct package Package;
Package data;

int redPin = 7;
int yellowPin = 6;
int greenPin = 5;
int bluePin = 4;
int buzzerPin = 3;
int touchPin = 2;
int counter = 0;

void wakeUp() {
  //Serial.begin(9600);
  servo.attach(10);
  pinMode(touchPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  //lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello world!");
  delay(500);
  lcd.clear();
  lcd.createChar(0, customChar);
}

void subZeroAlarm(int value) {
  bool alarm = value < 3;
  if (alarm) {
    digitalWrite(bluePin, HIGH);
    if (millis() - lastEvent > 900000) {
      digitalWrite(buzzerPin, HIGH);
      for (int i = 0; i < 180; i++) {
        servo.write(i);
        delay(2);
      }
      for (int i = 180; i > 0; i--) {
        servo.write(i);
        delay(2);
      }
      lastEvent = millis();
    }
    digitalWrite(buzzerPin, LOW);
    digitalWrite(bluePin, LOW);
  }
}

void initRadio() {
  receive.begin();
  receive.openReadingPipe(0, address);
  receive.setPALevel(RF24_PA_MIN);
  receive.setDataRate(RF24_250KBPS);
  receive.setChannel(100);
  receive.startListening();
}

void readInfo() {
  if (receive.available()) {
    //Serial.println(data.lightningStroke);
    //Serial.println("Received data! :)");
    if (counter == 0) {
      receive.read(&data, sizeof(data));
      digitalWrite(redPin, HIGH);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      subZeroAlarm(data.temperature);
      lcd.print("Temp = ");
      lcd.print(data.temperature);
      lcd.print(" ");
      lcd.write((uint8_t)0);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Hum = ");
      lcd.print(data.humidity);
      lcd.print(" %");
    } else if (counter == 1) {
      receive.read(&data, sizeof(data));
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(greenPin, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      subZeroAlarm(data.t);
      lcd.print("Temp= ");
      lcd.print(data.t);
      lcd.print(" ");
      lcd.write((uint8_t)0);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Alt= ");
      lcd.print(data.a);
      lcd.setCursor(0, 2);
      lcd.print("Ralt= ");
      lcd.print(data.ra);
      lcd.print(" mt");
    } else if (counter == 2) {
      receive.read(&data, sizeof(data));
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(greenPin, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Prsr= ");
      lcd.print(data.p);
      lcd.print(" hPa");
      lcd.setCursor(0, 1);
      lcd.print("SLVL pr= ");
      lcd.print(data.p0);
      lcd.print(" hPa");
    }
  }
  if (digitalRead(touchPin) == HIGH) {
    //Serial.println("Mode changed!");
    counter = ++counter % 3;
    Serial.println(counter);
    delay(500);
  }
}

void setup() {
  wakeUp();
  initRadio();
}

void loop() {
  readInfo();
}