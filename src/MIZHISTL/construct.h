#pragma once
#include "Iterator.h"
#include "type_traits.h"

namespace mystl
{
	template<typename T1, typename T2>
	inline void construct(T1 *p, const T2& value)
	{
		new(p) T1(value);
	}

	template<typename T1, typename T2>
	inline void construct(T1 *p, T2&& value)
	{
		new(p) T1(std::forward<T2>(value));
	}

	template<typename T>
	inline void destroy(T *pointer)
	{
		pointer->~T();
	}

	template<typename ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last)
	{
		__destroy(first, last, value_type(first));
	}

	template<typename ForwardIterator, typename T>
	inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
	{
		typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

	template<typename ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
	{
		for (; first < last; ++first)
			destroy(&*first);
	}

	template<typename ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

}
