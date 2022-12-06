#include <SPI.h>
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 2

const int pressureInput = A2; //select the analog input pin for the pressure transducer
const int pressureZero = 102.4; //analog reading of pressure transducer at 0psi
const int pressureMax = 921.6; //analog reading of pressure transducer at 100psi
const int pressuretransducermaxPSI = 100; //psi value of transducer being used

float pressureValue = 0; //variable to store the value coming from the pressure transducer

int counter = 0;

const int dry = 830; // dry value for moisture sensor
const int wet = 645; // wet value for moisture sensor

String Mymessage = "";
String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node2 = 0xCC;

#define PULSE_PIN 3  // flow meter pin

volatile long pulseCount = 0;
float calibrationFactor = 7.5;
float flowRate;

unsigned long oldTime;

void pulse ()
{
  pulseCount++;
}
void flowMeter(void);
void moisture(void);

int moistue_percentage_1;
int moistue_percentage_2;


void setup() {


 // Serial.begin(9600);

  pulseCount        = 0;
  flowRate          = 0.0;
  oldTime           = 0;


  while (!Serial);
  //Serial.println("LoRa node 2");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    //Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }
  LoRa.setTxPower(10);
}

void loop() {
  flowMeter();
  moisture();
  pressure();
  onReceive(LoRa.parsePacket());
  //delay(10);
}

void pressure(void){

  pressureValue = analogRead(pressureInput); //reads value from input pin and assigns to variable
  pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
  //Serial.print(pressureValue, 1); //prints value from previous line to serial
  //Serial.println("psi"); //prints label to serial
  
}

void flowMeter(void){
  
  detachInterrupt(PULSE_PIN);
  flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
  oldTime = millis();

  pulseCount = 0;

  attachInterrupt(1, pulse, FALLING);
  delay(10);
}

void moisture(void) {

  int sensorVal1 = analogRead(A0);
   moistue_percentage_1 = map(sensorVal1, wet, dry, 100, 0);
  int sensorVal2 = analogRead(A1);
   moistue_percentage_2 = map(sensorVal2, wet, dry, 100, 0);

//    Serial.print(moistue_percentage_1);
//    Serial.print("% - ");
//    Serial.print(moistue_percentage_2);
//    Serial.println("% - ");
}
void sendMessage(String outgoing, byte MasterNode, byte otherNode) {

  LoRa.beginPacket();                   // start packet
  LoRa.write(MasterNode);              // add destination address
  LoRa.write(Node2);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  //delay(10);
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
  if (recipient != Node2 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }
  //Serial.println(incoming);
  int Val = incoming.toInt();
  if (Val == 44) {
    Mymessage = Mymessage + String(flowRate) + "," + String(moistue_percentage_1) + "," + String(moistue_percentage_2) + "," + String(pressureValue);
    sendMessage(Mymessage , MasterNode, Node2);
    //Serial.println(Mymessage);
    Mymessage = "";
    //delay(100);
  }
}
