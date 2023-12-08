#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

enum actionType
{
  cacheToProcessor,
  processorToCache,
  memoryToCache,
  cacheToMemory,
  cacheToNowhere
};

typedef struct blockStruct
{
  int data[MAX_BLOCK_SIZE];
  bool isDirty;
  int lruLabel;
  int set;
  int tag;
} blockStruct;

typedef struct cacheStruct
{
  blockStruct blocks[MAX_CACHE_SIZE];
  int blocksPerSet;
  int blockSize;
  int lru;
  int numSets;
  int numBlockOffsetBits;
  int numSetOffsetBits;
} cacheStruct;

cacheStruct cache;

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

stateType state;

int convertNum(int);
int getOpcode(int pc);
int getRegA(int pc);
int getRegB(int pc);
int getOffset(int pc);
int getDestReg(int pc);
void printAction(int address, int size, enum actionType type);
void printCache(void);
int load(int addr); 
void store(int addr, int data); 

int main(int argc, char *argv[]){
    char line[MAXLINELENGTH];
    FILE *filePtr;
    
    int opcode = 0, regA = 0, regB = 0, destReg = 0, offset = 0, totalInstructions = 0;
    
    if (argc != 5) {
        printf("error: usage: %s <machine-code file> <blockSizeInWords> <numberOfSets> <blocksPerSet>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }
    cache.blockSize = atoi(argv[2]);
    cache.numSets = atoi(argv[3]);
    cache.blocksPerSet = atoi(argv[4]);
    cache.numBlockOffsetBits = log2(cache.blockSize);
    cache.numSetOffsetBits = log2(cache.numSets);
    
    for (int i = 0; i < MAX_CACHE_SIZE; i++){
        cache.blocks[i].tag = -1;
        int x = cache.blocksPerSet - 1 - (i % cache.blocksPerSet);
        cache.blocks[i].lruLabel = x;
    }

    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
    }
    
    for (int i = 0; i < 8; i++){
        state.reg[i] = 0;
    }
    state.pc = 0;
    
    while (1){
        totalInstructions++;
        load(state.pc);
        opcode = getOpcode(state.pc);
        //lw
        if (opcode >= 0 && opcode <= 5){
            regA = getRegA(state.pc);
            regB = getRegB(state.pc);
        }
        if (opcode == 0 || opcode == 1){
            destReg = getDestReg(state.pc);
        }
        if (opcode >= 2 && opcode <= 4){
            offset = getOffset(state.pc);
        }
        
        if (opcode == 0){ // add
            state.reg[destReg] = state.reg[regA] + state.reg[regB];
            state.pc++;
        }else if (opcode == 1){ // nor
            state.reg[destReg] = ~(state.reg[regA] | state.reg[regB]);
            state.pc++;
        }else if (opcode == 2){ // lw
            int memAddress = offset + state.reg[regA];
            state.reg[regB] = load(memAddress);
            state.pc++;
        }else if (opcode == 3){ // sw
            int memAddress = offset + state.reg[regA];
            store(memAddress, state.reg[regB]);
            state.pc++;
        }else if (opcode == 4){ // beq
            if (state.reg[regA] == state.reg[regB]){
                state.pc = state.pc + 1 + offset;
            }else {
                state.pc++;
            }
        }else if (opcode == 5){ // jalr
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];
        }else if (opcode == 6){
            state.pc++;
            break;
        }else if (opcode == 7){
            state.pc++;
        }
    }

    return(0);
}

int getOpcode(int pc){
    int instruction = state.mem[pc];
    int opcode = instruction >> 22;
    return opcode;
}

int getRegA(int pc){
    int instruction = state.mem[pc];
    int mask = 7 << 19;
    int maskedInstruction = instruction & mask;
    int regA = maskedInstruction >> 19;
    return regA;
}

int getRegB(int pc){
    int instruction = state.mem[pc];
    int mask = 7 << 16;
    int maskedInstruction = instruction & mask;
    int regB = maskedInstruction >> 16;
    return regB;
}

int getDestReg(int pc){
    int instruction = state.mem[pc];
    int mask = 7;
    int destReg = instruction & mask;
    return destReg;
}

int getOffset(int pc){
    int instruction = state.mem[pc];
    int mask = 0xFFFF;
    int offset = instruction & mask;
    offset = convertNum(offset);
    return offset;
}

int convertNum(int num){
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}

/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type){
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);

    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    }
    else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    }
    else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    }
    else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    }
    else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}

