#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <LoRa.h>
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Dialog 4G 275";
char pass[] = "Ishan1997";
//  Auth Token are provided by the Blynk.Cloud
char auth[] = "IGN-NmkYaFpX0X6lX1ZoVGkm3c3WbjM-";

#define ss 5
#define rst 14
#define dio0 2

byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
byte Node3 = 0xDD;

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;

String inString = "";    // string to hold input
String MyMessage = "";

float Hum_Value = 0.0;
float Temp_Value = 0.0;
float Light_Value = 0.0;
float flow_rate = 0.0;
float pressure = 0.0;
int moistue_percentage_1 = 0;
int moistue_percentage_2 = 0;
int Moisture_level = 0;
int Humidity_level = 0;
int Light_level = 0;
String message;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  while (!Serial);

  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(10);
}

BLYNK_WRITE(V0)
{
  Moisture_level = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}

BLYNK_WRITE(V1)
{
  Humidity_level = param.asInt(); 
}

BLYNK_WRITE(V9)
{
  Light_level = param.asInt(); 
}

void loop() {

  Blynk.run();

  currentMillis = millis();
  currentsecs = currentMillis / 500;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    //Serial.println(Secs);
    if ( (Secs >= 1) && (Secs <= 1) )
    {

      message = "33";
      sendMessage(message, MasterNode, Node1);
      //Serial.println(message);
    }

    if ( (Secs >= 2  ) && (Secs <= 2))
    {

      message = "44";
      sendMessage(message, MasterNode, Node2);
      //Serial.println(message);
    }

    if ( (Secs >= 3  ) && (Secs <= 3 ))
    {
      message = "";
      message = message + String(Hum_Value) + "," + String(Light_level) + "," + String(Light_Value) + "," + String(pressure) + "," + String(moistue_percentage_1) + "," + String(Moisture_level) + "," + String(Humidity_level);
      sendMessage(message, MasterNode, Node3);
      Serial.println(message);
      Secs = 0;
    }

    previoussecs = currentsecs;
    Serial.print("  M_L");
    Serial.print(Moisture_level);
    Serial.print("  H_L");
    Serial.println(Humidity_level);
    Serial.print("  L_L");
    Serial.println(Light_level);
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  //delay(10);

}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  while (LoRa.available()) {
    int inChar = LoRa.read();
    inString += (char)inChar;
    MyMessage = inString;
  }

  if (incomingLength != MyMessage.length()) {   // check length for error
    //Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    // Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }


  if ( sender == 0XBB )
  {
    node1();
  }

  if ( sender == 0XCC )
  {
    node2();
  }

  //  if ( sender == 0XDD )
  //  {
  //    node3();
  //  }


}

void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void node1() {

  String q = getValue(inString, ',', 0); // Humidity
  String r = getValue(inString, ',', 1); // Temperature
  String s = getValue(inString, ',', 2); // Light

  Hum_Value = q.toFloat();
  Temp_Value = r.toFloat();
  Light_Value = s.toFloat();
  Blynk.virtualWrite(V2, q); // Humidity
  Blynk.virtualWrite(V3, r); // Temperature
  Blynk.virtualWrite(V4, s); // Light

  inString = "";
  LoRa.packetRssi();
  Serial.print("   node 1 - ");
  Serial.println(MyMessage);
}


void node2() {

  String q = getValue(inString, ',', 0); // Flow rate
  String r = getValue(inString, ',', 1); // moistue_percentage_1
  String s = getValue(inString, ',', 2); // moistue_percentage_2
  String t = getValue(inString, ',', 3); // Pressure


  flow_rate = q.toFloat();
  moistue_percentage_1 = r.toInt();
  moistue_percentage_2 = s.toInt();
  pressure = t.toFloat();
  Blynk.virtualWrite(V5, q); // flow rate
  Blynk.virtualWrite(V6, r); // moistue_percentage_1
  Blynk.virtualWrite(V7, s); // moistue_percentage_2
  Blynk.virtualWrite(V8, t); // pressure

  inString = "";
  LoRa.packetRssi();
  Serial.print("    node 2 - ");
  Serial.println(MyMessage);
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
