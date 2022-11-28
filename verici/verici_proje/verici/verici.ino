#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
#include <Wire.h>
////////////////////////// DEFININGS
#define DHTPIN 10  // pin 0 ve pin 1'e baglamayiniz
#define DHTTYPE DHT11
//#define ALTITUDE 1020.0
#define seaLevelPressure_hPa 1013.25
////////////////////////// 7 SEGMENT LED PINS
int dp = 2;
int A = 3; 
int B = 4;
int C = 5;
//int D = A0;
//int E = A1;
int F = 6;
int G = 7;
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
////////////////////////// SENSORS
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
////////////////////////// NRF
RF24 radio(9, 8);  //CE,CSN
const byte address[6] = "robotistan100";
////////////////////////// INIT NRF RADIO
void initRadio() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);    //RF guc outputunu maksimuma ayarliyoruz
  radio.setDataRate(RF24_250KBPS);  //datarate'i 250kbps'e ayarliyoruz
  radio.setChannel(100);            //frekansi 100'e ayarliyoruz
  radio.stopListening();
}
////////////////////////// INIT SENSORS
void initSensors() {
  dht.begin();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  pinMode(A3, INPUT);
  // 7 segment led pins
  pinMode(dp, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
}
////////////////////////// TRANSMIT SENSOR DATA TO RECEIVER
void sendInfo() {
  if (analogRead(A3) < 1000) {
    data.lightningStroke = true;
  } else {
    data.lightningStroke = false;
  }
  ////////////////////////// BMP180
  data.t = bmp.readTemperature();
  data.a = bmp.readAltitude();
  data.ra = bmp.readAltitude(seaLevelPressure_hPa * 100);
  data.p = roundPa2hPa(bmp.readPressure());
  data.p0 = roundPa2hPa(bmp.readSealevelPressure());
  ////////////////////////// DHT11
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  ////////////////////////// 7 segment led controlling
  sevenSegmentControl(data.t);
  ////////////////////////// write
  radio.write(&data, sizeof(data));
}

double roundPa2hPa(int32_t value) {
  return value / 100.0;
}

void sevenSegmentControl(int value){
    if(value >= 20){
    digitalWrite(A1,LOW);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 18 && value < 20){
    digitalWrite(A1,HIGH);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,HIGH); 
  }  
  else if(value >= 16 && value < 18){
    digitalWrite(A1,LOW);
    digitalWrite(A0,LOW); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,LOW); 
    digitalWrite(G,LOW); 
  }
  else if(value >= 14 && value < 16){
    digitalWrite(A1,HIGH);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,LOW); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 12 && value < 14){
    digitalWrite(A1,LOW);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,LOW); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 10 && value < 12){
    digitalWrite(A1,LOW);
    digitalWrite(A0,LOW); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,LOW); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 8 && value < 10){
    digitalWrite(A1,LOW);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,LOW); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 6 && value < 8){
    digitalWrite(A1,HIGH);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,LOW); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,LOW); 
    digitalWrite(G,HIGH); 
  }
  else if(value >= 4 && value < 6){
    digitalWrite(A1,LOW);
    digitalWrite(A0,LOW); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,LOW); 
    digitalWrite(F,LOW); 
    digitalWrite(G,LOW); 
  }
  else if(value < 4){
    digitalWrite(A1,HIGH);
    digitalWrite(A0,HIGH); 
    digitalWrite(C,HIGH); 
    digitalWrite(B,HIGH); 
    digitalWrite(A,HIGH); 
    digitalWrite(F,HIGH); 
    digitalWrite(G,LOW); 
  }
  else{
    digitalWrite(dp,HIGH); 
  }
}

void printBMP180() {
  Serial.println("--------------");
  Serial.println("(BMP180)");
  Serial.print("Temp= ");
  Serial.print(bmp.readTemperature(), 1);
  Serial.print(" °C");
  Serial.println("");
  Serial.println("");
  //////////////////////////
  Serial.print("Alt= ");
  Serial.print(bmp.readAltitude());
  Serial.println(" mt");
  //////////////////////////
  Serial.print("Real alt = ");
  Serial.print(bmp.readAltitude(seaLevelPressure_hPa * 100));
  Serial.println(" mt");
  Serial.println("");
  //////////////////////////
  Serial.print("Prsr= ");
  Serial.print(roundPa2hPa(bmp.readPressure()));
  Serial.println(" hPa");
  //////////////////////////
  Serial.print("Sealevel prsr= ");
  Serial.print(roundPa2hPa(bmp.readSealevelPressure()));
  Serial.println(" hPa");
  Serial.println("");
}

void printDHT11() {
  Serial.println("(DHT11)");
  Serial.print("Temp= ");
  Serial.print(dht.readTemperature(), 1);
  Serial.println(" °C");
  //////////////////////////
  Serial.print("Hum= ");
  Serial.print((int)dht.readHumidity());
  Serial.println(" %");
  Serial.println("--------------");
}

void setup() {
  Serial.begin(9600);
  initRadio();
  initSensors();
}

void loop() {
  sendInfo();
  //printBMP180();
  //printDHT11();
  delay(100);
}