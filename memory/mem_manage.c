#include "board.h"
#include "mem_manage.h"

uint8_t heap_mem_size[MEM_SIZE];
static Mem_Header* end_mem_header;
static Mem_Header* start_mem_header; 



void mem_init(void)
{
	//初始化链表头
	start_mem_header = (Mem_Header*)heap_mem_size;
	((Mem_Header*)heap_mem_size)->magic = MEM_MAGIC;
	((Mem_Header*)heap_mem_size)->used = 0;
	((Mem_Header*)heap_mem_size)->prev = NULL;
	((Mem_Header*)heap_mem_size)->next = (uint32_t*)(heap_mem_size + MEM_SIZE - sizeof(Mem_Header));
	//初始化链表尾
	end_mem_header = (Mem_Header*)(heap_mem_size + MEM_SIZE - sizeof(Mem_Header));
	end_mem_header->magic = MEM_MAGIC;
	end_mem_header->used = 1;
	end_mem_header->prev = (uint32_t*)(heap_mem_size);
	end_mem_header->next = NULL;
}

void* mem_malloc(uint32_t size)
{
	Mem_Header* mem_header;
	Mem_Header* ptr;
	for(ptr = start_mem_header;ptr<end_mem_header;ptr=(Mem_Header*)(ptr->next))
	{
		if((ptr->used == 0)&&(((uint8_t*)(ptr->next)-(uint8_t*)ptr)>(size+sizeof(Mem_Header))))
		{
			//如果链表间的两个元素的内存空间大于本次申请的空间和元素头的空间，则在链表这连个元素中间再添加一个元素
			//如果空间不够则只是将当前元素标识为已使用，不需要改变链表
			if((uint8_t*)(ptr->next)-(uint8_t*)ptr>size+2*sizeof(Mem_Header))
			{
				//添加链表元素
				mem_header = (Mem_Header*)((uint8_t*)ptr + size + sizeof(Mem_Header));
				mem_header->magic = MEM_MAGIC;
				mem_header->used = 0;
				mem_header->prev = (uint32_t*)ptr;
				mem_header->next = ptr->next;
				((Mem_Header*)(ptr->next))->prev = (uint32_t*)mem_header;
				ptr->next = (uint32_t*)mem_header;
				ptr->used = 1;
				return ((uint8_t*)ptr + sizeof(Mem_Header));
			}
			else
			{
				ptr->used = 1;
				return ((uint8_t*)ptr + sizeof(Mem_Header));
			}
		}
	}
	return NULL;
}

uint8_t mem_free(uint8_t* mem)
{
	Mem_Header* mem_header;
	Mem_Header* temp_mem_header;
	if(mem == NULL) return NO_MEM;
	mem_header = (Mem_Header*)(mem - sizeof(Mem_Header));
	if(mem_header->magic != MEM_MAGIC) return ERR_MEM;
	if(mem_header == end_mem_header) return END_MEM;
	//判断上一块是否被使用
	if((((Mem_Header*)(mem_header->prev))->used == 0)&&(mem_header != start_mem_header))
	{
	  temp_mem_header = (Mem_Header*)(mem_header->prev);
	  temp_mem_header->next = mem_header->next;
	  ((Mem_Header*)(mem_header->next))->prev = (uint32_t*)temp_mem_header;
		mem_header = (Mem_Header*)(mem_header->prev);
	}
	//判断下一块是否被使用
	if(((Mem_Header*)(mem_header->next))->used == 0)
	{
		temp_mem_header = (Mem_Header*)(mem_header->next);
		mem_header->next = temp_mem_header->next;
		((Mem_Header*)(temp_mem_header->next))->prev = (uint32_t*)mem_header;
	}
	mem_header->used = 0;
	return OK_MEM;
}



