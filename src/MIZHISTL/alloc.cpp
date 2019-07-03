#include "alloc.h"

namespace mystl
{
	__malloc_alloc_template::new_handler __malloc_alloc_template::__malloc_alloc_oom_handler = nullptr;
	
	void *__malloc_alloc_template::oom_malloc(size_t n)
	{
		void *result = nullptr;
		for (;;)
		{
			auto my_malloc_handler = __malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr) {	__THROW_BAD_ALLOC; }
			my_malloc_handler();
			result = malloc(n);
			if (result) return (result);
		}
	}

	void *__malloc_alloc_template::oom_realloc(void *p, size_t n)
	{
		void *result = nullptr;
		for (;;)
		{
			auto my_malloc_handler = __malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr) { __THROW_BAD_ALLOC; }
			my_malloc_handler();
			result = realloc(p, n);
			if (result) return (result);
		}
	}

	void *__malloc_alloc_template::alloccate(size_t n)
	{
		//std::cout << "malloc" << std::endl;
		void *result = malloc(n);                       //一级配置器直接调用malloc
		if (result == nullptr) result = oom_malloc(n);  //调用失败调用oom_malloc
		return result;
	}

	void __malloc_alloc_template::deallocate(void *p)
	{
		free(p);
	}

	void *__malloc_alloc_template::reallocate(void *p, size_t old_sz, size_t new_sz)
	{
		void *result = realloc(p, new_sz);
		if (result == nullptr) result = oom_realloc(p, new_sz);
		return result;
	}

	__malloc_alloc_template::new_handler __malloc_alloc_template::set_malloc_handler(new_handler fun)
	{
		new_handler old_handler = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = fun;
		return old_handler;
	}

	size_t __default_alloc_template::ROUND_UP(size_t bytes)      //调整区块的上边界为8的倍数
	{
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));         //加7再取整
	}

	size_t __default_alloc_template::FREELIST_INDEX(size_t bytes) //根据区块大小决定index
	{
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}

	char *__default_alloc_template::start_free = nullptr;

	char *__default_alloc_template::end_free = nullptr;

	size_t __default_alloc_template::heap_size = 0;

	__default_alloc_template::obj *__default_alloc_template::free_list[__NFREELISTS] =
	{
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	};

	void *__default_alloc_template::allocate(size_t n)
	{
		obj *result; 
		if (n > (size_t)__MAX_BYTES)               //大于128时调用一级配置器
			return (__malloc_alloc_template::alloccate(n));
		//std::cout << "chunk" << std::endl;
		obj *my_free_list = free_list[FREELIST_INDEX(n)];
		result = my_free_list;
		if (result == nullptr)
			return refill(ROUND_UP(n));         //没找到可用的freelist，重新填充freelist
		my_free_list = result->free_list_link;
		return (result);
	}

	void __default_alloc_template::deallocate(void *p, size_t n)
	{
		if (n > (size_t)__MAX_BYTES)
			return __malloc_alloc_template::deallocate(p);
		obj *temp = reinterpret_cast<obj*>(p);
		size_t index = FREELIST_INDEX(n);
		temp->free_list_link = free_list[index];
		free_list[index] = temp;
	}

	void *__default_alloc_template::reallocate(void *p, size_t old_sz, size_t new_sz)
	{
		if (old_sz > 128 && new_sz > 128)
			return __malloc_alloc_template::reallocate(p, old_sz, new_sz);
		else
		{
			deallocate(p, old_sz);
			return allocate(new_sz);
		}
	}

	void *__default_alloc_template::refill(size_t n)
	{
	    size_t nobjs = 20;
		char *chunk = chunk_alloc(n, nobjs);
		if (nobjs == 1)
		{
			return static_cast<void*>(chunk);
		}
		else
		{
			void *result = static_cast<void*>(chunk);
			obj *my_free_list = free_list[FREELIST_INDEX(n)];
			for (size_t i = 1; i < nobjs; i++)
			{
				chunk += n;
				obj* temp = reinterpret_cast<obj*>(chunk);
				temp->free_list_link = my_free_list;
				my_free_list = temp;
			}
			return result;
		}
	}

	char *__default_alloc_template::chunk_alloc(size_t n, size_t& nobjs)
	{
		size_t need_bytes = nobjs * n;
		size_t remain_bytes = end_free - start_free;
		if (remain_bytes >= need_bytes)
		{
			char *result = start_free;
			start_free += need_bytes;
			return result;
		}
		else if (remain_bytes >= n)
		{
			nobjs = remain_bytes / n;
			need_bytes = n * nobjs;
			char *result = start_free;
			start_free += need_bytes;
			return result;
		}
		else
		{
			size_t bytes_to_get = 2 * need_bytes + ROUND_UP(heap_size >> 4);
			if (remain_bytes > 0)                 //内存池还有零头
			{
				obj **my_free_list = free_list + FREELIST_INDEX(remain_bytes);
				obj* t = reinterpret_cast<obj*>(start_free);
				t->free_list_link = *my_free_list;
				*my_free_list = t;
			}
			start_free = (char*)malloc(bytes_to_get);
			if (start_free == nullptr)
			{
				obj **my_free_list = nullptr, *p = nullptr;
				for (size_t i = n; i <= __MAX_BYTES; i += __ALIGN)
				{
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr)                 //freelist尚有未用区块
					{
						*my_free_list = p->free_list_link;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(n, nobjs);
					}
				}
				end_free = 0;
				start_free = (char*)__malloc_alloc_template::alloccate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(n, nobjs);
		}
	}
}