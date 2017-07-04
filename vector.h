#pragma once

#include "allocator.h"
#include "uninitialized.h"

#include <initializer_list>

namespace mystl
{
	template<typename T, typename Alloc = mystl::allocator<T> >
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
			vector_aux(first, last, mystl::is_integral<InputIterator>::value());          //参数是两个int时判断是迭代器还是size
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
				mystl::copy_initialize(rhs.start, rhs.finish);
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
			mystl::copy_initialize(l.begin(), l.end());
		}

		~vector() noexcept
		{
			destroy_and_deallocte();
		}


		/*比较操作*/

		inline bool operator==(const vector& rhs) const
		{
			if (size() != rhs.size())
			{
				return false;
			}
			else
			{
				auto p1 = start;
				auto p2 = rhs.start;
				difference_type n = finish - start;
				while (n--)
				{
					if (*p1 != *p2)
						return false;
					p1++, p2++;
				}
				return true;
			}
		}

		inline bool operator!=(const vector& rhs) const
		{
			return !(*this == rhs);
		}


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

		/*操作vector*/

		void push_back(const value_type& value)
		{
			if (finish != end_of_storage)
			{
				construct(finish, value);
				++finish;
			}
			else
			{
				insert_aux(end(), value);
			}
		}

		void push_back(value_type&& value)
		{
			if (finish != end_of_storage)
			{
				//std::cout << 233 << std::endl;
				construct(finish, std::forward<value_type>(value));
				++finish;
			}
			else
			{
				//std::cout << "2333" << std::endl;
				insert_aux(end(), std::forward<value_type>(value));
			}
		}

		void pop_back()
		{
			--finish;
			destroy(finish);
		}

		iterator insert(iterator position, const value_type& value)
		{
			insert_aux(position, value);
		}

		void insert(iterator position, size_type n, const value_type& value)
		{
			insert_aux(position, n, value, mystl::is_integral<size_type>::value());
		}

		template <typename InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last)
		{
			insert_aux(position, first, last, mystl::is_integral<InputIterator>::value());
		}

		iterator erase(iterator position)
		{
			//后续元素向前移动覆盖被删除pos然后删除最后一个多余节点
			if (position + 1 != end())
				copy(position + 1, finish, position);
			--finish;
			destroy(finish);
			return position;
		}

		iterator erase(iterator first, iterator last)
		{
			iterator pos = copy(last, finish, first);
			destroy(pos, finish);
			finish -= (last - first);
			return first;
		}

		void swap(vector& rhs)
		{
			if (this != &rhs)
			{
				mystl::swap(start, rhs.start);
				mystl::swap(finish, rhs.finish);
				mystl::swap(end_of_storage, rhs.end_of_storage);
			}
		}

		void clear()
		{
			erase(begin(), end());
		}

		/*size、capacity*/

		difference_type size() const { return finish - start; }

		difference_type capacity() const { return end_of_storage - start; }

		bool empty() const { return start == finish; }

		void resize(size_type new_size, value_type val = value_type())
		{
			if (new_size < size())
			{
				erase(begin() + new_size, end());
			}
			else if (new_size > size() && new_size <= capacity())
			{
				finish = mystl::uninitialized_fill_n(finish, new_size - size(), val);
			}
			else if (new_size > capacity())
			{
				auto new_capacity = new_size;
				iterator new_start = data_allocator::allocate(new_capacity);
				iterator new_finish = mystl::uninitialized_copy(begin(), end(), new_start);
				new_finish = mystl::uninitialized_fill_n(new_start, new_capacity, val);

				destroy_and_deallocte();
				start = new_start;
				finish = new_finish;
				end_of_storage = new_finish;
			}
		}

		void reserve(size_type n)
		{
			if (n <= capacity())
				return;
			iterator new_start = data_allocator::allocate(n);
			iterator new_finish = mystl::uninitialized_copy(start, finish, new_start);
			destroy_and_deallocte();
			start = new_start;
			finish = new_finish;
			end_of_storage = start + n;
		}

		void shrink_to_fit()
		{
			data_allocator::deallocate(finish, end_of_storage);
			end_of_storage = finish;
		}

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
			mystl::uninitialized_fill_n(result, n, value);
			return result;
		}

		iterator allocate_and_fill(size_t n, T&& value)
		{
			iterator result = data_allocator::allocate(n);
			mystl::uninitialized_fill_n(result, n, std::forward<T>(value));
			return result;
		}

		template<typename InputIterator>
		iterator allocate_and_copy(InputIterator first, InputIterator last)
		{
			iterator result = data_allocator::allocate(last - first);
			mystl::uninitialized_copy(first, last, result);
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

		void fill_initialize(size_t n, T&& value)
		{
			destroy_and_deallocte();
			start = allocate_and_fill(n, std::forward<T>(value));
			finish = start + n;
			end_of_storage = finish;
		}

		void insert_aux(iterator position, const T& val)
		{
			if (finish != end_of_storage)
			{
				construct(finish, *(finish - 1));
				++finish;
				mystl::copy_backward(position, finish - 2, finish - 1);
				mystl::fill_n(position, 1, val);
			}
			else
			{
				const size_type old_size = size();
				const size_type new_size = old_size != 0 ? 2 * old_size : 1;

				iterator new_start = data_allocator::allocate(new_size);
				iterator new_finish = new_start;
				try
				{
					new_finish = mystl::uninitialized_copy(start, position, new_start);
					construct(new_finish, val);
					++new_finish;
					new_finish = mystl::uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					//commit or rollback
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, new_size);
					throw;
				}

				destroy(start, finish);
				data_allocator::deallocate(start, old_size);

				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + new_size;
			}
		}

		void insert_aux(iterator position, T&& val)
		{
			if (finish != end_of_storage)
			{
				construct(finish, std::move(*(finish - 1)));
				++finish;
				mystl::copy_backward(position, finish - 2, finish - 1);
				mystl::fill_n(position, 1, std::forward<T>(val));
			}
			else
			{
				//finish == end_of_storage 此时size() == capacity()
				const size_type old_size = size();
				const size_type new_size = old_size != 0 ? 2 * old_size : 1;

				iterator new_start = data_allocator::allocate(new_size);
				iterator new_finish = new_start;
				try
				{
					new_finish = mystl::uninitialized_copy(start, position, new_start);
					construct(new_finish, std::forward<T>(val));
					++new_finish;
					new_finish = mystl::uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					//commit or rollback
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, new_size);
					throw;
				}

				destroy(start, finish);
				data_allocator::deallocate(start, old_size);

				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + new_size;
			}
		}

		template<typename InputIterator>
		void insert_aux(iterator position, InputIterator first, InputIterator last, mystl::__false_type)
		{
		}

		void insert_aux(iterator position, size_t n, const T& value, mystl::__true_type)
		{
			std::cout << "test" << std::endl;
		}

		//因为参数个数相同，迭代器用两个int的时候会被编译器误会，所以用is_integral判断是迭代器还是n
		template<typename InputIterator>
		void vector_aux(InputIterator first, InputIterator last, mystl::__false_type)
		{
			copy_initialize(first, last);
		}

		void vector_aux(size_t n, const T& value, mystl::__true_type)
		{
			fill_initialize(n, value);
		}

	};
}