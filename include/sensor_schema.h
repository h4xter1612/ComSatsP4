//sensor_schema.h

#ifndef  SENSOR_SCHEMA_H
#define  SENSOR_SCHEMA_H 

#include <stddef.h>
#include <stdint.h>

//Max constants

#define MAX_SENSOR_NAME_LEN 32
#define MAX_TABLE_NAME_LEN 32


//Data types for sensors (SQLite)
typedef enum {

	SENSOR_REAL,
	SENSOR_INTEGER,
	SENSOR_TEXT

} SensorDataType;

//Define sensors with their data type of storage 
typedef struct{

        uint8_t sensor_id;
	char sensor_name[MAX_SENSOR_NAME_LEN];
	SensorDataType type;
	void* data_ptr;

} SensorDef;

//SQL Table 
typedef struct{

	char table_name[MAX_TABLE_NAME_LEN];
	uint8_t conflict_field_id;
	SensorDef* fields;
	size_t field_count;

} TableSensorsSchema;

#endif //SENSOR_SCHEMA_H
































































