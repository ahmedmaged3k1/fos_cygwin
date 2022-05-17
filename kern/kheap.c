#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
int kHeapDataOption =0 ;
int numberOfIndicies=0;
int kernelHeapSize = ((KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE);
uint32 lastUsedAddress =KERNEL_HEAP_START;
uint32 firstFreeAddress =KERNEL_HEAP_START;
int kernelHeapIndex = 0;
 struct KernelHeapData
 {
 	uint32 virtualAddress;
 	uint32 physicalAddress;
 	uint32 presentBit ;
 	int size ;
 	int status ;
 	int frameNumbers;
 	struct Frame_Info* frame_info;
 };
struct KernelHeapData KernelHeapDataArray[((KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE)];
void* kmalloc(unsigned int size)
{
	 if(kHeapDataOption==0)
	 {
		 copyKheapIntoStruct();
		 kHeapDataOption=100 ;
	 }
	 if(lastUsedAddress==KERNEL_HEAP_MAX)
	 	 			 {
	 	 			 lastUsedAddress=KERNEL_HEAP_START;

	 	 			 }
	 if(KERNEL_HEAP_MAX-lastUsedAddress<size)return NULL;
  int frameNumbers ;
  size=ROUNDUP((uint32)size,PAGE_SIZE);
  frameNumbers=size/PAGE_SIZE;
  if(size%PAGE_SIZE!=0)frameNumbers++;

  int freePlaces = freePlacesNextFit( frameNumbers, lastUsedAddress);

  if(freePlaces==1)
  {
	   nextFitAllocation(firstFreeAddress,frameNumbers);

  }
  else
		   {
	   return NULL;
  }


  			uint32 returnAddress =(firstFreeAddress);


  return (void*)returnAddress;



}
int freePlacesNextFit(int pagesRequired,uint32 startAddress)
 {
	 int accumlativeCounter = 0;
	 int i =0;
	 uint32 address = startAddress ;
	 int option = 0 ;

	 int index =((address-KERNEL_HEAP_START)/PAGE_SIZE);
	 while(i<kernelHeapSize)
	 {
		 if(address==KERNEL_HEAP_MAX)
		 	 			 {
		 	 			 address=KERNEL_HEAP_START;
		 	 			 }
		 index =((address-KERNEL_HEAP_START)/PAGE_SIZE);
		 for(int j=0;j<pagesRequired;j++)
		 {

			 if( index < kernelHeapSize && KernelHeapDataArray[index].status==0)
				 {
					 accumlativeCounter++;

				 }

				 else
					 break ;

				index++;
		 }

			 if(accumlativeCounter==pagesRequired)
				 {

				 firstFreeAddress=address;
				 lastUsedAddress=firstFreeAddress;


				 return 1 ;
				 }


			 else{

				 address+=(PAGE_SIZE);
				accumlativeCounter=0;
			 }

		 i++;
	 }

	 return 0;

 }
void nextFitAllocation(uint32 startingAddress , int pagesRequired )
{
	 uint32 address =startingAddress;

	 int index =((startingAddress-KERNEL_HEAP_START)/PAGE_SIZE);
	 int cntr = 0 ;

	 for(int i=0;i<pagesRequired;i++)
	 {

		 struct Frame_Info *ptr_frame_info;
	     int res= allocate_frame(&ptr_frame_info);
		 if(res==E_NO_MEM)
			 {

			 return ;

			 }
	     res = map_frame(ptr_page_directory, ptr_frame_info, (void*)KernelHeapDataArray[index].virtualAddress,PERM_PRESENT|PERM_WRITEABLE);
		 if(res==E_NO_MEM)
					 {
						 free_frame(ptr_frame_info);

						  return  ;
					 }
					 KernelHeapDataArray[index].status=1;
						 KernelHeapDataArray[index].frameNumbers=pagesRequired;
						 KernelHeapDataArray[index].size=(pagesRequired*PAGE_SIZE);
						 lastUsedAddress+=PAGE_SIZE;
						 numberOfIndicies++;
						 index++;
						 cntr++;
	 }

}

void copyKheapIntoStruct()
{
	uint32 address =KERNEL_HEAP_START ;
	int index = 0 ;

	while(address<KERNEL_HEAP_MAX)
	{
		KernelHeapDataArray[index].virtualAddress=address;
		KernelHeapDataArray[index].status=0;
		index++;
		address+=PAGE_SIZE;

	}
	cprintf("Finished Initializing , Number Done is : %d \n",index);
}


void kfree(void* virtual_address)
{
	virtual_address=(uint32*)ROUNDDOWN((uint32)virtual_address,PAGE_SIZE);
	uint32 startAddress = (uint32)virtual_address;
	int index  = (((int32)startAddress-KERNEL_HEAP_START)/PAGE_SIZE);
	int ctr =0 ;

	int addresIndex  = (((int32)startAddress-KERNEL_HEAP_START)/PAGE_SIZE);
	for(int i = 0;i<KernelHeapDataArray[index].frameNumbers;i++)
	{


				unmap_frame(ptr_page_directory, (uint32 * )startAddress);
				KernelHeapDataArray[addresIndex].status=0;
				startAddress+=PAGE_SIZE;
				addresIndex++;
				ctr++;
	}
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	uint32 address =KERNEL_HEAP_START ;
				int index = 0 ;
				while(address<lastUsedAddress)
				{
					uint32 *pageTable ;
					struct Frame_Info *virtual_frame_info=get_frame_info(ptr_page_directory,(void * )address,&pageTable);

					if(virtual_frame_info!=NULL)
					{
						uint32 virtPhys = to_physical_address(virtual_frame_info);

						if(virtPhys==physical_address)
						{
							return address ;
						}
					}
					index++;
					address+=PAGE_SIZE;
				}

		 return  0;

		//change this "return" according to your answer
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	uint32 *pageTableEntry;
	get_page_table(ptr_page_directory,(int *)virtual_address,&pageTableEntry);
	if(pageTableEntry==NULL) return -1 ;
	uint32 pageTableNumber =PTX(virtual_address);
	uint32 offsett = (virtual_address & 0xFFF) ;
	uint32 physicalAddress = ((pageTableEntry[pageTableNumber]>>12)*PAGE_SIZE );
	if(physicalAddress!=0)
				{
					return physicalAddress;
				}
	return physicalAddress;
}

