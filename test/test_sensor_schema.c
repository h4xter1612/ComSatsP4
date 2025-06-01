#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "sensor_schema.h"
#include "sensor_schema_utils.h"

int main(void) {
    // Simulated sensor values
    float temperature = 0.0f;
    int32_t pressure = 0;
    char status_text[MAX_SENSOR_NAME_LEN] = "";
    char time_string[50] = ""; 

    time_t current_time = time(NULL);
    struct tm* time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string),"%Y-%m-%d %H:%M:%S",time_info);
    // Define and initialize sensorr
    SensorDef sensors[4];
	
    sensor_def_init(&sensors[0], 1, "temperature", SENSOR_REAL, &temperature);
    sensor_def_init(&sensors[1], 2, "pressure", SENSOR_INTEGER, &pressure);
    sensor_def_init(&sensors[2], 3, "status", SENSOR_TEXT, &status_text);
    sensor_def_init(&sensors[3], 4, "time", SENSOR_TEXT, &time_string);

    // Create the schema
    TableSensorsSchema schema;
    if (table_schema_init(&schema, "missionsatp4",3, sensors, 4) != 0) {
        printf("Failed to initialize schema.\n");
        return -1;
    }

    // Validate schema
    if (sensor_schema_validate(&schema) != 0) {
        printf("Schema validation failed.\n");
        return -1;
    }

    // Open SQLite database
    sqlite3* db;
    if (sqlite3_open("telemetry.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Create table based on schema
    if (create_sql_table_from_squema(db, &schema) != 0) {
        fprintf(stderr, "Failed to create table.\n");
        sqlite3_close(db);
        return -1;
    }

    // Simulate updates
    float up_temp = 24.0f;
    int up_pres = 2;
    char up_statos[32] = "OK";

    sensor_data_value_update(&sensors[0], &up_temp);
    sensor_data_value_update(&sensors[1], &up_pres);
    sensor_data_value_update(&sensors[2], &up_statos);
    char new_time[50];
    time_t now = time(NULL);
    strftime(new_time, sizeof(new_time), "%Y-%m-%d %H:%M:%S", localtime(&now));

    sensor_data_value_update(&sensors[3], &new_time);
    if (update_sql_db_from_schema(db, &schema) != 0) {
    fprintf(stderr, "Failed to upsert sensor data.\n");
	} 
    // Print results
    printf("Sensor Values:\n");
    printf(" - Temperature: %.2f\n", temperature);
    printf(" - Pressure: %d\n", pressure);
    printf(" - Status: %s\n", status_text);
    printf(" - Time: %s\n", time_string);
    up_temp = 26.0f;
    up_pres = 5;
    strcpy(up_statos, "ERROR");
	
    sensor_data_value_update(&sensors[0], &up_temp);
    sensor_data_value_update(&sensors[1], &up_pres);
    sensor_data_value_update(&sensors[2], &up_statos);
    sensor_data_value_update(&sensors[3], &new_time);
    if (update_sql_db_from_schema(db, &schema) != 0) {
    fprintf(stderr, "Failed to upsert sensor data.\n");
	}
    printf("Sensor Values (updated):\n");
    printf(" - Temperature: %.2f\n", temperature);
    printf(" - Pressure: %d\n", pressure);
    printf(" - Status: %s\n", status_text);
    printf(" - Time: %s\n", time_string);
    // Close database
    sqlite3_close(db);

    return 0;
}
	
