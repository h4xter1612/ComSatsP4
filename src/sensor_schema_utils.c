#include <stdio.h>
#include <string.h>
#include <sensor_schema_utils.h>
#include <stdlib.h>
#include <sqlite3.h>
// Inicializa un sensor individual
int sensor_def_init(SensorDef* f,
                    uint8_t id,
                    const char* name,
                    SensorDataType type,
                    void* ptr) {

    if (!f || !name || strlen(name) >= MAX_SENSOR_NAME_LEN || !ptr)
        return -1;

    f->sensor_id = id;

    strncpy(f->sensor_name, name, MAX_SENSOR_NAME_LEN - 1);
    f->sensor_name[MAX_SENSOR_NAME_LEN - 1] = '\0';

    f->type = type;
    f->data_ptr = ptr;
    return 0;
}

// Inicializa la estructura de tabla
int table_schema_init(TableSensorsSchema* schema,
                      const char* table_name,
		      uint8_t conflict_field_id,
                      SensorDef* fields,
                      size_t count) {
    if (!schema || !table_name || !fields || count == 0 || strlen(table_name) >= MAX_TABLE_NAME_LEN || conflict_field_id >= count)
        return -1;

    strncpy(schema->table_name, table_name, MAX_TABLE_NAME_LEN - 1);
    schema->table_name[MAX_TABLE_NAME_LEN - 1] = '\0';
    schema->conflict_field_id = conflict_field_id;
    schema->fields = fields;  // corregido nombre del campo
    schema->field_count = count;
    return 0;
}

// Valida que no haya sensores duplicados en la tabla
int sensor_schema_validate(const TableSensorsSchema* schema) {
    if (!schema || !schema->fields || schema->field_count == 0)
        return -1;

    for (size_t i = 0; i < schema->field_count; i++) {
        for (size_t j = i + 1; j < schema->field_count; j++) {
            if (schema->fields[i].sensor_id == schema->fields[j].sensor_id)
                return -1;

            if (strncmp(schema->fields[i].sensor_name, schema->fields[j].sensor_name, MAX_SENSOR_NAME_LEN) == 0)
                return -1;
        }
    }

    return 0;

}

//Update sensor value

int sensor_data_value_update(SensorDef* sensor, void* value){
	if (!sensor || !sensor->data_ptr || !value){
		return -1;
	}
	
	switch(sensor->type){
		case SENSOR_INTEGER:
			*(int*)(sensor->data_ptr) = *(int*)value;
			break;
		case SENSOR_REAL:
			*(float*)(sensor->data_ptr) = *(float*)value;
			break;
		case SENSOR_TEXT:
            		strncpy((char*)(sensor->data_ptr), (const char*)value, MAX_SENSOR_NAME_LEN - 1);
            		((char*)sensor->data_ptr)[MAX_SENSOR_NAME_LEN - 1] = '\0'; // Null-terminate
            		break;
		default:
			return -2;

	}

	return 0;
}

//Create table from squema
int create_sql_table_from_squema(sqlite3* db,const TableSensorsSchema* schema){
	if (!db || !schema->fields || schema->field_count == 0 || schema->conflict_field_id >= schema->field_count )
		return -1;
		
	//Sql stmnt
	char sql[1024] = {0}; //Quzias memoria dinamica mejor
	//Create table from squema
	snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s (id INTEGER PRIMARY KEY AUTOINCREMENT,", schema->table_name);
    // Append each sensor
    
    for (size_t i = 0; i < schema->field_count; ++i) {
        
	const SensorDef* s = &schema->fields[i];
        const char* sql_type = NULL;
	
        switch (s->type) {
            case SENSOR_REAL: sql_type = "REAL"; break;
            case SENSOR_INTEGER: sql_type = "INTEGER"; break;
            case SENSOR_TEXT: sql_type = "TEXT"; break;
            default: return -2;
        }
	//Conflict to upsert
        if (i==schema->conflict_field_id){
		char conflict_field_def[128];
		snprintf(conflict_field_def,sizeof(conflict_field_def),"%s %s NOT NULL UNIQUE%s",
		s->sensor_name, sql_type, (i < schema-> field_count - 1)? ",":")");
		strncat(sql,conflict_field_def, sizeof(sql) - strlen(sql) -1);
	}			
	// Ensure it fits (append with comma)
	else{
    		char field_def[128];
        	snprintf(field_def, sizeof(field_def), "%s %s%s",
                s->sensor_name,
                sql_type,
                (i < schema->field_count - 1) ? "," : ")");
        	strncat(sql, field_def, sizeof(sql) - strlen(sql) - 1);
	}
     }
    // Execute the SQL
    printf("Generated SQL: %s\n", sql);
    char* err_msg = NULL;
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -3;
    }

    printf("Table '%s' created successfully.\n", schema->table_name);
    return 0;

} 
















