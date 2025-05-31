#ifndef SENSOR_SCHEMA_UTILS_H
#define SENSOR_SCHEMA_UTILS_H

#include "sensor_schema.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize a sensor definition
int sensor_def_init(SensorDef* f,
                      uint8_t id,
                      const char* name,
                      SensorDataType type,
                      void* ptr);

// Initialize the table schema
int table_schema_init(TableSensorsSchema* schema,
                      const char* table_name,
                      SensorDef* fields,
                      size_t count);

// Check for duplicate sensor IDs or names
int sensor_schema_validate(const TableSensorsSchema* schema);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_SCHEMA_UTILS_H

