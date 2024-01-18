#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

// Custom data structure defined for the usage of Record Manager.
typedef struct RecordManager
{
    //Buffer Manager Page handle to access Page files
    BM_PageHandle pageHandle;

    //Buffer Pool for using Buffer Manager  
    BM_BufferPool bufferPool;

    // Record ID    
    RID recordID;

    // Variable defines the condition for scanning the records in the table
    Expr *cond;

    // Variable stores the total number of tuples in the table
    int tuplesCnt;
    // Variable stores the location of first free page which has empty slots in table
    int freePage;
    // Variable stores the count of the number of records scanned
    int scanCount;
} RecordManager;

const int MAX_NUM_PAGES = 100;
const int ATTR_SIZE = 15; // Name of the attribute size

RecordManager *recordMgr;

// This function returns a free slot within a page
int findFreeSlot(char *data, int recordSize)
{
    int i, totalSlots = PAGE_SIZE / recordSize; 

    for (i = 0; i < totalSlots; i++)
        if (data[i * recordSize] != '+')
            return i;
    return -1;
}

// Initializing Record Manager
extern RC initRecordManager (void *mgmtData)
{
    initStorageManager(); // Initializes Storage Manager for page creation
    return RC_OK;
}

// Record Manager shut down function
extern RC shutdownRecordManager ()
{
    recordMgr = NULL;
    free(recordMgr);
    return RC_OK;
}

// This function creates a TABLE with table name "name" having schema specified by "schema"
extern RC createTable (char *name, Schema *schema)
{
    recordMgr = (RecordManager*) malloc(sizeof(RecordManager));

    // Initalizing the Buffer Pool using Least Recently Used (LRU) page replacement policy
    initBufferPool(&recordMgr->bufferPool, name, MAX_NUM_PAGES, RS_LRU, NULL);

    char data[PAGE_SIZE];
    char *pageHandle = data;
     
    int result, k;
    *(int*)pageHandle = 0; 
    pageHandle = pageHandle + sizeof(int);
    *(int*)pageHandle = 1;
    pageHandle = pageHandle + sizeof(int);
    *(int*)pageHandle = schema->numAttr;
    pageHandle = pageHandle + sizeof(int); 
    *(int*)pageHandle = schema->keySize;

    pageHandle = pageHandle + sizeof(int);
    
    for(k = 0; k < schema->numAttr; k++)
        {
        // Setting attribute name
            strncpy(pageHandle, schema->attrNames[k], ATTR_SIZE);
            pageHandle = pageHandle + ATTR_SIZE;
    
        // Setting data type of attribute
            *(int*)pageHandle = (int)schema->dataTypes[k];

        // Incrementing pointer by sizeof(int) because we have data type using integer constants
            pageHandle = pageHandle + sizeof(int);

        // Setting length of datatype of the attribute
            *(int*)pageHandle = (int) schema->typeLength[k];

        // Incrementing pointer by sizeof(int)
            pageHandle = pageHandle + sizeof(int);
        }

    SM_FileHandle fileH;
        
    // Page file creation with the help of storage manager
    if((result = createPageFile(name)) != RC_OK)
        return result;
        
    // Opening the newly created page
    if((result = openPageFile(name, &fileH)) != RC_OK)
        return result;
        
    // Writing the schema to the first locaiton in the page file
    if((result = writeBlock(0, &fileH, data)) != RC_OK)
        return result;
        
    // Closing the file after writing is completed
    if((result = closePageFile(&fileH)) != RC_OK)
        return result;

    return RC_OK;
}

