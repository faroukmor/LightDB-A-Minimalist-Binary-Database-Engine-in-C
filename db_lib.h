/* * ============================================================================================
 * LightDB - A Minimalist Binary Database Engine in C
 * ============================================================================================
 * Description: 
 * A single-header library for managing binary database files. It supports dynamic 
 * schema definitions, full CRUD operations (Create, Read, Update, Delete), 
 * and a search engine with "Silent Mode" for programmatic indexing.
 * * Author: Farouk (Your Name)
 * Version: 1.0.0
 * * ------------------------------------------------------------------------------------------
 * HOW TO USE:
 * ------------------------------------------------------------------------------------------
 * 1. In ONE C file, define the implementation macro before including:
 * #define DB_LIB_IMPLEMENTATION
 * #include "db_lib.h"
 * * 2. In other files, just include the header:
 * #include "db_lib.h"
 * * ------------------------------------------------------------------------------------------
 * API QUICK REFERENCE:
 * ------------------------------------------------------------------------------------------
 * - create_db(file, schema)       : Initialize a DB with "colName TYPE, ..."
 * - write_data(file, values)      : Insert record as "val1, val2, ..."
 * - read_all_data(file)           : Print all records to console.
 * - search(file, query)           : Search by "col = val" or "col LIKE val".
 * Use "SILENT col = val" for background processing.
 * - update_data(file, q, newVals) : Update record(s) matching the query.
 * - delete_data(file, q)          : Physically remove record(s) from the file.
 * * Supported Types: INT, TXT (50 chars), BOOL (0/1).
 * ============================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_COLUMNS 100

typedef enum { INT, TXT, BOOL } DATATYPE;

struct Column {
    char name[50];
    DATATYPE type;
    int size;
};


struct table {
    int numOfColumns;
    struct Column colomn[MAX_COLUMNS];
    int recordSize;
};

struct SearchResult {
    int *indexes;
    int count;
};


void create_db(char *filename, char *instruction);
void write_data(char *db_name, char *instruction);
void read_data(char *db_name, char *instruction);
void read_all_data(char *db_name);
struct SearchResult search(char *db_name, char *instruction);
void update_data(char *db_name, char *search_query, char *new_values);
void delete_data(char *db_name, char *search_query);


#if defined (DB_LIB_IMPLEMENTATION) 

static struct table load_schema(char *filename) {
    struct table myTable;
    FILE *database = fopen(filename, "rb");
    if (database == NULL) {
        printf("Error: Could not open database file!\n");
        exit(1);
    }
    fread(&myTable, sizeof(struct table), 1, database);
    fclose(database);
    return myTable;
}

void create_db(char *filename, char *instruction) {
    struct table myTable;
    char temp_instruction[256];
    strcpy(temp_instruction, instruction);

    char *token = strtok(temp_instruction, " ,"); 
    int tokCount = 0;
    int colCount = 0;

    while (token != NULL) {
        if (tokCount % 2 == 0) {
            strcpy(myTable.colomn[colCount].name, token);
        } 
        else {
            if (strcmp(token, "INT") == 0) {
                myTable.colomn[colCount].type = INT;
                myTable.colomn[colCount].size = 4; 
            } else if (strcmp(token, "TXT") == 0) {
                myTable.colomn[colCount].type = TXT;
                myTable.colomn[colCount].size = 50; 
            } else if (strcmp(token, "BOOL") == 0) {
                myTable.colomn[colCount].type = BOOL; 
                myTable.colomn[colCount].size = 4; 
            }
            colCount++;
        }
        tokCount++;
        token = strtok(NULL, " ,");
    }
    myTable.numOfColumns = colCount;
    myTable.recordSize = 0;
    for (int i = 0; i < colCount; i++) {
        myTable.recordSize += myTable.colomn[i].size;
    }

    FILE *database = fopen(filename, "wb");
    if (database != NULL) {
        fwrite(&myTable, sizeof(struct table), 1, database);
        fclose(database);
    }
}

void write_data(char *db_name, char *instruction) {
    struct table loadedTable = load_schema(db_name);
    char *buffer = malloc(loadedTable.recordSize);
    memset(buffer, 0, loadedTable.recordSize);
    
    char temp_instruction[256];
    strcpy(temp_instruction, instruction);

    int offset = 0; 
    char *token = strtok(temp_instruction, " ,");
    for (int i = 0; i < loadedTable.numOfColumns; i++) {
        if (token == NULL) break;
        if (loadedTable.colomn[i].type == INT) {
            int val = atoi(token);
            memcpy(buffer + offset, &val, sizeof(int));
        } else if (loadedTable.colomn[i].type == TXT) {
            strncpy(buffer + offset, token, loadedTable.colomn[i].size);
        }
        offset += loadedTable.colomn[i].size;
        token = strtok(NULL, " ,");
    }

    FILE *database = fopen(db_name, "ab");
    if (database != NULL) {
        fwrite(buffer, loadedTable.recordSize, 1, database);
        fclose(database);
    }
    free(buffer);
}

void read_data(char *db_name, char *instruction) {
    struct table loadedTable = load_schema(db_name);
    FILE *database = fopen(db_name, "rb");
    if (database == NULL) return;

    int selected[MAX_COLUMNS] = {0};
    int all_columns = 0;
    
    if (strcmp(instruction, "*") == 0) {
        all_columns = 1;
    } else {
        char temp_instruction[256];
        strcpy(temp_instruction, instruction);
        char *token = strtok(temp_instruction, " ,");
        while (token != NULL) {
            for (int i = 0; i < loadedTable.numOfColumns; i++) {
                if (strcmp(token, loadedTable.colomn[i].name) == 0) selected[i] = 1;
            }
            token = strtok(NULL, " ,");
        }
    }

    char *buffer = malloc(loadedTable.recordSize);
    fseek(database, sizeof(struct table), SEEK_SET);
    printf("\n--- %s Data ---\n", db_name);

    while(fread(buffer, loadedTable.recordSize, 1, database) == 1) {
        int offset = 0;
        for (int i = 0; i < loadedTable.numOfColumns; i++) {
            if (all_columns || selected[i]) {
                if (loadedTable.colomn[i].type == INT) {
                    int val; memcpy(&val, buffer + offset, sizeof(int));
                    printf("%d\t", val);
                } else {
                    printf("%s\t", buffer + offset);
                }
            }
            offset += loadedTable.colomn[i].size;
        }
        printf("\n");
    }
    fclose(database);
    free(buffer);
}

void read_all_data(char *db_name) { read_data(db_name, "*"); }

struct SearchResult search(char *db_name, char *instruction) {
    struct table loadedTable = load_schema(db_name);
    struct SearchResult finalResult = {NULL, 0};

    char temp_instruction[256];
    strcpy(temp_instruction, instruction);
    int is_silent = 0;
    char *token = strtok(temp_instruction, " ");
    
    if (token != NULL && strcmp(token, "SILENT") == 0) {
        is_silent = 1;
        token = strtok(NULL, " ");
    }

    char *col_name = token;
    char *operator = strtok(NULL, " ");
    char *target_value = strtok(NULL, " ");

    if (!col_name || !operator || !target_value) return finalResult;

    int target_offset = 0, target_type = -1, col_found = 0;
    for (int i = 0; i < loadedTable.numOfColumns; i++) {
        if (strcmp(col_name, loadedTable.colomn[i].name) == 0) {
            target_type = loadedTable.colomn[i].type;
            col_found = 1;
            break;
        }
        target_offset += loadedTable.colomn[i].size;
    }
    if (!col_found) return finalResult;

    if (is_silent) finalResult.indexes = malloc(sizeof(int) * 100);

    FILE *database = fopen(db_name, "rb");
    fseek(database, sizeof(struct table), SEEK_SET);
    char *buffer = malloc(loadedTable.recordSize);
    int current_index = 0;

    while (fread(buffer, loadedTable.recordSize, 1, database) == 1) {
        int match = 0;
        if (target_type == INT) {
            int current_val; memcpy(&current_val, buffer + target_offset, 4);
            if (current_val == atoi(target_value)) match = 1;
        } else if (target_type == TXT) {
            char *current_str = (char *)(buffer + target_offset);
            if (strcmp(operator, "=") == 0 && strcmp(current_str, target_value) == 0) match = 1;
            else if (strcmp(operator, "LIKE") == 0 && strstr(current_str, target_value)) match = 1;
        }

        if (match) {
            if (is_silent) finalResult.indexes[finalResult.count++] = current_index;
            else {
                int p_off = 0;
                for (int j = 0; j < loadedTable.numOfColumns; j++) {
                    if (loadedTable.colomn[j].type == INT) {
                        int v; memcpy(&v, buffer + p_off, 4); printf("%d\t", v);
                    } else printf("%s\t", buffer + p_off);
                    p_off += loadedTable.colomn[j].size;
                }
                printf("\n");
            }
        }
        current_index++;
    }
    free(buffer); fclose(database);
    return finalResult;
}

void update_data(char *db_name, char *search_query, char *new_values) {
    struct table loadedTable = load_schema(db_name);
    char silent_instruction[300] = "SILENT ";
    strcat(silent_instruction, search_query);
    struct SearchResult res = search(db_name, silent_instruction);

    if (res.count == 0) {
        if (res.indexes) free(res.indexes);
        return;
    }

    char *new_buffer = malloc(loadedTable.recordSize);
    memset(new_buffer, 0, loadedTable.recordSize);
    char temp_vals[256]; strcpy(temp_vals, new_values);
    char *token = strtok(temp_vals, " ,");
    int offset = 0;

    for (int i = 0; i < loadedTable.numOfColumns; i++) {
        if (!token) break;
        if (loadedTable.colomn[i].type == INT) {
            int v = atoi(token); memcpy(new_buffer + offset, &v, 4);
        } else strncpy(new_buffer + offset, token, loadedTable.colomn[i].size);
        offset += loadedTable.colomn[i].size;
        token = strtok(NULL, " ,");
    }

    FILE *db = fopen(db_name, "r+b");
    for (int i = 0; i < res.count; i++) {
        fseek(db, sizeof(struct table) + (res.indexes[i] * loadedTable.recordSize), SEEK_SET);
        fwrite(new_buffer, loadedTable.recordSize, 1, db);
    }
    fclose(db); free(new_buffer); free(res.indexes);
}

void delete_data(char *db_name, char *search_query) {
    struct table loadedTable = load_schema(db_name);
    char silent_instruction[300] = "SILENT ";
    strcat(silent_instruction, search_query);
    struct SearchResult res = search(db_name, silent_instruction);

    if (res.count == 0) {
        if (res.indexes) free(res.indexes);
        return;
    }

    FILE *old_db = fopen(db_name, "rb");
    FILE *new_db = fopen("temp.db", "wb");
    fwrite(&loadedTable, sizeof(struct table), 1, new_db);

    char *buffer = malloc(loadedTable.recordSize);
    fseek(old_db, sizeof(struct table), SEEK_SET);
    int current_index = 0;

    while (fread(buffer, loadedTable.recordSize, 1, old_db) == 1) {
        int delete_this = 0;
        for (int i = 0; i < res.count; i++) {
            if (res.indexes[i] == current_index) { delete_this = 1; break; }
        }
        if (!delete_this) fwrite(buffer, loadedTable.recordSize, 1, new_db);
        current_index++;
    }

    fclose(old_db); fclose(new_db);
    free(buffer); free(res.indexes);
    remove(db_name); rename("temp.db", db_name);
}
#endif
