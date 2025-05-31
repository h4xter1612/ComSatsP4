#include <stdio.h>
#include "sensor_schema.h"
#include "sensor_schema_utils.h"

int main(void) {
    // Simulated sensor values
    float temperature = 0.0f;
    int32_t pressure = 0;
    char status_text[MAX_SENSOR_NAME_LEN] = "";

    // Define and initialize sensors
    SensorDef sensors[3];

    sensor_def_init(&sensors[0], 1, "temperature", SENSOR_REAL, &temperature);
    sensor_def_init(&sensors[1], 2, "pressure", SENSOR_INTEGER, &pressure);
    sensor_def_init(&sensors[2], 3, "status", SENSOR_TEXT, &status_text);

    // Create the schema
    TableSensorsSchema schema;
    if (table_schema_init(&schema, "telemetry", sensors, 3) != 0) {
        printf("Failed to initialize schema.\n");
        return -1;
    }

    // Validate schema
    if (sensor_schema_validate(&schema) != 0) {
        printf("Schema validation failed.\n");
        return -1;
    }

    // Simulate updates (this would come from hardware in real case)
    *((float*)sensors[0].data_ptr) = 23.7f;
    *((int32_t*)sensors[1].data_ptr) = 1020;
    snprintf((char*)sensors[2].data_ptr, MAX_SENSOR_NAME_LEN, "OK");

    // Print results
    printf("Sensor Values:\n");
    printf(" - Temperature: %.2f\n", temperature);
    printf(" - Pressure: %d\n", pressure);
    printf(" - Status: %s\n", status_text);

    return 0;

}

