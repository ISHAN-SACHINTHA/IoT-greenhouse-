#include "DHT.h"

#define DHTPIN 3     // Digital pin connected to the DHT sensor

#include <SPI.h>
#include <LoRa.h>

#define ss 10 //NSS of SX1278
#define rst 9 //RST of SX1278
#define dio0 2 //DIO0 of SX1278

String inString = "";    // string to hold input

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node3 = 0xDD; 

float Hum_Value = 0.0;
int Light_level = 0.0;
float Light_Value = 0.0;
float pressure = 0.0;
int moistue_percentage_1 = 0;
int Moisture_level = 0;
int Humidity_level = 0;

int relay1 = 4;
int relay2 = 5;
int relay3 = 6;
int relay4 = 7;

int period = 5000;
unsigned long time_now = 0;

void setup() {
  //Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  while (!Serial);
  //Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    //Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
  LoRa.setTxPower(10);
}

void loop() {

  time_now = millis();

  while (millis() < time_now + period) {
    // parse for a packet, and call onReceive with the result:
    onReceive(LoRa.parsePacket());
//      Serial.print(Hum_Value);
//      Serial.print(" / ");
//      Serial.print(Light_level);
//      Serial.print(" / ");
//      Serial.print(Light_Value);
//      Serial.print(" / ");
//      Serial.print(Hum_Value);
//      Serial.print(" / ");
//      Serial.print(pressure);
//      Serial.print(" / ");
//      Serial.print(moistue_percentage_1);
//      Serial.print(" / ");
//      Serial.print(Moisture_level);
//      Serial.print(" / ");
//      Serial.println(Humidity_level);
    delay(10);

  }

if(moistue_percentage_1 < Moisture_level){
  
  digitalWrite(relay1 , HIGH);
}

else{
  digitalWrite(relay1 , LOW);
}

if(Hum_Value > Humidity_level){
  
  digitalWrite(relay2 , HIGH);
}

else{
  digitalWrite(relay2 , LOW);
}

if(Light_Value < Light_level){
  
  digitalWrite(relay3 , HIGH);
}

else{
  digitalWrite(relay3 , LOW);
}
  
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  inString = "";

  while (LoRa.available()) {
    int inChar = LoRa.read();
    inString += (char)inChar;
  }

  if (incomingLength != inString.length()) {   // check length for error
    // Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node3 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }

  String q = getValue(inString, ',', 0); // Humidity
  String r = getValue(inString, ',', 1); // Light_level
  String s = getValue(inString, ',', 2); // Light
  String t = getValue(inString, ',', 3); // pressure
  String u = getValue(inString, ',', 4); // moistue_percentage_1
  String v = getValue(inString, ',', 5); // Moisture_level
  String w = getValue(inString, ',', 6); // Humidity_level


  Hum_Value = q.toFloat();
  Light_level = r.toInt();
  Light_Value = s.toFloat();
  pressure = t.toFloat();
  moistue_percentage_1 = u.toInt();
  Moisture_level = v.toInt();
  Humidity_level = w.toInt();

  //Serial.print("   node 3 - ");
  //Serial.println(inString);

  //delay(100);
}

String getValue(String data, char separator, int index) {

  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
