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
long nodeId = 0;
long networkId = 0;
long gatewayId = 0;

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
File myFile;


void setup() {
    // Initialise serial port
    Serial.begin(SERIAL_BAUD);
    Serial.println(F("TinySensors node starting up"));

    // start I2C
    Wire.begin();
    Wire.setClock(400000L);

    // Initialise OLED display
    display.begin(&Adafruit128x64, SCREEN_ADDRESS);

    display.setFont(System5x7);
    display.clear();

    display.println(F("Initialising SD"));

    // Initialise SD card
    if (!SD.begin(SD_CHIPSELECT)) {
        Serial.println(F("Cannot initialise SD card"));
    }

    // read radio config from the SD card
    display.println(F("Reading radio config"));
    myFile = SD.open(F("radio.txt"));
    if (myFile) {
        nodeId = myFile.parseInt();
        networkId = myFile.parseInt();
        gatewayId = myFile.parseInt();
        myFile.close();

        if (nodeId == 0 || networkId == 0 || gatewayId == 0) {
            display.println("Incorrect config");
            while(true);
        }

        display.print(F("Network ID: "));
        display.println(networkId);
        display.print(F("Node ID: "));
        display.println(nodeId);
        display.print(F("Gateway ID: "));
        display.println(gatewayId);
    } else {
        display.println(F("No 'radio.txt'"));
        while(true);
    }

    // Initialise radio
    radio.initialize(FREQUENCY, nodeId, networkId);
    radio.encrypt(ENCRYPTKEY);
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
}


void loop() {
    // check for any received packets
    // we don't expect anything, but let's be nice
    if (radio.receiveDone()) {
        Serial.print(F("["));
        Serial.print(radio.SENDERID, DEC);
        Serial.print(F("] "));

        for (byte i = 0; i < radio.DATALEN; i++)
            Serial.print((char)radio.DATA[i]);

        Serial.print(F("   [RX_RSSI:"));
        Serial.print(radio.readRSSI());
        Serial.print(F("]"));

        if (radio.ACKRequested()) {
            radio.sendACK();
            Serial.print(F(" - ACK sent"));
            delay(10);
        }
        Serial.println();
    }

    // get the current time
    unsigned long currentMillis = millis();
    // let's see if we need to send a measurement
    // cater for overflow of currentMillis every 50-odd days
    if (currentMillis < lastSend || currentMillis - lastSend > transmitPeriod) {
        Serial.print(F("Current time: "));
        Serial.print(currentMillis);
        Serial.print(F("   lastSend: "));
        Serial.println(lastSend);
        lastSend = currentMillis;

        // populate our payload
        txAutoTestPayload.nodeId = nodeId;
        txAutoTestPayload.nodeFunction = SENSORNODE_AUTOTEST;

        // send the data over radio
        Serial.print(F("Sending struct ("));
        Serial.print(sizeof(txAutoTestPayload));
        Serial.print(F(" bytes) ... "));
/*
        if (radio.sendWithRetry(gatewayId, (const void*)(&txAutoTestPayload), sizeof(txAutoTestPayload)))
            Serial.print(" ok!");
        else
            Serial.print(" nothing...");
*/
        Serial.println();

        //updateDisplay();
    }
}