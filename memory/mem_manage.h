#include "board.h"

#ifndef __MEM_MANAGE_H__
#define __MEM_MANAGE_H__

#define MEM_SIZE      1024
#define MEM_MAGIC     0x1ea0

#define OK_MEM        0
#define NO_MEM        1
#define ERR_MEM       2
#define END_MEM       3

typedef struct mem_header
{
	uint16_t magic;
	uint16_t used;
	uint32_t* prev;
	uint32_t* next;
}Mem_Header;

void mem_init(void);
void* mem_malloc(uint32_t size);
uint8_t mem_free(uint8_t* mem);

#endif
