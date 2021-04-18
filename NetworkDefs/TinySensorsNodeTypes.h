// This file lists the various sensor types

// Auto-test node; used only for automatic testing - not actually used in payload
#define SENSORNODE_AUTOTEST             0

// Single mains power sensor + Vrms
#define SENSORNODE_POWER_SINGLE         1
// Double mains power sensor + Vrms
#define SENSORNODE_POWER_DOUBLE         2
// Quad mains power sensor + Vrms
#define SENSORNODE_POWER_QUAD           3

// Temperature and humidity sensor + Vbatt (e.g SHT31)
#define SENSORNODE_TEMP_RH              4
// Temperature and pressure sensor + Vbatt (e.g BMP388)
#define SENSORNODE_TEMP_PRESSURE        5