-------------Record Manager CS-525 Assignment No.3 Group 31-------------
Saikiran Somanagoudar - A20542890
Sai Dhanush Soma - A20545628
Arafath Syed - A20550937
Usha Devaraju - A20539949
Haritha Chennuru Venugopal Reddy - A20531012

EXECUTING THE PROGRAM
=====================

1) Go to Project root using Terminal.
2) Give ls command to list the files and check to ensure we are in the correct directory.
3) Give "make clean" to delete old compiled .o files.
4) Give "make" to compile all project files including "test_assign3_1.c" file 
5) Give "make run" to run "test_assign3_1.c" file.
6) Give "make test_expr" to compile test expression related files including "test_expr.c".
7) Give "make run_expr" to run "test_expr.c" file.


CONTRIBUTIONS
=============

1) Saikiran Somanagoudar (20%)
-> Function Implementations:
	- getNumTuples(...)
	- insertRecord(...)
	- deleteRecord(...)
	- updateRecord(...)
	- getRecord(...)

2) Sai Dhanush Soma (20%)
-> Function Implementations:
	- createRecord(...)
	- freeRecord(...)
	- getAttr(...)
	- setAttr(...)

3) Arfath Syed (20%)
-> Function Implementations:
	- createTable(...)
	- openTable(...)
	- closeTable(...)
	- deleteTable(...)

4) Usha Devaraju (20%)
-> Function Implementations:
	- next(...) (Scan function)
	- getRecordSize(...) (Schema function)
	- createSchema(...) (Schema function)
	- freeSchema(...) (Schema function)

5) Haritha Chennuru Venugopal Reddy (20%)
-> Function Implementations:
	- initRecordManager(...)
	- shutdownRecordManager()
	- startScan(...)
	- closeScan(...)

TABLE AND RECORD MANAGER FUNCTIONS
==================================

initRecordManager (...)
--> The record manager is initialized by this function.
--> To initialize the storage manager, we call the Storage Manager's initStorageManager(...) function.

createTable(...)
--> This function opens the table with the name that the parameter "name" specifies.
--> It calls initBufferPool(...) to initialize the Buffer Pool. Our policy is to replace pages with LRUs.
--> It establishes the table's attributes (name, datatype, and size) and initializes all of the table's values.
--> Next, a page file is created, opened, the block containing the table is written to the page file, and the page file is closed.

openTable(...)
--> The 'name' argument in the schema supplied by the 'schema' parameter is used by this method to construct a table with that name.

closeTable(...)
--> The table is closed by this function using the parameter'rel'.
--> It accomplishes this by invoking the shutdownBufferPool() function of the buffer manager.
--> The buffer manager writes the table modifications to the page file before terminating the buffer pool.

deleteTable(...)
--> This function removes the table whose name is given by the 'name' parameter.
--> It invokes the destroyPageFile() function of the Storage Manager.
--> The function destroyPageFile(...) eliminates the page from the hard drive and frees up memory space designated for that particular mechanism.

shutdownRecordManager(...)
--> This function de-allocates all resources assigned to the record management and ends the record manager.
--> It releases all resources and memory that the Record Manager was utilizing.
--> To release the memory space, we call the C method free() and set the recordManager data structure pointer to NULL.

getNumTuples(...)
--> The number of tuples in the table that the parameter'rel' references is returned by this function.
--> The variable [tuplesCount], which is specified in our own data structure and is used to store the meta-data for the tables, is returned.


RECORD FUNCTIONS
================

insertRecord(...)
--> This function replaces the'record' argument with the Record ID supplied in the insertRecord() function and adds a record in the table.
--> For the record that is being inserted, we set the Record ID.
--> The page with the open slot is pinned. We find the data pointer and append a '+' to indicate that this is a freshly inserted record as soon as we find an empty space.
In order for the Buffer Manager to write the page's content back to the disk, we also flag the page as unclean.
--> Using the C method memcpy(), we copy the record's data (provided by parameter "record") into the new record before unpinning the page.

deleteRecord(...)
--> From the database that the parameter "rel" references, this function deletes a record whose Record ID ('id') is supplied through the parameter.
--> In order to free up this space for a future record, we set our table's meta-data freePage to the Page ID of the page whose record is being deleted.
--> To indicate that this record has been destroyed and is no longer required, we pin the page, browse to the data pointer of the record, and change the first character to '-'.
--> Lastly, we unpin the page after marking it as dirty to allow the buffer manager to store its data back to disk.

