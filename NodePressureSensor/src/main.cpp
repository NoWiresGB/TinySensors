#include <Arduino.h>

#include "../../NetworkDefs/TinySensorsNodeTypes.h"
// define the sensor type, so we have the correct payload definition
#define IS_SENSOR_TEMP_PRESSURE
#include "../../NetworkDefs/TinySensorsNodePayload.h"

// RF-related includes
#include <RFM69.h>
#include <RFM69_ATC.h>

// BMP3XX sensor to provide the data
#include <Adafruit_BMP3XX.h>

// radio network parameters
#define NODEID      4
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

// pressure sensor
Adafruit_BMP3XX bmp; // I2C
float temperature;
float pressure;

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

    if (!bmp.begin())
        Serial.println("Could not find a valid BMP3 sensor, check wiring!");

    // set various params for the pressure sensor
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);

    // perform an initial read that we discard
    bmp.performReading();
    delay(50);

    // set the VRef to the internal 1.1V
    analogReference(INTERNAL1V1);
    // do an initial reading, which we discard
    Vanalog = analogRead(VBatPin);
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

        // read the sensor data
        if (bmp.performReading()) {
            temperature = bmp.temperature;
            pressure = bmp.pressure;

            Serial.print("Temp *C = ");
            Serial.print(temperature);
            Serial.print("\t\t Pressure hPa = ");
            Serial.println(pressure / 100.0);
        } else {
            Serial.println("Unable to read sensor data!");
            temperature = 0;
            pressure = 0;
        }

        // populate our payload
        txTempPressurePayload.nodeId = NODEID;
        txTempPressurePayload.nodeFunction = SENSORNODE_TEMP_PRESSURE;
        txTempPressurePayload.temperature = temperature * 100;
        txTempPressurePayload.pressure = pressure;

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
  
        txTempPressurePayload.batteryVoltage = VBat;

        // send the data over radio
        Serial.print("Sending struct (");
        Serial.print(sizeof(txTempPressurePayload));
        Serial.print(" bytes) ... ");

        if (radio.sendWithRetry(GATEWAYID, (const void*)(&txTempPressurePayload), sizeof(txTempPressurePayload)))
            Serial.print(" ok!");
        else
            Serial.print(" nothing...");

        Serial.println();
    }
}