#pragma once

#include "Iterator.h"
#include "allocator.h"

namespace mystl
{
	//fill
	template<typename ForwardIterator, typename T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value)
	{
		for (; first != last; ++first)
			*first = value;
	}

	inline void fill(char *first, char *last, const char& value)
	{
		memset(first, static_cast<unsigned char>(value), last - first);
	}

	inline void fill(wchar_t *first, wchar_t *last, const wchar_t& value)
	{
		memset(first, static_cast<unsigned char>(value), (last - first) * sizeof(wchar_t));
	}

	//fill_n
	template<typename OutputIterator, typename Size, typename T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value)
	{
		for (; n > 0; --n, ++first)
			*first = value;
		return first;
	}

	template<typename Size>
	char *fill_n(char *first, Size n, const char& value)
	{
		memset(first, static_cast<unsigned char>(value), n);
		return first + n;
	}

	template<typename Size>
	wchar_t *fill_n(wchar_t *first, Size n, const wchar_t& value)
	{
		memset(first, static_cast<unsigned char>(value), n * sizeof(wchar_t));
		return first + n;
	}

	//copy

	template<typename T>
	inline T *__copy_t(const T *first, const T *last, T* result, __true_type)
	{
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	template<typename T>
	inline T *__copy_t(const T *first, const T *last, T *result, __false_type)
	{
		return __copy_d(first, last, result, (ptrdiff_t*)nullptr);
	}

	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag)
	{
		//以迭代器等同与否控制循环，速度慢
		for (; first != last; ++result, ++first)
			*result = *first;
		return result;
	}

	template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
	inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		//以n决定循环的执行次数，速度快
		for (Distance n = last - first; n > 0; --n, ++first, ++result)
			*result = *first;
		return result;
	}

	template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
	inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag)
	{
		return __copy_d(first, last, result, distance_type(first));
	}

	template<typename InputIterator, typename OutputIterator>
	struct __copy_dispatch
	{
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
		{
			return __copy(first, last, result, iterator_category(first));
		}
	};

	template<typename T>
	struct __copy_dispatch<T*, T*>
	{
		T* operator()(T *first, T *last, T *result)
		{
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	template<typename T>
	struct __copy_dispatch<const T*, T*>
	{
		T* operator()(const T *first, const T *last, T *result)
		{
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	inline char* copy(const char *first, const char *last, char *result)
	{
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* copy(const wchar_t *first, const wchar_t *last, wchar_t *result)
	{
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

}