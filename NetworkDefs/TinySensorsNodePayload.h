// This file defines the payload data for each sensor

#ifdef IS_SENSOR_AUTO_TEST
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
} __attribute__((packed)) AutoTestPayload;
AutoTestPayload txAutoTestPayload;
#endif

// Single mains power sensor + Vrms
#ifdef IS_SENSOR_POWER_SINGLE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  power1;
  uint16_t  VRms1;
} __attribute__((packed)) SinglePowerPayload;
SinglePowerPayload txSinglePowerPayload;
#endif

// Double mains power sensor + Vrms
#ifdef IS_SENSOR_POWER_DOUBLE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  power1;
  uint16_t  power2;
  uint16_t  VRms1;
} __attribute__((packed)) DoublePowerPayload;
DoublePowerPayload txDoublePowerPayload;
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
} __attribute__((packed)) QuadPowerPayload;
QuadPowerPayload txQuadPowerPayload;
#endif

// Temperature and humidity sensor
#ifdef IS_SENSOR_TEMP_RH
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  temperature;
  uint16_t  relativeHumidity;
  uint16_t  batteryVoltage;
} __attribute__((packed)) TempRhPayload;
TempRhPayload txTempRhPayload;
#endif

// Temperature and pressure sensor
#ifdef IS_SENSOR_TEMP_PRESSURE
typedef struct {
  uint16_t  nodeId;
  uint8_t   nodeFunction;
  uint16_t  temperature;
  uint32_t  pressure;
  uint16_t  batteryVoltage;
} __attribute__((packed)) TempPressurePayload;
TempPressurePayload txTempPressurePayload;
#endif