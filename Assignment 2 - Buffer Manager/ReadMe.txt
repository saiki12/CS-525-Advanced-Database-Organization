EXECUTION INSTRUCTIONS:
=======================

1) In the Terminal, type "make clean" to delete any old compiled residual .o files
2) Run the "make test" command to compile all project files including "test_assign2_1.c" file
3) Type "make exec_test1" to run "test_assign2_1.c" file

1. BUFFER FRAME FUNCTIONS DESCRIPTION:
======================================

createBufferFrame(...)
-> This function creates new buffer frame for the buffer pool, invoked by initBufferPool(...) function.
-> The mgmtData field is used to store the data pertaining to the buffer frame.

replaceBufferFrame(...)
-> This function is used to replace buffer frame when the frame is not in use by the clients (the fixCount field is set to 0), invoked by both LRU (algoLRU(...)) & FIFO (algoFIFO(...)) algorithms.
-> The parameter inputFrames is used to insert new buffer frames into the buffer pool.

containBufferFrame(...)
-> This function acts as a bool parameter which is used to determine whether the requested page frame is present in the buffer pool.

2. BUFFER POOL FUNCTIONS DESCRIPTION:
=====================================

For an already-existing page file on disk, a buffer pool is created using the functions pertaining to buffer pools. To operate on the page file on disk, we used Storage Manager (from Assignment 1).

initBufferPool(...)
-> This function creates a new buffer pool in memory.
-> The parameter numPages defines the size of the buffer i.e. number of page frames that can be stored in the buffer.
-> pageFileName stores the name of the page file whose pages are being cached in memory.
-> strategy represents the page replacement strategy (FIFO, LRU) that will be used by this buffer pool
-> stratData field is used to pass parameters if any to the page replacement strategy. 

shutdownBufferPool(...)
-> This function destroys the buffer pool.
-> It frees up all resources/memory space being used by the Buffer Manager for the buffer pool.
-> Before destroying the buffer pool, we call forceFlushPool(...) which writes all the dirty pages (modified pages) to the disk.
-> If any page is being used by any client, then it throws RC_PINNED_PAGES_IN_BUFFER error.

forceFlushPool(...)
-> This function writes all the dirty pages (modified pages whose isDirty = 1) to the disk.
-> It checks all the page frames in buffer pool and checks if it's isDirty = 1 (which indicates that content of the page frame has been modified by some client) and fixCount = 0 (which indicates no user is using that page Frame) and if both conditions are satisfied then it writes the page frame to the page file on disk.


3. PAGE MANAGEMENT FUNCTIONS DESCRIPTION:
=========================================

The page management related functions are used to load pages from disk into the buffer pool (pin pages), remove a page frame from buffer pool (unpin page), mark page as dirty and force a page fram to be written to the disk.

pinPage(...)
-> This function pins the page number pageNum i.e, it reads the page from the page file present on disk and stores it in the buffer pool.
-> Before pinning a page, it checks if the buffer pool ha an empty space. If it has an empty space, then the page frame can be stored in the buffer pool else a page replacement strategy has to be used in order to replace a page in the buffer pool.
-> We have implemented FIFO, and LRU page replacement strategies which are used while pinning a page.
-> The page replacement algorithms determine which page has to be replaced. That respective page is checked if it is dirty. In case it's isDirty = 1, then the contents of the page frame is written to the page file on disk and the new page is placed at that location where the old page was.

unpinPage(...)
-> This function unpins the specified page. The page to be unpinned is decided using page's pageNum.
-> After locating the page using a loop, it decrements the fixCount of that page by 1 which means that the client is no longer using this page.

markDirty(...)
-> This function set's the isDirty of the specified page frame to 1.
-> It locates the page frame through pageNum by iteratively checking each page in the buffer pool and when the page id founf it set's isDirty = 1 for that page.

forcePage(....)
-> This page writes the content of the specified page frame to the page file present on disk.
-> It locates the specified page using pageNum by checking all the pages in the buffer loop using a loop construct.
-> When the page is found, it uses the Storage Manager functions to write the content of the page frame to the page file on disk. After writing, it sets isDirty = 0 for that page.


4. PAGE REPLACEMENT ALGORITHMS DESCRIPTION:
===========================================

The FIFO and LRU algorithms used when pinning a page are implemented via the page replacement strategy functions. A page should be replaced from the buffer pool if the buffer pool is full and a new page needs to be pinned. The page that has to be replaced from the buffer pool is determined by these page replacement algorithms.

algoFIFO(...)
-> First In First Out (FIFO) is the most basic page replacement strategy used.
-> FIFO is generally like a queue where the page which comes first in the buffer pool is in front and that page will be replaced first if the buffer pool is full.
-> Once the page is located, we write the content of the page frame to the page file on disk and then add the new page at that location.
-> The field seqNum is used to keep track of pages that are in the queue.

algoLRU(...)
-> Least Recently Used (LRU) removes the page frame which hasn't been used for a long time (least recent) amongst the other page frames in the buffer pool.
-> The field timeUsage is used to determine the number of times the page has been 
-> We then write the content of the page frame to the page file on disk and then add the new page at that location.

5. STATISTICS FUNCTIONS DESCRIPTION:
====================================

Information on the buffer pool is gathered using functions linked to statistics. As a result, it offers a variety of statistical data regarding the buffer pool.

getFrameContents(...)
-> This function returns an array of PageNumbers. The array size = buffer size (numPages).
-> Iterating over all the page frames in the buffer pool to get the pageNum value of the page frames present in the buffer pool.

getDirtyFlags(...)
-> This function returns an array of bools. The array size = buffer size (numPages).
-> We iterate over all the page frames in the buffer pool to get the isDirty value of the page frames present in the buffer pool.

getFixCounts(...) 
-> This function returns an array of ints. The array size = buffer size (numPages).
-> We iterate over all the page frames in the buffer pool to get the fixCount value of the page frames present in the buffer pool.

getNumReadIO(...)
-> This function returns the count of total number of IO reads performed by the buffer pool i.e. number of pages read from the disk.

getNumWriteIO(...)
-> This function returns the count of total number of IO writes performed by the buffer pool i.e. number of pages written to the disk.
