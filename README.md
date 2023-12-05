# CDA-cache-simulator
1. Purpose

The purpose of this project is to teach you about cache design and how a caching processor generates and services address references.

2. Problem

In this project, you will simulate a CPU cache (unified instruction/data) and integrate the cache into the Project 1 simulator.  As the processor
simulator executes a program, it will access instructions and data.  These accesses will be serviced by the cache, which will transfer data to/from 
memory as needed.

3. Cache Simulator

The central part of this project is to write a function that implements a flexible cache simulator.  The cache function will be used by the processor
simulation when the processor accesses addresses.  Your cache function should be able to simulate a variety of cache configurations.  Once integrated into your processor simulator, the program will be run as follows:

    simulate program.mc blockSizeInWords numberOfSets blocksPerSet

Your cache function should simulate a cache with the following characteristics:

    1) WRITE POLICY: write-back

    2) ASSOCIATIVITY: varies according to the "blocksPerSet" parameter.
        Associativity ranges from 1 (direct-mapped) to the number of blocks
        in the cache (fully associative).

    3) SIZE: the total number of words in the cache is
        blockSizeInWords * numberOfSets * blocksPerSet

    4) BLOCK SIZE: varies according to the "blockSizeInWords" parameter.  All
        transfers between the cache and memory take place in units of a single
        block.

    5) PLACEMENT/REPLACEMENT POLICY: when looking for a block within a set to
        replace, the best block to replace is an invalid (empty) block.  If
        there is none of these, replace the least-recently used block.

Make sure the units of each parameter are as specified.  Note that the smallest data granularity in this project is a word, because this is the
data granularity of the LC3100 architecture.  blockSizeInWords, numberOfSets, and blocksPerSet should all be powers of two.  To simplify your program, you may assume that the maximum number of cache blocks is 256 (this small number allows you to use a 2-dimensional array for the cache data structure).

4. Origin and Servicing of Address References

As the processor executes an assembly-language program, it accesses addresses. The three sources of address references are instruction fetch, lw, and sw. When the program starts up, it will initialize the memory with the machine-code file as in Project 1.  These initialization references should NOT be passed to the cache simulator; they should simply set the initial memory state.

Each address reference should be passed to the cache simulator.  The cache simulator keeps track of what blocks are currently in the cache and what state they are in (e.g. dirty, valid, etc.).  To service the address reference, the cache simulator may need to write back a dirty cache block to memory, then it may need to read a block into the cache from memory.  After these possible steps, the cache simulator should return the data to the processor (for read accesses) or write the data to the cache (for write accesses).  Each of these data transfers will be logged by calling the printAction function (make sure any code you write to replace this prints the same format):

enum actionType
        {cacheToProcessor, processorToCache, memoryToCache, cacheToMemory,
        cacheToNowhere};
/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *     cacheToProcessor: reading data from the cache to the processor
 *     processorToCache: writing data from the processor to the cache
 *     memoryToCache: reading data from the memory to the cache
 *     cacheToMemory: evicting cache data by writing it to the memory
 *     cacheToNowhere: evicting cache data by throwing it away
 */
void
printAction(int address, int size, enum actionType type)
{
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);
    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    } else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    } else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    } else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    } else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}

5. Test Cases

An integral (and graded) part of writing your cache simulator will be to write a suite of test cases to validate any LC3100 cache simulator.  This
is common practice in the real world--software companies maintain a suite of test cases for their programs and use this suite to check the program's correctness after a change.  Writing a comprehensive suite of test cases will deepen your understanding of the project specification and your program, and it will help you a lot as you debug your program.

The test cases for this project will be short assembly-language programs that, after being assembled into machine code, serve as input to a simulator.  You will submit your suite of test cases together with your simulator, and we will
grade your test suite according to how thoroughly it exercises an LC3100 cache simulator.  Each test case may generate at most 200 printAction statements on a correct simulator, and your test suite may contain up to 20 test cases. These limits are much larger than needed for full credit.

Each test case will specify the cache parameters to use when running the test case.  These parameters will be communicated via the name of the test case file.  Each test case should have a 3-part suffix, where each part identifies a cache parameter and the parts are separated by periods:

    anyName.<blockSizeInWords>.<numberOfSets>.<blocksPerSet>

For example, the test case in Section 8 would be named "test.as.4.2.1".  The combination of cache parameters should be legal (e.g. they should all be powers of two; numberOfSets and blocksPerSet should not exceed 256).  The instructor buggy simulators will not have error-checking bugs. See Section 6 for how your test suite will be graded.

Writing good test cases for this project will require careful planning.  Think about what different types of behavior a cache can exhibit and generate test cases that cause the cache to exhibit each behavior.  Think about how to test the various algorithms used in the cache simulator, e.g. LRU, writebacks, read and write hits, read and write misses.  As you write the code for your simulator, keep notes on different conditions you think of, and write test cases to test those conditions.

6. Grading and Formatting

Your program will be tested on a set of (hidden) test cases that range from  simple to complex.  You will receive points for each test case that is correctly processed.  The points awarded will be 75% of the total for the assignment.
You will also submit a set of test cases that test different aspects of your cache implementation.  We will evaluate the completeness of your test cases by running them on our simulator that contains common errors to see if your test cases identify the errors.  The points awarded for the ability of your test cases to identify common errors will be 25% of the total points for the assignment.  

7. Turning in the Project

   Turn in the source code and test cases on canvas.

8. Sample Assembly-Language Program and Output

Here is a simple, sample assembly-language program:

        sw      0       1       6
        lw      0       1       23
        lw      0       1       30
        halt

and its corresponding output with the following cache parameters:

blockSizeInWords = 4, numberOfSets = 2, and blocksPerSet = 1.  

Make sure you understand each of the data transfers and their order.

@@@ transferring word [0-3] from the memory to the cache
@@@ transferring word [0-0] from the cache to the processor
@@@ transferring word [4-7] from the memory to the cache
@@@ transferring word [6-6] from the processor to the cache
@@@ transferring word [1-1] from the cache to the processor
@@@ transferring word [4-7] from the cache to the memory
@@@ transferring word [20-23] from the memory to the cache
@@@ transferring word [23-23] from the cache to the processor
@@@ transferring word [2-2] from the cache to the processor
@@@ transferring word [20-23] from the cache to nowhere
@@@ transferring word [28-31] from the memory to the cache
@@@ transferring word [30-30] from the cache to the processor
@@@ transferring word [3-3] from the cache to the processor
