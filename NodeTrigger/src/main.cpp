#include <Arduino.h>

#include "../../NetworkDefs/TinySensorsNodeTypes.h"
// define the sensor type, so we have the correct payload definition
#define IS_SENSOR_TRIGGER
#include "../../NetworkDefs/TinySensorsNodePayload.h"

// RF-related includes
#include <RFM69.h>
#include <RFM69_ATC.h>

// radio network parameters
#define NODEID      5
#define NETWORKID   90
#define GATEWAYID   1

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" // 16 character key for the network
#define ENABLE_ATC    // enable auto-transmission control

#define SERIAL_BAUD 115200

// how often report the measurements
unsigned long transmitPeriod = 60000;
// this will ensure that we'll send a measurement straight away
unsigned long lastSend = 2 * transmitPeriod;

#ifdef ENABLE_ATC
    RFM69_ATC radio;
#else
    RFM69 radio;
#endif

// set the last trigger value so that we send an initial 'measurement'
uint8_t lastTrigger = -1;
uint8_t currentTrigger;
int triggerPin = PD3;

// battery reading
int VBatPin = A0;    // Reads in the analogue number of voltage
//#define VBATTPIN A0
unsigned long VBat = 0; // This will hold the batery pack voltage 2000->3000mv
long Vanalog = 0; // Raw ADC readings of battery voltage

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println("TinySensors node starting up");
    radio.initialize(FREQUENCY,NODEID,NETWORKID);

    radio.encrypt(ENCRYPTKEY);
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);

    // set the VRef to the internal 1.1V
    analogReference(INTERNAL1V1);
    // do an initial reading, which we discard
    Vanalog = analogRead(VBatPin);

    pinMode(triggerPin, INPUT_PULLUP);
}

void loop() {
    // check for any received packets
    // we don't expect anything, but let's be nice
    if (radio.receiveDone()) {
        Serial.print('[');
        Serial.print(radio.SENDERID, DEC);
        Serial.print("] ");

        for (byte i = 0; i < radio.DATALEN; i++)
            Serial.print((char)radio.DATA[i]);

        Serial.print("   [RX_RSSI:");
        Serial.print(radio.readRSSI());
        Serial.print("]");

        if (radio.ACKRequested()) {
            radio.sendACK();
            Serial.print(" - ACK sent");
            delay(10);
        }
        Serial.println();
    }

    // read trigger status
    currentTrigger = digitalRead(triggerPin);

    if (lastTrigger != currentTrigger) {
        // we need to send the trigger
        // populate our payload
        txTriggerPayload.nodeId = NODEID;
        txTriggerPayload.nodeFunction = SENSORNODE_TRIGGER;
        // invert the trigger (as 0 is on and 1 is off)
        txTriggerPayload.trigger = 1 - currentTrigger;

        // read the battery voltage
        Vanalog = analogRead(VBatPin);
        Serial.print("ADCraw: ");
        Serial.println(Vanalog);
        // Calculate voltage: Internal Ref 1060mV..   VBAT---560k--^---220k---GND
        // Adjusted for actual reading but need more accurate resistors really! - 5% LOL.
        VBat = (Vanalog * 3750) / 1000;
        Serial.print("VBat: ");
        Serial.print(VBat);
        Serial.println("mV");
  
        txTriggerPayload.batteryVoltage = VBat;

        // send the data over radio
        Serial.print("Sending struct (");
        Serial.print(sizeof(txTriggerPayload));
        Serial.print(" bytes) ... ");

        if (radio.sendWithRetry(GATEWAYID, (const void*)(&txTriggerPayload), sizeof(txTriggerPayload)))
            Serial.print(" ok!");
        else
            Serial.print(" nothing...");

        Serial.println();

        // save the trigger status
        lastTrigger = currentTrigger;
    }
}