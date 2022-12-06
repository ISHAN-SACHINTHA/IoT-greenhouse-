#include "DHT.h"

#define DHTPIN 3     // Digital pin connected to the DHT sensor

#include <SPI.h>
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 2

int counter = 0;

String Mymessage = "";
String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node1 = 0xBB;

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
float h;
float t;

int temt6000Pin = A0;
float light;
int light_value;

void setup() {
  //Serial.begin(9600);
  pinMode(temt6000Pin, INPUT); //data pin for ambientlight sensor
  //Serial.println(F("DHT Lora Trasnsmitter"));

  dht.begin();

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

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  sensor();
  delay(10);
}

void sensor() {
}
void sendMessage(String outgoing, byte MasterNode, byte otherNode) {

  LoRa.beginPacket();                   // start packet
  LoRa.write(MasterNode);              // add destination address
  LoRa.write(Node1);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  delay(10);
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    // Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }
  //Serial.println(incoming);
  int Val = incoming.toInt();
  if (Val == 33) {
    int light_value = analogRead(temt6000Pin);
    light = light_value * 0.0976;// percentage calculation
    //  Serial.print(light);
    //  Serial.print("%-");
    h = dht.readHumidity();       //Gets Humidity value
    t = dht.readTemperature();    //Gets Temperature value
    Mymessage = Mymessage + String(h) + "," + String(t) + "," + String(light);
    //    Serial.print(t);
    //    Serial.print("C-");
    //    Serial.print(h);
    //    Serial.println("%");
    sendMessage(Mymessage , MasterNode, Node1);
    //Serial.println(MyMessage);
    Mymessage = "";
    delay(10);
  }

}
