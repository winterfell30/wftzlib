#pragma once
#include "Iterator.h"
#include "type_traits.h"

//都要具备commit and rollback语意
namespace mystl
{
	template<typename ForwardIterator, typename Size, typename T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
	}

	template<typename InputIterator, typename ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
	{

	}

	template<typename ForwardIterator, typename T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
	{

	}


}