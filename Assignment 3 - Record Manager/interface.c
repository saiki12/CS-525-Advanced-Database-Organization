#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"  // Import your record manager functions
#include "storage_mgr.h"
#include "buffer_mgr.h"
#include "tables.h"
#include "expr.h"
#include "test_helper.h"

typedef struct TestRecord {
    char *a;
    char *b;
} TestRecord;

// helper methods
Schema *testSchema();
Schema *schema;
static int i=0, u=0;
RM_TableData *table;
Record *r;
RID *rids;

RC inRecords(char* arg1, char* arg2);
RC upRecords(char* arg1, char* arg2);
Record *testRecord(Schema *schema, char *a, char *b)
{
    Record *result;
    Value *value;

    createRecord(&result, schema);

    MAKE_STRING_VALUE(value, a);
    setAttr(result, schema, 0, value);
    freeVal(value);

    MAKE_STRING_VALUE(value, b);
    setAttr(result, schema, 1, value);
    freeVal(value);

    return result;
}

Record *
fromTestRecord (Schema *schema, TestRecord in)
{
    return testRecord(schema, in.a, in.b);
}

int main() {
    char command[100];
    char tableName[100];
    bool tableOpen = false;
    initRecordManager(NULL);
    schema = testSchema();
    while (1) {
        printf("\nAvailable Commands:\n");
        printf("1. CREATE <table_name>\n");
        printf("2. OPEN <table_name>\n");
        printf("3. INSERT <values>\n");
        printf("4. UPDATE <rid> <values>\n");
        printf("5. DELETE <rid>\n");
        printf("6. SCAN <condition>\n");
        printf("7. CLOSE <table_name>\n");
        printf("8. EXIT\n");
        printf("Enter a command: ");

        // Read user input
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove the newline character

        printf(command);

        // Parse the user command
        char *cmd = strtok(command, " ");
        char *arg1 = strtok(NULL, " ");
        char *arg2 = strtok(NULL, " ");


        if (strcmp(cmd, "CREATE") == 0) {
            // Implement CREATE command to create a new table
            if (arg1 != NULL) {
                if (createTable(arg1, schema) == RC_OK) {
                    printf("\nTable created successfully.\n");
                } else {
                    printf("\nError: Unable to create the table.\n");
                }
            } else {
                printf("\nError: Invalid syntax. Usage: CREATE <table_name>\n");
            }
        } else if (strcmp(cmd, "OPEN") == 0) {
            // Implement OPEN command to open an existing table
            table = (RM_TableData *) malloc(sizeof(RM_TableData));
            if (arg1 != NULL) {
                if (openTable(table, arg1) == RC_OK) {
                    printf("\nTable opened successfully.\n");
                    tableOpen = true;
                    strcpy(tableName, arg1);
                } else {
                    printf("\nError: Unable to open the table.\n");
                }
            } else {
                printf("\nError: Invalid syntax. Usage: OPEN <table_name>\n");
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            // Implement INSERT command to insert a new record
            if (tableOpen && arg1 != NULL) {
                // Parse and insert the record
                if (createRecord(&r, schema) == RC_OK) {
                    // Parse and insert record values from arg1
                    // ...
                    if (inRecords(arg1, arg2) == RC_OK) {
                        printf("\nRecord inserted successfully.\n");
                    } else {
                        printf("\nError: Unable to insert the record.\n");
                    }
                    free(r);
                } else {
                    printf("\nError: Unable to create the record.\n");
                }
            } else {
                printf("\nError: Invalid syntax or table not open. Usage: INSERT <values>\n");
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            // Implement UPDATE command to update an existing record
            // Implement UPDATE command to update an existing record
            if (tableOpen && arg1 != NULL && arg2 != NULL) {
                // Parse the RID (Record ID)
                int page, slot;
                char *cm = arg1;
                strcat(cm, " ");
                strcat(cm, arg2);
                if (sscanf(cm, "%d %d", &page, &slot) != 2) {
                    printf("\nError: Invalid RID format. Usage: UPDATE <page> <slot> <values>\n");
                    continue;
                }
        
                // Fetch the existing record based on RID
                RID rid;
                rid.page = page;
                rid.slot = slot;
                if (getRecord(table, rid, r) != RC_OK) {
                    printf("\nError: Unable to fetch the record for update.\n");
                    continue;
                }
        
                // Parse and update record values from arg2
                // ...
        
                // Update the record
                if (upRecords(arg1, arg2) == RC_OK) {
                    printf("\nRecord updated successfully.\n");
                } else {
                    printf("\nError: Unable to update the record.\n");
                }
                free(r);
            } else {printf("\nError: Invalid syntax or table not open. Usage: UPDATE <page> <slot> <values>\n");
                }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (tableOpen && arg1 != NULL) {
                // Parse the RID (Record ID)
                int page, slot;
                if (sscanf(arg1, "%d %d", &page, &slot) != 2) {
                    printf("\nError: Invalid RID format. Usage: DELETE <page> <slot>\n");
                    continue;
                }
                // Create the RID
                RID rid;
                rid.page = page;
                rid.slot = slot;
        
                // Delete the record
                if (deleteRecord(table, rid) == RC_OK) {
                    printf("\nRecord deleted successfully.\n");
                } else {
                    printf("\nError: Unable to delete the record.\n");
                }
            } else {
                printf("\nError: Invalid syntax or table not open. Usage: DELETE <page> <slot>\n");
            }
         } 
        //   else if (strcmp(cmd, "SCAN") == 0) {
        //     RM_ScanHandle *sc1 = (RM_ScanHandle *) malloc(sizeof(RM_ScanHandle));
        //     RM_ScanHandle *sc2 = (RM_ScanHandle *) malloc(sizeof(RM_ScanHandle));
        //     Expr *se1, *left, *right;
        //     int rc,rc2;
        //     // Implement SCAN command to scan records based on a condition
        //     //
        //     if (tableOpen && arg1 != NULL) {
        //         if (startScan(table, scan, cond) == RC_OK) {
        //         printf("\nScan completed successfully.\n");
        //     // Process and display the scanned records if needed
        //     // You can retrieve scanned records and print them here
        //         } else {
        //             printf("\nError: Unable to perform the scan.\n");
        //         }
        //     } else {
        //         printf("\nError: Invalid syntax or table not open. Usage: SCAN <condition>\n");
        //     }
        // } 
        else if (strcmp(cmd, "CLOSE") == 0) {
            // Implement CLOSE command to close the currently open table
            if (tableOpen) {
                closeTable(table);
                printf("\nTable closed successfully.\n");
                tableOpen = false;
            } else {
                printf("\nNo table is currently open.\n");
            }
        } 
            else if (strcmp(cmd, "EXIT") == 0) {
            // Implement EXIT command to exit the program
            if (tableOpen) {
                closeTable(table);
            }
            printf("\nExiting the program.\n");
            break;
        } else {
            printf("\nUnknown command. Please enter a valid command.\n");
        }
    }

    return 0;
}

Schema *testSchema (void)
{
    Schema *result;
    char *names[] = { "a", "b", "c" };
    DataType dt[] = { DT_INT, DT_STRING, DT_INT };
    int sizes[] = { 0, 4, 0 };
    int keys[] = {0};
    int i;
    char **cpNames = (char **) malloc(sizeof(char*) * 3);
    DataType *cpDt = (DataType *) malloc(sizeof(DataType) * 3);
    int *cpSizes = (int *) malloc(sizeof(int) * 3);
    int *cpKeys = (int *) malloc(sizeof(int));

    for(i = 0; i < 3; i++)
    {
        cpNames[i] = (char *) malloc(2);
        strcpy(cpNames[i], names[i]);
    }
    memcpy(cpDt, dt, sizeof(DataType) * 3);
    memcpy(cpSizes, sizes, sizeof(int) * 3);
    memcpy(cpKeys, keys, sizeof(int));

    result = createSchema(3, cpNames, cpDt, cpSizes, 1, cpKeys);

    return result;
}

RC inRecords(char* arg1, char* arg2)
{
    TestRecord inserts[] = {{arg1, arg2}};
    rids = (RID *) malloc(sizeof(RID) * (i+1));
    // insert rows into table
    r = fromTestRecord(schema, inserts[i]);
    insertRecord(table, r);
    rids[i++] = r->id;
    free(rids);
    return RC_OK;
}

RC upRecords(char* arg1, char* arg2) {
    TestRecord updates[] = {{arg1, arg2}};
    r = fromTestRecord(schema, updates[u]);
    r->id = rids[u];
    updateRecord(table, r);
    return RC_OK;
}