updateRecord(...)
--> In the table that the parameter "rel" references, this method modifies a record that is referenced by the parameter "record".
--> Using the meta-data from the table, it locates the record's page and pins it to the buffer pool.
--> It selects the Record ID and opens the data location for the record.
--> Using the memcpy() C method, we copy the record's data (provided through parameter "record") into the new record, mark the page as dirty, and then unpin the page.

getRecord(....)
--> In the table that "rel" references, which is also supplied in the argument, this function retrieves a record with Record ID "id" passed in the parameter. The location specified by the argument "record" is where the result record is kept.
--> It uses the table's meta-data to determine the record's page, and then it pins that page in the buffer pool using the record's 'id'.
--> It duplicates the data and sets the Record ID of the'record' parameter to the ID of the record that is present on the page.
--> The page is then unpinned.


SCAN FUNCTIONS
==============

startScan(...)
--> This function starts a scan by getting data from the RM_ScanHandle data structure which is passed as an argument to startScan() function.
--> We initialize our custom data structure's scan related variables.
--> If condition iS NULL, we return error code RC_SCAN_CONDITION_NOT_FOUND

next(...)
--> This function returns the next tuple which satisfies the condition (test expression).
--> If condition iS NULL, we return error code RC_SCAN_CONDITION_NOT_FOUND
--> If there are no tuples in the table, we return error code RC_RM_NO_MORE_TUPLES
--> We iterate through the tuples in the table. Pin the page having that tuple, navigate to the location where data is stored, copy data into a temporary buffer and then evaluate the test expression by calling eval(....)
--> If the result (v.boolV) of the test expression is TRUE, it means the tuple fulfills the condition. We then unpin the page and return RC_OK
--> If none of the tuples fulfill the condition, then we return error code RC_RM_NO_MORE_TUPLES

closeScan(...) 
--> This function ends the scanning process.
--> We examine the scanCount value in the table's metadata to see if the scan was comprehensive. It indicates an incomplete scan if it is greater than 0.
--> In the event that the scan was not completed, we reset all variables linked to the scan mechanism in our table's meta-data (custom data structure) and unpin the page.
.-> Next, we release (de-allocate) the space that the metadata was taking up.


SCHEMA FUNCTIONS
================

getRecordSize(...)
--> The record size in the given schema is returned by this function.
--> We cycle over the schema's characteristics. We add the size (space in bytes) needed for each attribute iteratively to the'size' variable. 
--> The record's size is the value of the variable "size."

freeSchema(...)
--> This function clears the memory of the schema indicated by the parameter "schema."
--> Each page frame's variable (field) refNum is used for this. refNum maintains track of the number of page frames that the client visits.
--> To eliminate the schema from the memory, we de-allocate the memory space it was using using the C function free(...).

createSchema(...)
--> This function uses the given parameters to generate a new schema in memory.
.-> The number of parameters is indicated by numAttr. The attributes' names are specified by attrNames. The datatype of the attributes is specified by datatypes. typeLength (e.g., length of STRING) defines the attribute's length.
--> We establish a schema object and provide it memory space. At last, we set the parameters of the schema to the ones that were supplied in the createSchema().


ATTRIBUTE FUNCTIONS
=========================================

createRecord(...)
--> This function creates a new record in the schema passed by parameter 'schema' and passes the new record to the 'record' paramater in the createRecord() function.
--> We allocate proper memory space to the new record. Also we give memory space for the data of the record which is the record size.
--> Also, we add a '-' to the first position and append '\0' which NULL in C. '-' denotes that this is a new blank record.
--> Finally, we assign this new record to the 'record' passed through the parameter.

attrOffset(...)
--> This function sets the offset (in bytes) into the'result' argument that is given through the function from the initial position to the specified attribute of the record.
--> Up to the designated attribute number, we loop through each of the schema's attributes. The size (space in bytes) needed for each property is successively added to the pointer *result.

getAttr(...)
--> This function retrieves an attribute from the given record in the specified schema.
--> The record, schema and attribute number whose data is to be retrieved is passed through the parameter. The attribute details are stored back to the location referenced by 'value' passed through the parameter.
--> We go to the location of the attribute using the attrOffset(...) function. We then depending on the datatype of the attribute, copy the attribute's datatype and value to the '*value' parameter.

setAttr(...)
--> In the designated schema, this function sets the attribute value in the record. The parameter passes through the record, schema, and attribute number whose data has to be retrieved.
--> The 'value' parameter passes the data to be stored in the attribute.
--> We use the attrOffset(...) function to get the attribute's location. Next, we copy the data in the '*value' parameter to the attributes' datatype and value, depending on the attribute's datatype.

freeRecord(...)
--> The memory space allotted to the'record' given by the parameter is released by this function.
--> To de-allocate (free up) the memory space utilized by the record, we utilize the C function free().