void updateLrus(int setStart, int blockThatWasJustUsedStartPosition){
    int i = blockThatWasJustUsedStartPosition;
    int lruLabel = cache.blocks[i].lruLabel;
    cache.blocks[i].lruLabel = 0;
    for (int j = setStart; j < setStart + cache.blocksPerSet; j++){
        if (j != i && cache.blocks[j].lruLabel < lruLabel){
            cache.blocks[j].lruLabel++;
        }
    }
}

int getSetOffset(int addr){
    int mask = 0;
    for (int i = 0; i < cache.numSetOffsetBits; i++){
        mask = mask * 2 + 1;
    }
    int setOffset = addr >> cache.numBlockOffsetBits;
    return setOffset & mask;
}

int getBlockOffset(int addr){
    int mask = 0;
    for (int i = 0; i < cache.numBlockOffsetBits; i++){
        mask = mask * 2 + 1;
    }
    return addr & mask;
}

int findBlockWithMatchingTag(int setStartPosition, int tag){
    for (int i = setStartPosition; i < setStartPosition + cache.blocksPerSet; i++){
        if (cache.blocks[i].tag == tag){
            return i;
        }
    }

    return -1;
}

int findLeastRecentlyUsedBlock(int setStartPosition){
    for (int i = setStartPosition; i < setStartPosition + cache.blocksPerSet; i++){
        if (cache.blocks[i].lruLabel == cache.blocksPerSet - 1){
            return i;
        }
    }
    
    return -1;
}

void evictBlockToMemory(int blockStartPosition){
    int memAddress = (cache.blocks[blockStartPosition].tag << (cache.numSetOffsetBits + cache.numBlockOffsetBits)) + (cache.blocks[blockStartPosition].set << cache.numBlockOffsetBits);
    if (cache.blocks[blockStartPosition].isDirty){
        for (int j = 0; j < cache.blockSize; j++){
            state.mem[memAddress + j] = cache.blocks[blockStartPosition].data[j];
        }
        printAction(memAddress, cache.blockSize, cacheToMemory);
        cache.blocks[blockStartPosition].isDirty = false;
    }else {
        if (cache.blocks[blockStartPosition].tag != -1){
            printAction(memAddress, cache.blockSize, cacheToNowhere);
        }
    }
}

void transferBlockFromMemoryToCache(int leastRecentlyUsedBlockStartPosition, int addr, int tag, int setOffset){
    int memoryBlockStartPosition = addr - (addr % cache.blockSize);
    printAction(memoryBlockStartPosition, cache.blockSize, memoryToCache);
    for (int j = 0; j < cache.blockSize; j++){
        cache.blocks[leastRecentlyUsedBlockStartPosition].data[j] = state.mem[memoryBlockStartPosition + j];
    }
    cache.blocks[leastRecentlyUsedBlockStartPosition].tag = tag;
    cache.blocks[leastRecentlyUsedBlockStartPosition].set = setOffset;
}

int cacheAccess(int addr){
    int setOffset, tag;
    setOffset = getSetOffset(addr);
    tag = addr >> (cache.numBlockOffsetBits + cache.numSetOffsetBits);
    
    int setStartPosition = setOffset * cache.blocksPerSet;
    int blockStartPosition = findBlockWithMatchingTag(setStartPosition, tag);
    bool hit = (blockStartPosition != -1) ? true : false;
    
    if (!hit){
        int leastRecentlyUsedBlockStartPosition = findLeastRecentlyUsedBlock(setStartPosition);
        evictBlockToMemory(leastRecentlyUsedBlockStartPosition);
        transferBlockFromMemoryToCache(leastRecentlyUsedBlockStartPosition, addr, tag, setOffset);
        updateLrus(setStartPosition, leastRecentlyUsedBlockStartPosition);
        blockStartPosition = leastRecentlyUsedBlockStartPosition;
    }
    updateLrus(setStartPosition, blockStartPosition);
    return blockStartPosition;
}

int load(int addr){
    int blockOffset = getBlockOffset(addr);
    int blockStartPosition = cacheAccess(addr);
    printAction(addr, 1, cacheToProcessor);
    return cache.blocks[blockStartPosition].data[blockOffset];
}

void store(int addr, int data){
    int blockOffset = getBlockOffset(addr);
    int blockStartPosition = cacheAccess(addr);
    printAction(addr, 1, processorToCache);
    cache.blocks[blockStartPosition].data[blockOffset] = data;
    cache.blocks[blockStartPosition].isDirty = true;
}
