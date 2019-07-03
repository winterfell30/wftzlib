#pragma once

#include <new>
#include "alloc.h"
#include "construct.h"

namespace mystl
{
	template<typename T>
	class allocator
	{
	public:
		typedef T         value_type;
		typedef T*        pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
	public:
		static T *allocate();
		static T *allocate(size_t n);
		static void deallocate(T *ptr);
		static void deallocate(T *ptr, size_t n);

		static void construct(T *ptr);
		static void construct(T *ptr, const T& value);
		static void destroy(T *ptr);
		static void destroy(T *first, T *last);
	};

	template<typename T>
	T *allocator<T>::allocate()
	{
		return static_cast<T*>(__default_alloc_template::allocate(sizeof(T)));
	}

	template<typename T>
	T *allocator<T>::allocate(size_t n)
	{
		return static_cast<T*>(__default_alloc_template::allocate(sizeof(T) * n));
	}

	template<typename T>
	void allocator<T>::deallocate(T *ptr)
	{
		__default_alloc_template::deallocate(static_cast<void*>(ptr), sizeof(T));
	}

	template<typename T>
	void allocator<T>::deallocate(T *ptr, size_t n)
	{
		if (n == 0) return;
		__default_alloc_template::deallocate(static_cast<void*>(ptr), sizeof(T) * n);
	}

	template<typename T>
	void allocator<T>::construct(T *ptr)
	{
		new(ptr), T();
	}

	template<typename T>
	void allocator<T>::construct(T *ptr, const T& value)
	{
		new(ptr), T(value);
	}

	template<typename T>
	void allocator<T>::destroy(T *ptr)
	{
		mystl::destroy(ptr);
	}

	template<typename T>
	void allocator<T>::destroy(T *first, T *last)
	{
		mystl::destroy(first, last);
	}
}
