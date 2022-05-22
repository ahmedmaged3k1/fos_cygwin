#include <inc/lib.h>

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

	for(int index = 0 ;index<userHeapSizeInPages;index++)
	{
		userHeapArray[index].startAddress=0;
		userHeapArray[index].status=0;
		userHeapArray[index].size=0;

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
			 if( userHeapArray[index].status==0)
			 {
				 accumlativeCounter++;
				 if(accumlativeCounter==pagesRequired)
					 break;
			 }

			 else
			 {
				 accumlativeCounter=0;
				 break ;
			 }
				index=(index+1)%userHeapSizeInPages;
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
	 for(int i=0;i<pagesRequired;i++)
	 {
		 userHeapArray[index].size=pagesRequired-i;
		 userHeapArray[index].status=1;
		 userHeapArray[index].startAddress=lastUsedAddress;
		lastUsedAddress+=PAGE_SIZE;
		if(lastUsedAddress==USER_HEAP_MAX)
			lastUsedAddress=USER_HEAP_START;
		index=(index+1)%userHeapSizeInPages;
		cntr++;
	 }

}
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

void free(void* virtual_address)
{
	virtual_address=(uint32*)ROUNDDOWN((uint32)virtual_address,PAGE_SIZE);
	uint32 startAddress = (uint32)virtual_address;
	int ctr =0 ;
	bool found = 0;
	int index= ((startAddress-USER_HEAP_START)/PAGE_SIZE);
	int frameNumbers = userHeapArray[index].size;
	sys_freeMem((uint32)startAddress,frameNumbers);
	for(int i = 0;i<frameNumbers;i++)
	{
				userHeapArray[index].startAddress=0;
				userHeapArray[index].status=0;
				userHeapArray[index].size=0;
				index=(index+1)%userHeapSizeInPages;
	}
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}



void *realloc(void *virtual_address, uint32 new_size)
{

	panic("realloc() is not implemented yet...!!");

	return NULL;
}
