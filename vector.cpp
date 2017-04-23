
#include "vector.h"

namespace mystl
{

	/*比较操作*/

	template<typename T, typename Alloc>
	inline bool vector<T, Alloc>::operator==(const vector& rhs) const
	{

	}

	template<typename T, typename Alloc>
	bool vector<T, Alloc>::operator<(const vector& rhs) const
	{

	}

	template<typename T, typename Alloc>
	bool vector<T, Alloc>::operator!=(const vector& rhs) const
	{

	}

	template<typename T, typename Alloc>
	bool vector<T, Alloc>::operator>(const vector& rhs) const
	{

	}

	template<typename T, typename Alloc>
	bool vector<T, Alloc>::operator<=(const vector& rhs) const
	{

	}

	template<typename T, typename Alloc>
	bool vector<T, Alloc>::operator>=(const vector& rhs) const
	{

	}


	/*size、capacity*/
	template<typename T, typename Alloc>
	void vector<T, Alloc>::resize(size_type n, value_type val = value_type())
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::reserve(size_type n)
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::shrink_to_fit()
	{

	}

	/*操作vector*/
	template<typename T, typename Alloc>
	void vector<T, Alloc>::clear()
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::swap(vector& rhs)
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::push_back(const value_type& value)
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::pop_back()
	{

	}

	template<typename T, typename Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& value)
	{

	}

	template<typename T, typename Alloc>
	void vector<T, Alloc>::insert(iterator position, const size_type& n, const value_type& value)
	{
	}

	template<typename T, typename Alloc>
	template <typename InputIterator>
	void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
	{

	}

	template<typename T, typename Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
	{

	}

	template<typename T, typename Alloc>
	typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
	{

	}
}