// This function opens the table with table name "name"
extern RC openTable (RM_TableData *rel, char *name)
{
    SM_PageHandle pageHandle;    
    
    int attrCount, k;
    
    // Setting table's meta data to our custom record manager meta data structure
    rel->mgmtData = recordMgr;
    // Setting the table's name
    rel->name = name;
    
    // Pinning a page
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, 0);
    
    // Setting the initial pointer to 0th location
    pageHandle = (char*) recordMgr->pageHandle.data;
    
    recordMgr->tuplesCnt= *(int*)pageHandle;
    pageHandle = pageHandle + sizeof(int);

    recordMgr->freePage= *(int*) pageHandle;
    pageHandle = pageHandle + sizeof(int);
    
    attrCount = *(int*)pageHandle;
    pageHandle = pageHandle + sizeof(int);
    
    Schema *schema;

    schema = (Schema*) malloc(sizeof(Schema));
    
    // Schema's parameters
    schema->numAttr = attrCount;
    schema->attrNames = (char**) malloc(sizeof(char*) *attrCount);
    schema->dataTypes = (DataType*) malloc(sizeof(DataType) *attrCount);
    schema->typeLength = (int*) malloc(sizeof(int) *attrCount);

    // Allocate memory space for storing attribute name for every attribute
    for(k = 0; k < attrCount; k++)
        schema->attrNames[k]= (char*) malloc(ATTR_SIZE);
      
    for(k = 0; k < schema->numAttr; k++)
        {
        // Setting attribute name
        strncpy(schema->attrNames[k], pageHandle, ATTR_SIZE);
        pageHandle = pageHandle + ATTR_SIZE;
       
        // Setting data type of attribute
        schema->dataTypes[k]= *(int*) pageHandle;
        pageHandle = pageHandle + sizeof(int);

        // Setting length of datatype (length of STRING) of the attribute
        schema->typeLength[k]= *(int*)pageHandle;
        pageHandle = pageHandle + sizeof(int);
    }
    
    rel->schema = schema;   

    unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);
    forcePage(&recordMgr->bufferPool, &recordMgr->pageHandle);
    
    return RC_OK;
}   
  
// This function closes the table referenced by "rel"
extern RC closeTable (RM_TableData *rel)
{
    RecordManager *recordMgr = rel->mgmtData;
    shutdownBufferPool(&recordMgr->bufferPool);

    return RC_OK;
}

// This function deletes the table having table name "name"
extern RC deleteTable (char *name)
{
    destroyPageFile(name);
    return RC_OK;
}

// This function returns the number of tuples (records) in the table referenced by "rel"
extern int getNumTuples (RM_TableData *rel)
{
    RecordManager *recordMgr = rel->mgmtData;
    return recordMgr->tuplesCnt;
}

// This function inserts a new record in the table referenced by "rel" and updates the 'record' parameter with the Record ID of the newly inserted record
extern RC insertRecord (RM_TableData *rel, Record *record)
{
    RecordManager *recordMgr = rel->mgmtData;   
    RID *recordID = &record->id; 
    
    char *data, *slotPtr;

    int recordSize = getRecordSize(rel->schema);

    recordID->page = recordMgr->freePage;

    // Pinning page i.e. telling Buffer Manager that we are using this page
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, recordID->page);
    
    // Setting the data to initial position of record's data
    data = recordMgr->pageHandle.data;
    
    // Getting a free slot using our custom function
    recordID->slot = findFreeSlot(data, recordSize);

    while(recordID->slot == -1)
    {
        // If the pinned page doesn't have a free slot then unpin that page
        unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);  
        
        recordID->page++;
        
        pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, recordID->page);      
        data = recordMgr->pageHandle.data;

        recordID->slot = findFreeSlot(data, recordSize);
    }
    
    slotPtr = data;
    
    // Mark page dirty to notify that this page was modified
    markDirty(&recordMgr->bufferPool, &recordMgr->pageHandle);
    
    // Calculation slot starting position
    slotPtr = slotPtr + (recordID->slot * recordSize);

    // Appending '+' as tombstone to indicate this is a new record and should be removed if space is lesss
    *slotPtr = '+';

    // Copy the record's data to the memory location pointed by slotPtr
    memcpy(++slotPtr, record->data + 1, recordSize - 1);

    // Unpinning a page i.e. removing a page from the BUffer Pool
    unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);
    
    // Incrementing count of tuples
    recordMgr->tuplesCnt++;
    
    // Pinback the page 
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, 0);

    return RC_OK;
}

