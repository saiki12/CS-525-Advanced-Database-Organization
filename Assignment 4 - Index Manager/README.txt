-------------Index Manager (B+ tree) CS-525 Assignment No.4 Group 31-------------

EXECUTING THE PROGERAM:
=======================
Step 1: Open Terminal (Linux/Unix), Command Prompt or PowerShell (Windows).

Step 2: Navigate to the Assignment 4 directory.

Step 3: Type command 'make' and hit enter. (Files are complied and ready to be executed)

Step 4: Type command 'make exec1', this will execute test_assign4.c file

Step 5: Type command 'make exec2', this will execute test_expr.c file

Step 6: Type command 'make clean', for cleaning up the compiled files (.o)

INDEX MANAGER FUNCTIONS:
========================

- initIndexManager(...) and shutdownIndexManager(...)
These are used to initialize and shut down index manager. Especially shutdownIndexManager() is also used to deallocate the resources.

B+ TREE FUNCTIONS:
==================

- createBtree() - It is used to create B+ tree index.
- openBtree() - It is used to open B+ tree index.
- closeBtree() - It is used to close B+ tree index.
- deleteBtree() - It is used to delete B+ tree index. It also removes the corresponding page.

DEBUG FUNCTION:
===============

- printTree() - It is used to create string representation of B+ tree. It is used for debugging. 

KEY FUNCTIONS:
==============

- findKey() - It is used to find the key in the B+ tree. It returns RID for the entry along with the search key.
- insertKey() - It is used to insert a new key and record pointer pair into the index.
- deleteKey() - It is used to remove the key and the corresponding record pointer from the index.
- openTreeScan(), closeTreeScan() - These functions are used to scan all the entries of B+ tree.

CONTRIBUTIONS:
==============

1) Saikiran Somanagoudar (20%)
-> Create, Destroy, Open, and Close a btree index functions
- createBtree(...)
- openBtree(...)
- closeBtree(...)
- deleteBtree(...)
-> Debug and Test functions
- printTree(...)

2) Syed Arfath (20%)
-> Index Access information functions
- findKey(...)
- insertKey(...)
- deleteKey(...)

3) Usha Devaraju (20%)
-> Initiating and Shutdown related functions
- initIndexManager(...);
- shutdownIndexManager(...);

4) Sai Dhanush Soma (20%)
-> Accessing information about btree functions
- getNumNodes(...)
- getNumEntries(...)
- getKeyType(...)

5) Haritha Chennuru Venugopal Reddy (20%)
-> Tree scan related functions
- openTreeScan(...)
- nextEntry(...)
- closeTreeScan(...)