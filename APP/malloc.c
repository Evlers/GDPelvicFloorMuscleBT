#include "malloc.h"
#include "stdio.h"

Malloc_TypeDef mem_dev;
MallocErrorCb Error;

void malloc_Init(MallocErrorCb ErrorCB){
	mymem_set(mem_dev.Mem_Map,0,MAX_ALLOC_TABLE_SIZE);//内存管理表初始化
	mymem_set(mem_dev.Mem_Base,0,MAX_ALLOC_TABLE_SIZE);//内存池初始化
	mem_dev.Init_Flag = 1;//初始化完成标志
	Error = ErrorCB;
}

//内存设置
void mymem_set(u8 *map_add,u8 num,u32 len)
{
	u8 *s = map_add;
	while(len--)
		*s++ = num;
}

//内存复制
void mymen_copy(u8 *a1,u8 *a2,u32 len){
	u8 *des = a1;
	u8 *src = a2;
	while(len--)
		*des++ = *src++;
}

//内存使用率
float mem_rate(void){
	float rate;
	u32 i;
	rate = 0;
	for(i=0;i<MAX_ALLOC_TABLE_SIZE;i++){
		if(mem_dev.Mem_Map[i])rate++;
	}
	return (rate*100.0)/(float)MAX_ALLOC_TABLE_SIZE;
}

void *Malloc(u32 len){
	u32 i,offset;
	u32 nmemb; //需要的内存块
	u32 cmemb=0; //找到的内存块
	if(!mem_dev.Init_Flag){malloc_Init(NULL);}//调用初始化
	if(len == 0)return ((void *)0);//不需要分配
	nmemb = len/MEM_BLOCK_SIZE;
	if(len%MEM_BLOCK_SIZE)nmemb++;
	INTX_DISABLE();
	for(offset=MAX_ALLOC_TABLE_SIZE-1;offset>0;offset--){
		if(mem_dev.Mem_Map[offset] == 0) cmemb++;
		else cmemb = 0;
		if(cmemb == nmemb){//找到连续内存
			for(i=0;i<nmemb;i++){
				mem_dev.Mem_Map[i+offset]=nmemb;
			}
			return ((void *)((offset*MEM_BLOCK_SIZE)+(u32)(mem_dev.Mem_Base)));//返回变量地址
		}
	}
	INTX_ENABLE();
	if(Error)
		Error();
	return (void *)0;//没有找到
}

void Free(void *ptr){
	u32 Mem_addr;
	u32 i,offset;
	u32 Bsize;
	offset = (u32)ptr;
	offset -= (u32)&mem_dev.Mem_Base;
	INTX_DISABLE();
	if(offset < MAX_MEM_RAM_SIZE){
		Mem_addr = offset/MEM_BLOCK_SIZE;//块地址
		Bsize = mem_dev.Mem_Map[Mem_addr];//块数量
		for(i=0;i<Bsize;i++){
			mem_dev.Mem_Map[i+Mem_addr] = 0;
		}
	}
	INTX_ENABLE();
}