// This function deletes a record having Record ID "id" in the table referenced by "rel"
extern RC deleteRecord (RM_TableData *rel, RID id)
{
    // Retrieving our meta data stored in the table
    RecordManager *recordMgr = rel->mgmtData;
    
    // Pinning the page which has the record which we want to update
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, id.page);

    // Update free page because this page 
    recordMgr->freePage = id.page;
    
    char *data = recordMgr->pageHandle.data;

    // Getting the size of the record
    int recordSize = getRecordSize(rel->schema);

    // Setting data pointer to the specific slot of the record
    data = data + (id.slot * recordSize);
    
    // '-' is used for Tombstone mechanism. It denotes that the record is deleted
    *data = '-';
        
    // Mark the page dirty because it has been modified
    markDirty(&recordMgr->bufferPool, &recordMgr->pageHandle);

    // Unpin the page after the record is retrieved since the page is no longer required to be in memory
    unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);

    return RC_OK;
}

// This function updates a record referenced by "record" in the table referenced by "rel"
extern RC updateRecord (RM_TableData *rel, Record *record)
{   
    // Retrieving our meta data stored in the table
    RecordManager *recordMgr = rel->mgmtData;
    
    // Pinning the page which has the record which we want to update
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, record->id.page);

    char *data;

    // Getting the size of the record
    int recordSize = getRecordSize(rel->schema);

    // Set the Record's ID
    RID id = record->id;

    // Getting record data's memory location and calculating the start position of the new data
    data = recordMgr->pageHandle.data;
    data = data + (id.slot * recordSize);
    
    // '+' is used for Tombstone mechanism. It denotes that the record is not empty
    *data = '+';
    
    // Copy the new record data to the exisitng record
    memcpy(++data, record->data + 1, recordSize - 1 );
    
    // Mark the page dirty because it has been modified
    markDirty(&recordMgr->bufferPool, &recordMgr->pageHandle);

    // Unpin the page after the record is retrieved since the page is no longer required to be in memory
    unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);
    
    return RC_OK;   
}

// This function retrieves a record having Record ID "id" in the table referenced by "rel".
// The result record is stored in the location referenced by "record"
extern RC getRecord (RM_TableData *rel, RID id, Record *record)
{
    // Retrieving our meta data stored in the table
    RecordManager *recordMgr = rel->mgmtData;
    
    // Pinning the page which has the record we want to retreive
    pinPage(&recordMgr->bufferPool, &recordMgr->pageHandle, id.page);

    // Getting the size of the record
    int recordSize = getRecordSize(rel->schema);
    char *dataPointer = recordMgr->pageHandle.data;
    dataPointer = dataPointer + (id.slot * recordSize);
    
    if(*dataPointer != '+')
    {
        // Return error if no matching record for Record ID 'id' is found in the table
        return RC_RM_NO_TUPLE_WITH_GIVEN_RID;
    }
    else
    {
        // Setting the Record ID
        record->id = id;

        // Setting the pointer to data field of 'record' so that we can copy the data of the record
        char *data = record->data;

        // Copy data using C's function memcpy(...)
        memcpy(++data, dataPointer + 1, recordSize - 1);
    }

    // Unpin the page after the record is retrieved since the page is no longer required to be in memory
    unpinPage(&recordMgr->bufferPool, &recordMgr->pageHandle);

    return RC_OK;
}

// This function scans all the records using the condition
extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
    // Checking if scan condition (test expression) is present
    if (cond == NULL)
    {
        return RC_SCAN_CONDITION_NOT_FOUND;
    }

    // Open the table in memory
    openTable(rel, "ScanTable");

        RecordManager *scanMgr;
    RecordManager *tableManager;

    // Allocating some memory to the scanMgr
        scanMgr = (RecordManager*) malloc(sizeof(RecordManager));
        
    // Setting the scan's meta data to our meta data
        scan->mgmtData = scanMgr;
        
    // 1 to start scan from the first page
        scanMgr->recordID.page = 1;
        
    // 0 to start scan from the first slot  
    scanMgr->recordID.slot = 0;
    
    // 0 because this just initializing the scan. No records have been scanned yet      
    scanMgr->scanCount = 0;

    // Setting the scan condition
        scanMgr->cond = cond;
        
    // Setting the our meta data to the table's meta data
        tableManager = rel->mgmtData;

    // Setting the tuple count
        tableManager->tuplesCnt = ATTR_SIZE;

    // Setting the scan's table i.e. the table which has to be scanned using the specified condition
        scan->rel= rel;

    return RC_OK;
}

