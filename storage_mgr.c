#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dberror.h"
#include "storage_mgr.h"


FILE *fp_25; //File pointer

void initStorageManager(void) {
	printf("\n--------------- Buffer Manager Implementation - Group 25 -----------------\n ");
	printf("\n");
	printf("\tSaikiran Somanagoudar (A20542890) \n");
	printf("\tSai Dhanush Soma (A20545628) \n");
	printf("\tArafath Syed (A20550937) \n");
    printf("\tUsha Devaraju (A20539949) \n");
    printf("\tHaritha Chennuru Venugopal Reddy (A20531012) \n\n");

}

//Creating a Page in the file
RC createPageFile(char *fileName) {
	fp_25 = fopen(fileName, "w+"); //Opening a file in the write mode
	char *memBlock = (char*) calloc(PAGE_SIZE * sizeof(char), 1); //Memory block initialized with size PAGE_SIZE

	if (fp_25 == 0)
		return RC_FILE_NOT_FOUND;

	else {
		memset(memBlock, '\0', PAGE_SIZE); //Using memset function setting the allocated memory block by null (\0) character, if the file exists
		fwrite(memBlock, sizeof(char), PAGE_SIZE, fp_25);	//Writing the allocated memory block in the file
		free(memBlock);		//Freeing the allocated memory block after usage
		fclose(fp_25);			//Close file after creating it is done
		return RC_OK;
	}
}

//Opening a Page in the file
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
	fp_25 = fopen(fileName, "r+");		//Opening the file fileName in read mode

	if (fp_25 == 0)
		return RC_FILE_NOT_FOUND;

	else {
		fseek(fp_25, 0, SEEK_END);//fseek will point the file pointer to the last location of the file
		int lastByte = ftell(fp_25); 		//ftell returns the current byte position, here its the lastByte position
		int fullLength = lastByte + 1;
		int totNumPages = fullLength / PAGE_SIZE; //Retrieves total number of pages in the file

		(*fHandle).fileName = fileName; //Initializing the file attributes
		(*fHandle).totalNumPages = totNumPages;
		(*fHandle).curPagePos = 0;
		rewind(fp_25); //rewind sets the file pointer to beginning of the file
		return RC_OK;
	}
}

//Reads pageNum block in the file
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC read_size;

	if ((*fHandle).totalNumPages < pageNum)	//Checks condition if the total no. of pages exceeds pageNum, it throws an error
		return RC_READ_NON_EXISTING_PAGE;

	else {
		fseek(fp_25, pageNum * PAGE_SIZE, SEEK_SET);
		read_size = fread(memPage, sizeof(char), PAGE_SIZE, fp_25);
		if (read_size < PAGE_SIZE || read_size > PAGE_SIZE) {//Checks condition if the block returned by fread() is beyond the Page size limit, it throws an error
			return RC_READ_NON_EXISTING_PAGE;
		}
		(*fHandle).curPagePos = pageNum;//Update current page position with pageNum value
		return RC_OK;
	}
}

//Get current block position
int getBlockPos(SM_FileHandle *fHandle) {
	return (*fHandle).curPagePos;	//et current page position
}

//Reads first block from the file
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;
	if (fHandle == NULL)
		return_method = RC_FILE_NOT_FOUND;

	else {
		if ((*fHandle).totalNumPages <= 0)
			return_method = RC_READ_NON_EXISTING_PAGE;
		else {
			RC first_block;
			fseek(fp_25, 0, SEEK_SET);
			first_block = fread(memPage, sizeof(char), PAGE_SIZE, fp_25); //fread returns the first block of size PAGE_SIZE
			(*fHandle).curPagePos = 0; //First page of the file has index 0

			if (first_block < 0 || first_block > PAGE_SIZE) //If the read block is beyond the page size limits, it throws an error
				return_method = RC_READ_NON_EXISTING_PAGE;

			return_method = RC_OK;
		}
	}
	return return_method;
}

//Reads previous block from the file 
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;

	if (fHandle == NULL)
		return_method = RC_FILE_NOT_FOUND;

	else {

		RC previous_block;
		RC prev_blockread;

		previous_block = (*fHandle).curPagePos - 1; //Storing previous block index

		if (previous_block < 0)
			return_method = RC_READ_NON_EXISTING_PAGE;

		else {
			fseek(fp_25, (previous_block * PAGE_SIZE), SEEK_SET); //fseek will point the file pointer to the start of the previous block
			prev_blockread = fread(memPage, sizeof(char), PAGE_SIZE, fp_25); //fread returns the read block
			(*fHandle).curPagePos = (*fHandle).curPagePos - 1; //Update the current page position reducing it by 1

			if (prev_blockread < 0 || prev_blockread > PAGE_SIZE)
				return_method = RC_READ_NON_EXISTING_PAGE;

			return_method = RC_OK;
		}
	}
	return return_method;
}

//Reads the current block which the file pointer points to
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;

	if (fHandle == NULL)	//Throws an error if the handle is null
		return_method = RC_FILE_HANDLE_NOT_INIT;

	else {
		RC cblock;     //Variable for current block
		RC cblockread;     // Variable current block read
		cblock = getBlockPos(fHandle); //Get current block position to read
		fseek(fp_25, (cblock * PAGE_SIZE), SEEK_SET);
		cblockread = fread(memPage, sizeof(char), PAGE_SIZE, fp_25); //fread returns the current block to be read
		(*fHandle).curPagePos = cblock;

		if (cblockread < 0 || cblockread > PAGE_SIZE)
			return_method = RC_READ_NON_EXISTING_PAGE;

		return_method = RC_OK;
	}
	return return_method;
}

