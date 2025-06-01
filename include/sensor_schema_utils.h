#ifndef SENSOR_SCHEMA_UTILS_H
#define SENSOR_SCHEMA_UTILS_H

#include "sensor_schema.h"
#include <sqlite3.h>

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
		      uint8_t conflict_field_id,
                      SensorDef* fields,
                      size_t count);

// Check for duplicate sensor IDs or names
int sensor_schema_validate(const TableSensorsSchema* schema);

//Update data value sensor


int sensor_data_value_update(SensorDef* sensor, void* value);

//Create table from squema
int create_sql_table_from_squema(sqlite3* db,const TableSensorsSchema* schema);
//Update sqlite table

int update_sql_db_from_schema(sqlite3 *, const TableSensorsSchema* schema);
    

#ifdef __cplusplus
}
#endif

#endif // SENSOR_SCHEMA_UTILS_H