// This function scans each record in the table and stores the result record (record satisfying the condition)
// in the location pointed by  'record'.
extern RC next (RM_ScanHandle *scan, Record *record)
{
    // Initiliazing scan data
    RecordManager *scanMgr = scan->mgmtData;
    RecordManager *tableManager = scan->rel->mgmtData;
        Schema *schema = scan->rel->schema;
    
    // Checking if scan condition (test expression) is present
    if (scanMgr->cond == NULL)
    {
        return RC_SCAN_CONDITION_NOT_FOUND;
    }

    Value *result = (Value *) malloc(sizeof(Value));
   
    char *data;
    
    // Getting record size of the schema
    int recordSize = getRecordSize(schema);

    // Calculating Total number of slots
    int totalSlots = PAGE_SIZE / recordSize;

    // Getting Scan Count
    int scanCount = scanMgr->scanCount;

    // Getting tuples count of the table
    int tuplesCnt = tableManager->tuplesCnt;

    // Checking if the table contains tuples. If the tables doesn't have tuple, then return respective message code
    if (tuplesCnt == 0)
        return RC_RM_NO_MORE_TUPLES;

    // Iterate through the tuples
    while(scanCount <= tuplesCnt)
    {  
        // If all the tuples have been scanned, execute this block
        if (scanCount <= 0)
        {
            // printf("INSIDE If scanCount <= 0 \n");
            // Set PAGE and SLOT to first position
            scanMgr->recordID.page = 1;
            scanMgr->recordID.slot = 0;
        }
        else
        {
            // printf("INSIDE Else scanCount <= 0 \n");
            scanMgr->recordID.slot++;

            // If all the slots have been scanned execute this block
            if(scanMgr->recordID.slot >= totalSlots)
            {
                scanMgr->recordID.slot = 0;
                scanMgr->recordID.page++;
            }
        }

        // Pinning the page i.e. putting the page in buffer pool
        pinPage(&tableManager->bufferPool, &scanMgr->pageHandle, scanMgr->recordID.page);
            
        // Retrieving the data of the page          
        data = scanMgr->pageHandle.data;

        // Calulate the data location from record's slot and record size
        data = data + (scanMgr->recordID.slot * recordSize);
        
        // Set the record's slot and page to scan manager's slot and page
        record->id.page = scanMgr->recordID.page;
        record->id.slot = scanMgr->recordID.slot;

        // Intialize the record data's first location
        char *dataPointer = record->data;

        // '-' is used for Tombstone mechanism.
        *dataPointer = '-';
        
        memcpy(++dataPointer, data + 1, recordSize - 1);

        // Increment scan count because we have scanned one record
        scanMgr->scanCount++;
        scanCount++;

        // Test the record for the specified condition (test expression)
        evalExpr(record, schema, scanMgr->cond, &result); 

        // v.boolV is TRUE if the record satisfies the condition
        if(result->v.boolV == TRUE)
        {
            // Unpin the page i.e. remove it from the buffer pool.
            unpinPage(&tableManager->bufferPool, &scanMgr->pageHandle);
            // Return SUCCESS           
            return RC_OK;
        }
    }
    
    // Unpin the page i.e. remove it from the buffer pool.
    unpinPage(&tableManager->bufferPool, &scanMgr->pageHandle);
    
    // Reset the Scan Manager's values
    scanMgr->recordID.page = 1;
    scanMgr->recordID.slot = 0;
    scanMgr->scanCount = 0;
    
    // None of the tuple satisfy the condition and there are no more tuples to scan
    return RC_RM_NO_MORE_TUPLES;
}

