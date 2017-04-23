#pragma once

#include "allocator.h"
#include "uninitialized.h"

#include <initializer_list>

namespace mystl
{
	template<typename T, typename Alloc = allocator<T> >
	class vector
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = value_type*;
		using const_iterator = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		//还未加入reverse_iterator
		
	protected:
		iterator start;
		iterator finish;
		iterator end_of_storage;
		using data_allocator = Alloc;

	public:

		/*构造函数 拷贝构造函数 赋值函数 析构函数*/

		vector():
			start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

		explicit vector(size_type n) 
		{
			fill_initialize(n, value_type());
		}

		vector(size_type n, const T& value) 
		{
			fill_initialize(n, value);
		}

		template<typename InputIterator>
		vector(InputIterator first, InputIterator last) 
		{
			vector_aux(first, last, is_integral<InputIterator>::value());          //参数是两个int时判断是迭代器还是size
		}

		vector(std::initializer_list<T> l)                  //{}默认构造初始化列表
		{
			copy_initialize(l.begin(), l.end());
		}

		vector(const vector& rhs) noexcept 
		{
			copy_initialize(rhs.start, rhs.finish);
		}

		vector(vector&& rhs) noexcept:
			start(rhs.start), finish(rhs.finish), end_of_storage(rhs.end_of_storage)
		{
			rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
		}

		vector& operator=(const vector& rhs) noexcept
		{
			if (this != &rhs)
			{
				copy_initialize(rhs.start, rhs.finish);
			}
			return *this;
		}

		vector& operator=(vector&& rhs) noexcept
		{
			if (this != &rhs)
			{
				destroy_and_deallocte();
				start = rhs.start;
				finish = rhs.finish;
				end_of_storage = rhs.end_of_storage;
				rhs.start = rhs.finish = rhs.end_of_storage = nullptr;
			}
			return *this;
		}

		vector& operator=(std::initializer_list<T> l) noexcept
		{
			destroy_and_deallocte();
			copy_initialize(l.begin(), l.end());
		}

		~vector()
		{
			destroy_and_deallocte();
		}


		/*比较操作*/

		inline bool operator==(const vector& rhs) const;

		inline bool operator<(const vector& rhs) const;

		inline bool operator!=(const vector& rhs) const;

		inline bool operator>(const vector& rhs) const;

		inline bool operator<=(const vector& rhs) const;

		inline bool operator>=(const vector& rhs) const;

		/*迭代器接口*/

		iterator begin() { return start; }

		const_iterator cbegin() const { return start; }

		iterator end() { return finish; }

		const_iterator cend() const { return finish; }

		//待加入reverse_iterator

		/*元素接口*/

		reference operator[](const difference_type i) { return *(begin() + i); }

		const_reference operator[](const difference_type i) const { return *(begin() + i); }

		reference front() { return *(begin()); }

		reference back() { return *(end() - 1); }

		/*size、capacity*/

		difference_type size() const { return finish - start; }

		difference_type capacity() const { return end_of_storage - start; }

		bool empty() const { return start == finish; }

		void resize(size_type n, value_type val = value_type());

		void reserve(size_type n);

		void shrink_to_fit();

		/*操作vector*/

		void clear();

		void swap(vector& rhs);

		void push_back(const value_type& value);

		void pop_back();

		iterator insert(iterator position, const value_type& value);

		void insert(iterator position, const size_type& n, const value_type& value);

		template <typename InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last);

		iterator erase(iterator position);

		iterator erase(iterator first, iterator last);

		Alloc get_allocator() { return data_allocator; }

	protected:

		void destroy_and_deallocte()
		{
			if (capacity() != 0)
			{
				data_allocator::destroy(start, finish);
				if (start)
					data_allocator::deallocate(start, end_of_storage - start);
			}
		}

		iterator allocate_and_fill(size_t n, const T& value)
		{
			iterator result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, value);
			return result;
		}

		template<typename InputIterator>
		iterator allocate_and_copy(InputIterator first, InputIterator last)
		{
			iterator result = data_allocator::allocate(last - first);
			uninitialized_copy(first, last, result);
			return result;
		}

		template<typename InputIterator>
		void copy_initialize(InputIterator first, InputIterator last)
		{
			destroy_and_deallocte();
			start = allocate_and_copy(first, last);
			finish = start + (last - first);
			end_of_storage = finish;
		}

		void fill_initialize(size_t n, const T& value)
		{
			destroy_and_deallocte();
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

		//因为参数个数相同，迭代器用两个int的时候会被编译器误会，所以用is_integral判断是迭代器还是n
		template<typename InputIterator>
		void vector_aux(InputIterator first, InputIterator last, __false_type)
		{
			copy_initialize(first, last);
		}

		void vector_aux(size_t n, const T& value, __true_type)
		{
			fill_initialize(n, value);
		}

	};
}