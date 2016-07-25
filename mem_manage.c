#include "board.h"
#include "mem_manage.h"

uint8_t heap_mem_size[MEM_SIZE];
static Mem_Header* end_mem_header;
static Mem_Header* start_mem_header; 



void mem_init(void)
{
	//��ʼ������ͷ
	start_mem_header = (Mem_Header*)heap_mem_size;
	((Mem_Header*)heap_mem_size)->magic = MEM_MAGIC;
	((Mem_Header*)heap_mem_size)->used = 0;
	((Mem_Header*)heap_mem_size)->prev = NULL;
	((Mem_Header*)heap_mem_size)->next = (uint32_t*)(heap_mem_size + MEM_SIZE - sizeof(Mem_Header));
	//��ʼ������β
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
			//�������������Ԫ�ص��ڴ�ռ���ڱ�������Ŀռ��Ԫ��ͷ�Ŀռ䣬��������������Ԫ���м������һ��Ԫ��
			//����ռ䲻����ֻ�ǽ���ǰԪ�ر�ʶΪ��ʹ�ã�����Ҫ�ı�����
			if((uint8_t*)(ptr->next)-(uint8_t*)ptr>size+2*sizeof(Mem_Header))
			{
				//�������Ԫ��
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
	//�ж���һ���Ƿ�ʹ��
	if((((Mem_Header*)(mem_header->prev))->used == 0)&&(mem_header != start_mem_header))
	{
	  temp_mem_header = (Mem_Header*)(mem_header->prev);
	  temp_mem_header->next = mem_header->next;
	  ((Mem_Header*)(mem_header->next))->prev = (uint32_t*)temp_mem_header;
		mem_header = (Mem_Header*)(mem_header->prev);
	}
	//�ж���һ���Ƿ�ʹ��
	if(((Mem_Header*)(mem_header->next))->used == 0)
	{
		temp_mem_header = (Mem_Header*)(mem_header->next);
		mem_header->next = temp_mem_header->next;
		((Mem_Header*)(temp_mem_header->next))->prev = (uint32_t*)mem_header;
	}
	mem_header->used = 0;
	return OK_MEM;
}



