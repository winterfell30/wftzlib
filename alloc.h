#pragma once

#if 0
#	include <new>
#	define __THROW_BAD_ALLOC throw std::bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#	include <iostream>
#	define __THROW_BAD_ALLOC std::cerr << "out of memery" << std::endl;exit(1)
#endif

namespace mystl
{
	//以下是一级配置器
	class __malloc_alloc_template
	{
	private:
		//以下函数用来处理内存不足的情况(outofmomery)
		using new_handler = void(*)();                         //C++11using定义别名(using还可以定义模板别名typedef不行)
		static void *oom_malloc(size_t);
		static void *oom_realloc(void *, size_t);
		static new_handler __malloc_alloc_oom_handler;           //指定的oom handler

	public:
		static void *alloccate(size_t);
		static void deallocate(void *p);
		static void *reallocate(void *, size_t old_sz, size_t new_sz);
		static new_handler set_malloc_handler(new_handler);      //指定自己的oom handler
	};

	//以下是二级配置器
    enum {__ALIGN = 8};                                          //小型区块的上调边界
    enum {__MAX_BYTES = 128};                                    //小型区块的上限
    enum {__NFREELISTS = __MAX_BYTES / __ALIGN};                 //free_list的个数
    class __default_alloc_template
    {
	private:
		union obj                                       //free_list的节点构造
		{
			union obj *free_list_link;
			char client_data[1];
		};
	private:
		static size_t ROUND_UP(size_t);                  //调整边界为8的倍数
		static obj *free_list[__NFREELISTS];    //定义16个free_list
		static size_t FREELIST_INDEX(size_t);            //根据区块大小决定freelist编号
		static void *refill(size_t n);                   //返回一个大小为n的对象
		static char *chunk_alloc(size_t, size_t&);          //配置可以容纳nobjs个大小为size的区块

		static char *start_free;                         //内存池起始位置
		static char *end_free;                           //内存池结束位置
		static size_t heap_size;

	public:
		static void *allocate(size_t);
		static void deallocate(void*, size_t);
		static void *reallocate(void*, size_t old_sz, size_t new_sz);
	};
}
