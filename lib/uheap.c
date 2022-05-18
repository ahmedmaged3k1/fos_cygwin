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
int userHeapSizeInPages = ((USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE);
uint32 lastUsedAddress =USER_HEAP_START;
uint32 firstFreeAddress =USER_HEAP_START;
struct userHeapData{
	uint32 startAddress;
	int status;
	int size;
};
struct userHeapData userHeapArray[(USER_HEAP_MAX - USER_HEAP_START ) / PAGE_SIZE];
void copyUheapIntoStruct()
{
	int index = 0 ;
	for(uint32 address=USER_HEAP_START ;address<USER_HEAP_MAX;address+=PAGE_SIZE)
	{
		userHeapArray[index].startAddress=address;
		userHeapArray[index].status=0;
		userHeapArray[index].size=0;
		index++;
	}
}

int freePlacesNextFit(uint32 pagesRequired,uint32 startAddress)
 {
	 int accumlativeCounter = 0;
	 uint32 address = startAddress;
	 int index;
	 for(int i = 0 ; i < userHeapSizeInPages;i++)
	 {
		 if(address==USER_HEAP_MAX)
			 address=USER_HEAP_START;

		 index =((address-USER_HEAP_START)/PAGE_SIZE);
		 for(int j=0;j<pagesRequired;j++)
		 {
			 if( index < userHeapSizeInPages && userHeapArray[index].status==0)
				 accumlativeCounter++;
			 else
			 {
				 accumlativeCounter=0;
				 break ;
			 }
				index++;
		 }

		if(accumlativeCounter==pagesRequired)
		{
			firstFreeAddress=address;
			lastUsedAddress=address;
			return 1 ;
		}
		else
			address+=(PAGE_SIZE);
	 }

	 return 0;
 }

void nextFitAllocation(uint32 startingAddress , int pagesRequired )
{
	 uint32 address =startingAddress;
	 int index =((startingAddress-USER_HEAP_START)/PAGE_SIZE);
	 int cntr = 0 ;
	 userHeapArray[index].size=pagesRequired;
	 for(int i=0;i<pagesRequired;i++)
	 {
		 userHeapArray[index].status=1;
		 userHeapArray[index].size=pagesRequired;
		lastUsedAddress+=PAGE_SIZE;
		if(lastUsedAddress==USER_HEAP_MAX)
			lastUsedAddress=USER_HEAP_START;

		index=(index+1)%userHeapSizeInPages;
		cntr++;
	 }

}
///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


int firstmalloc=1;
void* malloc(uint32 size)
{

	 if(sys_isUHeapPlacementStrategyNEXTFIT())
	 {
		 if(firstmalloc==1)
		 {
			 copyUheapIntoStruct();
			 firstmalloc=0;
		 }
		 if(lastUsedAddress==USER_HEAP_MAX)
			 lastUsedAddress=USER_HEAP_START;
		 int frameNumbers ;
		 size=ROUNDUP((uint32)size,PAGE_SIZE);
		 frameNumbers=size/PAGE_SIZE;

		 int freePlaces = freePlacesNextFit( frameNumbers, lastUsedAddress);
		 if(freePlaces==1)
			 nextFitAllocation(firstFreeAddress,frameNumbers);
		 else
			 return 0;
		 uint32 returnAddress = firstFreeAddress;
		 sys_allocateMem(returnAddress,frameNumbers);
		 return (void*)returnAddress;
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
	virtual_address=(uint32*)ROUNDDOWN((uint32)virtual_address,PAGE_SIZE);
	uint32 startAddress = (uint32)virtual_address;
	int ctr =0 ;
	bool found = 0;
	int j = 0;
	int index=-1;
	/*while(1)
	{
		if(userHeapArray[j].startAddress==startAddress)
		{
			index = j;
			found=1;
			break;
		}
		j++;
		if(j==userHeapSizeInPages)
			break;
	}*/
	index=(((int32)startAddress-USER_HEAP_START)/PAGE_SIZE);

	int size = userHeapArray[index].size;
	int loopIndex = index;
	if(userHeapArray[loopIndex].status==0)return;
	for(int i = 0;i<size;i++)
	{

				userHeapArray[loopIndex].status=0;
				userHeapArray[loopIndex].size=0;
				loopIndex++;
				if(loopIndex==userHeapSizeInPages)
				{
					cprintf("Reached Max\n");
				break;
				}

	}
	sys_freeMem((uint32)virtual_address,size);
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
