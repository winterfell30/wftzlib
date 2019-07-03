#pragma once

#include "Iterator.h"
#include "construct.h"
#include "algorithm.h"
#include "type_traits.h"

//都要具备commit and rollback语意
namespace mystl
{
	//fill_n
	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
	{
		return mystl::fill_n(first, n, x);
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, T&& x, __true_type)
	{
		return mystl::fill_n(first, n, std::forward<T>(x));
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; n > 0; --n, first++)
				construct(&*cur, x);
			return cur;
		}
		catch (...)               //rollback
		{
			destroy(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, T&& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; n > 0; --n, first++)
				construct(&*cur, std::forward<T>(x));
			return cur;
		}
		catch (...)               //rollback
		{
			destroy(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		typedef typename __type_traits<T>::is_POD_type is_POD;
		return __uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, T&& x)
	{
		typedef typename __type_traits<T>::is_POD_type is_POD;
		return __uninitialized_fill_n_aux(first, n, std::forward<T>(x), is_POD());
	}

	//copy
	template<typename InputIterator, typename ForwardIterator>
	ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
	{
		return mystl::copy(first, last, result);
	}

	template<typename InputIterator, typename ForwardIterator>
	ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
	{
		ForwardIterator cur = result;
		try
		{
			for (; first != last; ++first, ++cur)
				construct(&*cur, std::move(*first));
		}
		catch (...)
		{
			destroy(result, cur);
			throw;
		}
		return cur;
	}


	template<typename InputIterator, typename ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{
		typedef typename __type_traits<iterator_traits<InputIterator>::value_type>::is_POD_type is_POD;
		return __uninitialized_copy_aux(first, last, result, is_POD());
	}

	//fill
	template<typename ForwardIterator, typename T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
	{
		mystl::fill(first, last, x);
	}

	template<typename ForwardIterator, typename T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; cur != last; ++cur)
				construct(&*cur, x);
		}
		catch (...)
		{
			destroy(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename T>
	void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, T&& x, __false_type)
	{
		ForwardIterator cur = first;
		try
		{
			for (; cur != last; ++cur)
				construct(&*cur, std::forward<T>(x));
		}
		catch (...)
		{
			destroy(first, cur);
			throw;
		}
	}

	template<typename ForwardIterator, typename T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
	{
		typedef typename __type_traits<T>::is_POD_type is_POD;
		__uninitialized_fill_aux(first, last, x, is_POD());
	}
}