#include <Arduino.h>

#include "../../NetworkDefs/TinySensorsNodeTypes.h"
// define the sensor type, so we have the correct payload definition
#define IS_SENSOR_AUTO_TEST
#include "../../NetworkDefs/TinySensorsNodePayload.h"

// RF-related includes
#include <RFM69.h>
#include <RFM69_ATC.h>

#include <Wire.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include <SPI.h>
#include <SD.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// radio network parameters
#define NODEID      6
#define NETWORKID   90
#define GATEWAYID   1

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" // 16 character key for the network
#define ENABLE_ATC    // enable auto-transmission control

#define SERIAL_BAUD 115200

// how often report the measurements
unsigned long transmitPeriod = 3000;
// this will ensure that we'll send a measurement straight away
unsigned long lastSend = 2 * transmitPeriod;

#ifdef ENABLE_ATC
    RFM69_ATC radio;
#else
    RFM69 radio;
#endif

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
SSD1306AsciiWire display;

#define SD_CHIPSELECT   9
//File myFile;

void updateDisplay() {
    display.setFont(System5x7);
    display.clear();
    display.println(F("TinySensors tester"));
    display.println(F("Net - 90  Node - 6"));
    display.println(F("Time up: 328s"));
    display.println(F("Packets out: 56"));
    display.print(F("Status: wait ("));
    display.print(millis()/1000);
    display.println(F("s)"));
}


void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println(F("TinySensors node starting up"));

    Wire.begin();
    Wire.setClock(400000L);

    display.begin(&Adafruit128x64, SCREEN_ADDRESS);
    
    updateDisplay();

    if (!SD.begin(SD_CHIPSELECT)) {
        Serial.println(F("Cannot initialise SD card"));
    }

    radio.initialize(FREQUENCY,NODEID,NETWORKID);

    radio.encrypt(ENCRYPTKEY);
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
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

    // get the current time
    unsigned long currentMillis = millis();
    // let's see if we need to send a measurement
    // cater for overflow of currentMillis every 50-odd days
    if (currentMillis < lastSend || currentMillis - lastSend > transmitPeriod) {
        Serial.print("Current time: ");
        Serial.print(currentMillis);
        Serial.print("   lastSend: ");
        Serial.println(lastSend);
        lastSend = currentMillis;

        // populate our payload
        txAutoTestPayload.nodeId = NODEID;
        txAutoTestPayload.nodeFunction = SENSORNODE_AUTOTEST;

        // send the data over radio
        Serial.print("Sending struct (");
        Serial.print(sizeof(txAutoTestPayload));
        Serial.print(" bytes) ... ");
/*
        if (radio.sendWithRetry(GATEWAYID, (const void*)(&txAutoTestPayload), sizeof(txAutoTestPayload)))
            Serial.print(" ok!");
        else
            Serial.print(" nothing...");
*/
        Serial.println();

        updateDisplay();
    }
}