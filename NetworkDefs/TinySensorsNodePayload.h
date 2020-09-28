// This file defines the payload data for each sensor

// Single mains power sensor + Vrms
#ifdef IS_SENSOR_POWER_SINGLE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  power1;
  uint16_t  VRms1;
} __attribute__((packed)) Payload;
Payload txPayload;
#endif

// Double mains power sensor + Vrms
#ifdef IS_SENSOR_POWER_DOUBLE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  power1;
  uint16_t  power2;
  uint16_t  VRms1;
} __attribute__((packed)) Payload;
Payload txPayload;
#endif

// Quad mains power sensor + Vrms
#ifdef IS_SENSOR_POWER_QUAD
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  power1;
  uint16_t  power2;
  uint16_t  power3;
  uint16_t  power4;
  uint16_t  VRms1;
} __attribute__((packed)) Payload;
Payload txPayload;
#endif

// Temperature and humidity sensor
#ifdef IS_SENSOR_TEMP_RH
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  temperature;
  uint16_t  relativeHumidity;
  uint16_t  batteryVoltage;
} __attribute__((packed)) Payload;
Payload txData;
#endif

// Temperature and pressure sensor
#ifdef IS_SENSOR_TEMP_PRESSURE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  temperature;
  uint16_t  pressure;
  uint16_t  batteryVoltage;
} __attribute__((packed)) Payload;
Payload txData;
#endif