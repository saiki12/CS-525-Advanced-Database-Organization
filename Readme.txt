--------Storage Manager CS - 525 Assignment No.1 Group 25-------------
Saikiran Somanagoudar - A20542890
Sai Dhanush Soma - A20545628
Arafath Syed - A20550937
Usha Devaraju - A20539949
Haritha Chennuru Venugopal Reddy - A20531012

Contributions:
1) Saikiran Somanagoudar (20%)
-> Function Implementations
	- readBlock()
	- getBlockPos()
	- readFirstBlock()
	- readPreviousBlock()

2) Sai Dhanush Soma (20%)
-> Function Implementations
	- readCurrentBlock()
	- readNextBlock()
	- readLastBlock()

3) Arafath Syed (20%)
-> Function Implementations
	- writeBlock()
	- writeCurrentBlock()
	- appendEmptyBlock()

4) Usha Devaraju (20%)
-> Function Implementations
	- openPageFile()
	- ensureCapacity()

5) Haritha Chennuru Venugopal Reddy (20%)
-> Function Implementations
	- createPageFile()
	- closePageFile()
	- destroyPageFile()

----------------------------------------------------

Executing Instructions :

Step 1: Enter the make command in linux terminal - $ make
Step 2: test_assign1_1 file consist the main function, therefore to run the whole program enter the command - $ ./test_assign1_1


------------------------------------------------------

THE FIRST PAGE IN THE FILE IS THE CURRENT PAGE POSITION WHEN THE FILE IS OPENED, or CURRENT PAGE POSITION = 0. AND THE FILE SIZE AFFECTS THE TOTAL NUMBER OF PAGES.

--------------------------------------------------------

Function Description :

A) createPageFile(): This function creates a new page file with the parameters PAGE_SIZE, 1 page, and 0 bytes.
      - The file is created using the fopen() method.
      - In order to add '0' bytes into the file, we open it in write mode.

b) openPageFile(): - Confirms the input file's existence.
      - If the file is there, it is opened; if not, the error code RC_FILE_NOT_FOUND is returned.
      - The total number of pages can be calculated by moving the file pointer to the end and dividing the result by the page size. 
      - The file name, the total number of pages, and the current page position are then updated in the file handle.
      
c) closePageFile(): This function closes an open page file.
      - If the file closes properly, RC_OK is returned; if not, RC_FILE_NOT_FOUND.

d) destroyPageFile(): - Destroys a currently active page file.
      - We are using the remove function to remove the file.
      - It returns RC_OK if the file is successfully removed; if not, it returns RC_FILE_NOT_FOUND. 

------------------------------------------------------------------------------------
2. Reading functions :

a) readBlock(): 
      - This function determines whether the total number of pages does not exceed the page number (pageNum) from which to start reading.
      - Throws an error if it is bigger.
      - If not, we shall move the pointer to the page we need to read.
      followed by reading the block.
      - After reading a page, we set the current page position to that page's number.

b) getBlockPos(): This function uses the data structure object fHandle to return the current page position in the page file.

c) readFirstBlock(): Verifies whether the values are present in the fHandle data structure.
      - If it works, the total number of pages is next checked to see if it is larger than 0.
      - If, then read the file's first block.
      - And sets the current page position to first block i.e. 0.

d) readPreviousBlock(): Verifies whether the values are present in the data structure fHandle.
      - If so, move the pointer to the spot on the preceding page.
      - Throws an error if the previous page position is outside of the allowed range.
      - Alternatively, reading the file's previous block.
      Additionally, the previous block is assigned as the current page position.

e) readCurrentBlock(): Verifies whether the values are present in the fHandle data structure.
      - If so, calling the getBlockPos() function to obtain the current page position.
      reading the page in the file that is now open.
      - After that, setting the current block's page position.

f) readNextBlock(): Verifies whether the values are present in the fHandle data structure.
      - If so, move the pointer to the appropriate spot on the following page.
      - Reading the next block of the file if the following block is not out of bounds.
      
g) readLastBlock():
      - Verifies whether the values are present in the fHandle data structure.
      - If so, move the cursor to the last position on the page.
      - reading the file's final section.
      - adding the final block to the current page position.

-------------------------------------------------------------------------------------------------------------

3. Writing Functions

a) writeBlock(): 
      - Verifies that the page number to be written on is not zero or greater than the total number of pages.
      - If not, it is then determined whether the data structure fHandle contains the values.
      - If it is Null, an error is thrown.
      - If not, we shall move the pointer to the page where we must type.
      - After which the block is written at page number(pageNum).
      - After that, we set the current page position to the newly entered page number.
      - The overall number of pages is then updated.

b) writeCurrentBlock(): 
      - First, we call getBlockPos() to obtain the current page position.
      – After which the writeBlock() method is invoked with the current location passed as one of its arguments.
      - If current block is successfully written then we return RC_OK.
      - Else we return RC_WRITE_FAILED.

c) appendEmptyBlock(): 
      - Verifies whether the values are present in the fHandle data structure or not.
      - If it does, we build a PAGE_SIZE buffer.
      - After which we write an empty block there and move the file pointer position to the end.
      - After which the total number of pages and current page position are updated.
    
d) ensureCapacity(): 
      - In the first step of the ensureCapacity() function, we determine whether the total number of pages is fewer than the number of pages (numberOfPages) specified in the function argument.
      - If it is fewer, we calculate how many pages need to be added.
      - after which we execute a loop and use the appendEmptyBlock() function to generate the remaining number of pages.
