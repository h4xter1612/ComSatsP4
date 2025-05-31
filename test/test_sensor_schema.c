#include <stdio.h>
#include "sensor_schema.h"
#include "sensor_schema_utils.h"
#include <string.h>
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
    	//*((float*)sensors[0].data_ptr) = 23.7f;
    	//*((int32_t*)sensors[1].data_ptr) = 1020;
    	//snprintf((char*)sensors[2].data_ptr, MAX_SENSOR_NAME_LEN, "OK");
    //Update sensor value

    float up_temp = 24.0f;
    int up_pres = 2;
    char up_statos[32] = "OK";
    sensor_data_value_update(&sensors[0] ,  &up_temp);
    sensor_data_value_update(&sensors[1] ,  &up_pres);
    sensor_data_value_update(&sensors[2] ,  &up_statos);
    // Print results
    printf("Sensor Values:\n");
    printf(" - Temperature: %.2f\n", temperature);
    printf(" - Pressure: %d\n", pressure);
    printf(" - Status: %s\n", status_text);
   
    up_temp = 26.0f;
    up_pres = 5;
    strcpy(up_statos, "ERROR");
    
    sensor_data_value_update(&sensors[0] ,  &up_temp);
    sensor_data_value_update(&sensors[1] ,  &up_pres);
    sensor_data_value_update(&sensors[2] ,  &up_statos);
    // Print results
    printf("Sensor Values:\n");
    printf(" - Temperature: %.2f\n", temperature);
    printf(" - Pressure: %d\n", pressure);
    printf(" - Status: %s\n", status_text);

   
    return 0;

}

