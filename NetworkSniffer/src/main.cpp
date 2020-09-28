#include <Arduino.h>
#include <RFM69.h>
#include <RFM69_ATC.h>

#define NODEID      99
#define NETWORKID   89
#define GATEWAYID   1

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" // 16 character key for the network
#define ENABLE_ATC    // enable auto-transmission control

#define SERIAL_BAUD 115200

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("RFM69 sniffer starting up");
  radio.initialize(FREQUENCY,NODEID,NETWORKID);

  radio.encrypt(ENCRYPTKEY);
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  // enable spy mode straight away
  radio.spyMode(true);
}

char hexDigit(byte v) {
  v &= 0x0F; // just the lower 4 bits

  return v < 10 ? '0' + v : 'A' + (v - 10);
}

void loop() {
  // process any serial input
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'r') {
      radio.readAllRegs();
    }
  }

  //check for any received packets
  if (radio.receiveDone()) {
    Serial.print('[');
    Serial.print(radio.SENDERID, DEC);
    Serial.print("] [");

    // hex dump of payload
    // max data length is 61, so we allocate 2x61 + 1 for string termination
    char  hexData[123];
    byte  ptr = 0;
    for (byte i = 0; i < radio.DATALEN; i++) {
      hexData[ptr++] = hexDigit(radio.DATA[i] >> 4);
      hexData[ptr++] = hexDigit(radio.DATA[i]);
    }
    hexData[ptr] = '\0';
    String hexPayload = String(hexData);
    Serial.print(hexData);
    Serial.print("] ");

    Serial.print("   [RX_RSSI:");
    Serial.print(radio.readRSSI());
    Serial.print("]");

    Serial.println();
  }
}