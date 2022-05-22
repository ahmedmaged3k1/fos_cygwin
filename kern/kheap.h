#ifndef FOS_KERN_KHEAP_H_
#define FOS_KERN_KHEAP_H_

#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

//Values for user heap placement strategy
#define KHP_PLACE_CONTALLOC 0x0
#define KHP_PLACE_FIRSTFIT 	0x1
#define KHP_PLACE_BESTFIT 	0x2
#define KHP_PLACE_NEXTFIT 	0x3
#define KHP_PLACE_WORSTFIT 	0x4


void* kmalloc(unsigned int size);
void kfree(void* virtual_address);
void unMapingFrameFromStruct(int addressIndex , int status);
void display();
void initialization(int index,int pagesRequired , uint32 startingAddress,int status);
void unintialization(int index );
void nextFitAllocation(uint32 startingAddress , int pagesRequired );
void* nextFitHeapAllocation(uint32 startingAddress , int frameNumber,int size );
void allocation(uint32 address,int frameNumber);
unsigned int kheap_virtual_address(unsigned int physical_address);
unsigned int kheap_physical_address(unsigned int virtual_address);
unsigned int convert_physical_address(unsigned int virtual_address);
void copyKheapIntoStruct();
int freePlacesNextFit(int pagesRequired,uint32 startAddress);
uint32 bestFitFreePlaces(int pagesRequired);
void bestFitAllocation(uint32 startingAddress , int pagesRequired );
int numOfKheapVACalls ;

#endif // FOS_KERN_KHEAP_H_