// This function closes the scan operation.
extern RC closeScan (RM_ScanHandle *scan)
{
    RecordManager *scanMgr = scan->mgmtData;
    RecordManager *recordMgr = scan->rel->mgmtData;

    // Check if scan was incomplete
    if(scanMgr->scanCount > 0)
    {
        // Unpin the page i.e. remove it from the buffer pool.
        unpinPage(&recordMgr->bufferPool, &scanMgr->pageHandle);
        
        // Reset the Scan Manager's values
        scanMgr->scanCount = 0;
        scanMgr->recordID.page = 1;
        scanMgr->recordID.slot = 0;
    }
    
    // De-allocate all the memory space allocated to the scans's meta data (our custom structure)
        scan->mgmtData = NULL;
        free(scan->mgmtData);  
    
    return RC_OK;
}

// This function returns the record size of the schema referenced by "schema"
extern int getRecordSize (Schema *schema)
{
    int size = 0, i; // offset is zero
    
    // Iterating through all the attributes in the schema
    for(i = 0; i < schema->numAttr; i++)
    {
        switch(schema->dataTypes[i])
        {
            // Switch depending on DATA TYPE of the ATTRIBUTE
            case DT_STRING:
                // If attribute is STRING then size = typeLength (Defined Length of STRING)
                size = size + schema->typeLength[i];
                break;
            case DT_INT:
                // If attribute is INTEGER, then add size of INT
                size = size + sizeof(int);
                break;
            case DT_FLOAT:
                // If attribite is FLOAT, then add size of FLOAT
                size = size + sizeof(float);
                break;
            case DT_BOOL:
                // If attribite is BOOLEAN, then add size of BOOLEAN
                size = size + sizeof(bool);
                break;
        }
    }
    return ++size;
}

// This function creates a new schema
extern Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{
    // Allocate memory space to schema
    Schema *schema = (Schema *) malloc(sizeof(Schema));
    // Set the Number of Attributes in the new schema   
    schema->numAttr = numAttr;
    // Set the Attribute Names in the new schema
    schema->attrNames = attrNames;
    // Set the Data Type of the Attributes in the new schema
    schema->dataTypes = dataTypes;
    // Set the Type Length of the Attributes i.e. STRING size  in the new schema
    schema->typeLength = typeLength;
    // Set the Key Size  in the new schema
    schema->keySize = keySize;
    // Set the Key Attributes  in the new schema
    schema->keyAttrs = keys;

    return schema; 
}

// This function removes a schema from memory and de-allocates all the memory space allocated to the schema.
extern RC freeSchema (Schema *schema)
{
    // De-allocating memory space occupied by 'schema'
    free(schema);
    return RC_OK;
}


// ******** DEALING WITH RECORDS AND ATTRIBUTE VALUES ******** //

// This function creates a new record in the schema referenced by "schema"
extern RC createRecord (Record **record, Schema *schema)
{
    // Allocate some memory space for the new record
    Record *newRecord = (Record*) malloc(sizeof(Record));
    
    // Retrieve the record size
    int recordSize = getRecordSize(schema);

    // Allocate some memory space for the data of new record    
    newRecord->data= (char*) malloc(recordSize);

    // Setting page and slot position. -1 because this is a new record and we don't know anything about the position
    newRecord->id.page = newRecord->id.slot = -1;

    // Getting the starting position in memory of the record's data
    char *dataPointer = newRecord->data;
    
    // '-' is used for Tombstone mechanism. We set it to '-' because the record is empty.
    *dataPointer = '-';
    
    // Append '\0' which means NULL in C to the record after tombstone. ++ because we need to move the position by one before adding NULL
    *(++dataPointer) = '\0';

    // Set the newly created record to 'record' which passed as argument
    *record = newRecord;

    return RC_OK;
}

// This function sets the offset (in bytes) from initial position to the specified attribute of the record into the 'result' parameter passed through the function
RC attrOffset (Schema *schema, int attrNum, int *result)
{
    int i;
    *result = 1;

    // Iterating through all the attributes in the schema
    for(i = 0; i < attrNum; i++)
    {
        // Switch depending on DATA TYPE of the ATTRIBUTE
        switch (schema->dataTypes[i])
        {
            // Switch depending on DATA TYPE of the ATTRIBUTE
            case DT_STRING:
                *result = *result + schema->typeLength[i];
                break;
            case DT_INT:
                *result = *result + sizeof(int);
                break;
            case DT_FLOAT:
                *result = *result + sizeof(float);
                break;
            case DT_BOOL:
                *result = *result + sizeof(bool);
                break;
        }
    }
    return RC_OK;
}

