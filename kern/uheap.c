#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

uint32 pages=(USER_HEAP_MAX-USER_HEAP_START ) /PAGE_SIZE;
bool inUse [(USER_HEAP_MAX-USER_HEAP_START ) /PAGE_SIZE];

static int numberOfBlocks[(USER_HEAP_MAX-USER_HEAP_START ) /PAGE_SIZE];

struct allocations{
	uint32 startAddress;
	uint32 startIndex;
	uint32 noOfBlocks;
};
struct allocations allocations1[(USER_HEAP_MAX - USER_HEAP_START +1 ) / PAGE_SIZE];
uint32 allocationsCounter=0;

uint32 nextFit (uint32 size){

	uint32 currentBlocks=0;
	uint32 checkedPages=0;
	static uint32 i = 0;
	uint32 startIndex;
	static uint32 currentAddress = USER_HEAP_START;
	uint32 startAddress ;
	bool started=0;

	while(checkedPages<pages)
	{
		if (inUse[i]==0)
		{
			if(started==0)
			{
				startAddress=currentAddress;
				startIndex=i;
				started=1;
			}
			currentBlocks++;
			if(currentBlocks>=size)
			{
				//numberOfBlocks[startIndex]= currentBlocks;
				allocations1[allocationsCounter].startAddress=startAddress;
				allocations1[allocationsCounter].startIndex=startIndex;
				allocations1[allocationsCounter].noOfBlocks=currentBlocks;
				while (currentBlocks>0)
				{
					inUse[startIndex]=1;
					startIndex = (startIndex+1)%pages;
					currentBlocks--;
				}
				allocationsCounter++;
				currentAddress+=PAGE_SIZE;
				if(currentAddress==USER_HEAP_MAX)
					currentAddress=USER_HEAP_START;
				i = (i+1) % pages;
				return startAddress;
			}
		}
		else
		{
			currentBlocks=0;
			started=0;
		}

		checkedPages++;
		i = (i+1) % pages;
		currentAddress+=PAGE_SIZE;
		if(currentAddress==USER_HEAP_MAX)
			currentAddress=USER_HEAP_START;

	}
	return 0;
}





void* malloc(uint32 size)
{
	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyNEXTFIT() and
	//sys_isUHeapPlacementStrategyBESTFIT() for the bonus
	//to check the current strategy
	size = ROUNDUP(size,PAGE_SIZE);
	size = size / PAGE_SIZE;


	if(sys_isUHeapPlacementStrategyNEXTFIT())
	{
		uint32 startAddress;
		startAddress = nextFit(size);
		if(startAddress!=0)
		{
			sys_allocateMem(startAddress,size*PAGE_SIZE);
			return (void*)startAddress;
		}
	}


	return 0;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	int startIndex ;
	int size;
	int i = 0 ;
	for(; i < allocationsCounter; i++)
	{
		if(allocations1[i].startAddress==(uint32)virtual_address)
		{
			startIndex=allocations1[i].startIndex;
			size=allocations1[i].noOfBlocks;
		}

	}
	//int size = numberOfBlocks[startIndex];
	sys_freeMem((uint32)virtual_address,size*PAGE_SIZE);
	//numberOfBlocks[startIndex]=0;
	while(size>0)
	{
		inUse[startIndex]=0;
		startIndex = (startIndex+1) % pages;
		size--;
	}
	allocations1[i].startAddress=0;
	allocations1[i].startIndex=0;
	allocations1[i].noOfBlocks=0;
	//allocationsCounter--;
	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
	return;
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