//Reading next block of the file
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;

	if (fHandle == NULL)
		return_method = RC_FILE_HANDLE_NOT_INIT;

	else {
		RC nblock;   //Next block
		RC nblockread;    //Next block read

		nblock = (*fHandle).curPagePos + 1; //Get next block position
		if (nblock < (*fHandle).totalNumPages) {

			fseek(fp_25, (nblock * PAGE_SIZE), SEEK_SET); //fseek points the file pointer to the next block
			nblockread = fread(memPage, sizeof(char), PAGE_SIZE, fp_25);

			(*fHandle).curPagePos = nblock; //Update current page position to next block index

			if (nblockread < 0 || nblockread > PAGE_SIZE)
				return_method = RC_READ_NON_EXISTING_PAGE;

			return_method = RC_OK;
		} else
			return RC_READ_NON_EXISTING_PAGE;

	}
	return return_method;
}

//Reading the last block of the file
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;

	if (fHandle == NULL)
		return_method = RC_FILE_NOT_FOUND;

	else {
		RC find_lblock;    //Find last block
		RC read_lblock;    //Read last block

		find_lblock = (*fHandle).totalNumPages - 1; //Last block has index 1 less than total no. of pages, because the starting index is 0

		fseek(fp_25, (find_lblock * PAGE_SIZE), SEEK_SET);
		read_lblock = fread(memPage, sizeof(char), PAGE_SIZE, fp_25);

		(*fHandle).curPagePos = find_lblock; //Update current page position to the last block index

		if (read_lblock < 0 || read_lblock > PAGE_SIZE)
			return_method = RC_READ_NON_EXISTING_PAGE;

		return_method = RC_OK;
	}
	return return_method;
}

//Writing block at page pageNum
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC return_method;
	if (pageNum < 0 || pageNum > (*fHandle).totalNumPages) //If the pageNum is greater than totalNumber of pages, or less than zero, throw an error
		return_method = RC_WRITE_FAILED;

	else

	if (fHandle == NULL)
		return_method = RC_FILE_HANDLE_NOT_INIT;

	else {
		if (fp_25 != NULL) { //If there is no file, it throws an error
			if (fseek(fp_25, (PAGE_SIZE * pageNum), SEEK_SET) == 0) {
				fwrite(memPage, sizeof(char), PAGE_SIZE, fp_25);

				(*fHandle).curPagePos = pageNum; //Update current page position to the pageNum i.e. index of the block written

				fseek(fp_25, 0, SEEK_END);
				(*fHandle).totalNumPages = ftell(fp_25) / PAGE_SIZE; //Update the value of totalNumPages which increases by 1

				return_method = RC_OK;
			} else {
				return_method = RC_WRITE_FAILED;
			}
		} else {
			return_method = RC_FILE_NOT_FOUND;
		}
	}
	return return_method;

}

//Writing current block in the file
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
	RC c_position;
	c_position = getBlockPos(fHandle); //Retrieve current block position

	RC write_cblock;
	write_cblock = writeBlock(c_position, fHandle, memPage); //WriteBlock function call

	if (write_cblock == RC_OK)
		return RC_OK;

	else
		return RC_WRITE_FAILED;

}

//Append empty block to the file
RC appendEmptyBlock(SM_FileHandle *fHandle) {

	RC return_method;
	if (fp_25 != NULL) {
		RC size = 0;

		char *newemptyblock;
		newemptyblock = (char *) calloc(PAGE_SIZE, sizeof(char)); //Create and initialize empty block

		fseek(fp_25, 0, SEEK_END);
		size = fwrite(newemptyblock, 1, PAGE_SIZE, fp_25);

		if (size == PAGE_SIZE) {
			(*fHandle).totalNumPages = ftell(fp_25) / PAGE_SIZE; //Update the total number of pages, adding 1 to it
			(*fHandle).curPagePos = (*fHandle).totalNumPages - 1; //Reposition the page so that the index is 1 less than the total
			return_method = RC_OK;
		}

		else {
			return_method = RC_WRITE_FAILED;
		}

		free(newemptyblock);

	} else {
		return_method = RC_FILE_NOT_FOUND;
	}
	return return_method;
}

//Making sure the file's capacity is numberOfPages.
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {

	int pageNum = (*fHandle).totalNumPages;
	int i;

	if (numberOfPages > pageNum) { //In the event that the new capacity exceeds the existing capacity
		int add_page = numberOfPages - pageNum;
		for (i = 0; i < add_page; i++) //Calling the appendEmptyBlock method to add the specified number of pages
			appendEmptyBlock(fHandle);

		return RC_OK;
	}

	else
		return RC_WRITE_FAILED;
}

//Closing Page file
RC closePageFile(SM_FileHandle *fHandle) {
	RC isFileClosed;
	isFileClosed = fclose(fp_25); //File is closed successfully and the return code is 0
	return (isFileClosed == 0) ? RC_OK : RC_FILE_NOT_FOUND;		
}

//Destroying Page file
RC destroyPageFile(char *fileName) {
	return (remove(fileName) != 0) ? RC_FILE_NOT_FOUND : RC_OK; //If successful, remove will successfully erase the file fileName and return 0.
}