// This function removes the record from the memory.
extern RC freeRecord (Record *record)
{
    free(record);
    return RC_OK;
}

// This function retrieves an attribute from the given record in the specified schema
extern RC getAttr (Record *record, Schema *schema, int attrNum, Value **value)
{
    int offset = 0;

    // Getting the ofset value of attributes depending on the attribute number
    attrOffset(schema, attrNum, &offset);

    // Allocating memory space for the Value data structure where the attribute values will be stored
    Value *attribute = (Value*) malloc(sizeof(Value));

    // Getting the starting position of record's data in memory
    char *dataPointer = record->data;
    
    // Adding offset to the starting position
    dataPointer = dataPointer + offset;

    // If attrNum = 1
    schema->dataTypes[attrNum] = (attrNum == 1) ? 1 : schema->dataTypes[attrNum];
    
    switch(schema->dataTypes[attrNum])
    {
        case DT_STRING:
        {
                // Getting attribute value from an attribute of type STRING
            int length = schema->typeLength[attrNum];
            // Allocate space for string hving size - 'length'
            attribute->v.stringV = (char *) malloc(length + 1);

            // Copying string to location pointed by dataPointer and appending '\0' which denotes end of string in C
            strncpy(attribute->v.stringV, dataPointer, length);
            attribute->v.stringV[length] = '\0';
            attribute->dt = DT_STRING;
                break;
        }

        case DT_INT:
        {
            // Getting attribute value from an attribute of type INTEGER
            int value = 0;
            memcpy(&value, dataPointer, sizeof(int));
            attribute->v.intV = value;
            attribute->dt = DT_INT;
                break;
        }
    
        case DT_FLOAT:
        {
            // Getting attribute value from an attribute of type FLOAT
            float value;
            memcpy(&value, dataPointer, sizeof(float));
            attribute->v.floatV = value;
            attribute->dt = DT_FLOAT;
            break;
        }

        case DT_BOOL:
        {
            // Getting attribute value from an attribute of type BOOLEAN
            bool value;
            memcpy(&value,dataPointer, sizeof(bool));
            attribute->v.boolV = value;
            attribute->dt = DT_BOOL;
                break;
        }

        default:
            printf("Serializer not defined for the given datatype. \n");
            break;
    }

    *value = attribute;
    return RC_OK;
}

// The following function sets the attribute value in the record in the specified schema
extern RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
    int offset = 0;

    // Getting the ofset value of attributes depending on the attribute number
    attrOffset(schema, attrNum, &offset);

    // Getting the starting position of record's data in memory
    char *dataPointer = record->data;
    
    // Adding offset to the starting position
    dataPointer = dataPointer + offset;
        
    switch(schema->dataTypes[attrNum])
    {
        case DT_STRING:
        {
            // Setting attribute value of an attribute of type STRING
            // Getting the length of the string as defined while creating the schema
            int length = schema->typeLength[attrNum];

            // Copying attribute's value to the location pointed by record's data (dataPointer)
            strncpy(dataPointer, value->v.stringV, length);
            dataPointer = dataPointer + schema->typeLength[attrNum];
            break;
        }

        case DT_INT:
        {
            // Setting attribute value of an attribute of type INTEGER
            *(int *) dataPointer = value->v.intV;     
            dataPointer = dataPointer + sizeof(int);
            break;
        }
        
        case DT_FLOAT:
        {
            // Setting attribute value of an attribute of type FLOAT
            *(float *) dataPointer = value->v.floatV;
            dataPointer = dataPointer + sizeof(float);
            break;
        }
        
        case DT_BOOL:
        {
            // Setting attribute value of an attribute of type STRING
            *(bool *) dataPointer = value->v.boolV;
            dataPointer = dataPointer + sizeof(bool);
            break;
        }

        default:
            printf("Serializer not defined for the given datatype. \n");
            break;
    }           
    return RC_OK;
}