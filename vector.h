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
		iterator start_;
		iterator finish_;
		iterator end_of_storage;
		using data_allocator = Alloc;

	public:

		/*构造函数 拷贝构造函数 赋值函数 析构函数*/

		vector():
			start_(nullptr), finish_(nullptr), end_of_storage(nullptr) {}

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

		vector(const vector& rhs)  
		{
			copy_initialize(rhs.start_, rhs.finish_);
		}

		vector(vector&& rhs) noexcept:
			start_(rhs.start_), finish_(rhs.finish_), end_of_storage(rhs.end_of_storage)
		{
			rhs.start_ = rhs.finish_ = rhs.end_of_storage = nullptr;
		}

		vector& operator=(const vector& rhs)
		{
			if (this != &rhs)
			{
				mystl::copy_initialize(rhs.start_, rhs.finish_);
			}
			return *this;
		}

		vector& operator=(vector&& rhs) noexcept
		{
			if (this != &rhs)
			{
				destroy_and_deallocte();
				start_ = rhs.start_;
				finish_ = rhs.finish_;
				end_of_storage = rhs.end_of_storage;
				rhs.start_ = rhs.finish_ = rhs.end_of_storage = nullptr;
			}
			return *this;
		}

		vector& operator=(std::initializer_list<T>& l) 
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
				auto p1 = start_;
				auto p2 = rhs.start_;
				difference_type n = finish_ - start_;
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

		iterator begin() { return start_; }

		const_iterator cbegin() const { return start_; }

		iterator end() { return finish_; }

		const_iterator cend() const { return finish_; }

		//待加入reverse_iterator

		/*元素接口*/

		reference operator[](const difference_type i) { return *(begin() + i); }

		const_reference operator[](const difference_type i) const { return *(begin() + i); }

		reference front() { return *(begin()); }

		reference back() { return *(end() - 1); }

		/*操作vector*/

		void push_back(const value_type& value)
		{
			if (finish_ != end_of_storage)
			{
				construct(finish_, value);
				++finish_;
			}
			else
			{
				insert_aux(end(), value);
			}
		}

		void push_back(value_type&& value) 
		{
			if (finish_ != end_of_storage)
			{
				construct(finish_, std::forward<value_type>(value));
				++finish_;
			}
			else
			{
				insert_aux(end(), std::forward<value_type>(value));
			}
		}

		void pop_back()
		{
			--finish_;
			destroy(finish_);
		}

		iterator insert(iterator position, const value_type& value)
		{
			insert_aux(position, value);
		}

		void insert(iterator position, size_type n, const value_type& value)
		{
			insert_aux(position, n, value, mystl::is_integral<size_type>::value());
		}

		//为了让编译器能区分出这个和上面的insert，用is_integral区分
		template <typename InputIterator>
		void insert(iterator position, InputIterator first, InputIterator last)
		{
			insert_aux(position, first, last, mystl::is_integral<InputIterator>::value());
		}

		iterator erase(iterator position)
		{
			//后续元素向前移动覆盖被删除pos然后删除最后一个多余节点
			if (position + 1 != end())
				copy(position + 1, finish_, position);
			--finish_;
			destroy(finish_);
			return position;
		}

		iterator erase(iterator first, iterator last)
		{
			iterator pos = copy(last, finish_, first);
			destroy(pos, finish_);
			finish_ -= (last - first);
			return first;
		}

		void swap(vector& rhs)
		{
			if (this != &rhs)
			{
				mystl::swap(start_, rhs.start_);
				mystl::swap(finish_, rhs.finish_);
				mystl::swap(end_of_storage, rhs.end_of_storage);
			}
		}

		void clear()
		{
			erase(begin(), end());
		}

		/*size、capacity*/

		difference_type size() const { return finish_ - start_; }

		difference_type capacity() const { return end_of_storage - start_; }

		bool empty() const { return start_ == finish_; }

		void resize(size_type new_size, value_type val = value_type())
		{
			if (new_size < size())
			{
				erase(begin() + new_size, end());
			}
			else if (new_size > size() && new_size <= capacity())
			{
				finish_ = mystl::uninitialized_fill_n(finish_, new_size - size(), val);
			}
			else if (new_size > capacity())
			{
				auto new_capacity = new_size;
				iterator new_start = data_allocator::allocate(new_capacity);
				iterator new_finish = mystl::uninitialized_copy(begin(), end(), new_start);
				new_finish = mystl::uninitialized_fill_n(new_start, new_capacity, val);

				destroy_and_deallocte();
				start_ = new_start;
				finish_ = new_finish;
				end_of_storage = new_finish;
			}
		}

		void reserve(size_type n)
		{
			if (n <= capacity())
				return;
			iterator new_start = data_allocator::allocate(n);
			iterator new_finish = mystl::uninitialized_copy(start_, finish_, new_start);
			destroy_and_deallocte();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage = start_ + n;
		}

		void shrink_to_fit()
		{
			data_allocator::deallocate(finish_, end_of_storage);
			end_of_storage = finish_;
		}

		Alloc get_allocator() { return data_allocator; }

	protected:

		void destroy_and_deallocte()
		{
			if (capacity() != 0)
			{
				data_allocator::destroy(start_, finish_);
				if (start_)
					data_allocator::deallocate(start_, end_of_storage - start_);
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
			start_ = allocate_and_copy(first, last);
			finish_ = start_ + (last - first);
			end_of_storage = finish_;
		}

		void fill_initialize(size_t n, const T& value)
		{
			destroy_and_deallocte();
			start_ = allocate_and_fill(n, value);
			finish_ = start_ + n;
			end_of_storage = finish_;
		}

		void fill_initialize(size_t n, T&& value)
		{
			destroy_and_deallocte();
			start_ = allocate_and_fill(n, std::forward<T>(value));
			finish_ = start_ + n;
			end_of_storage = finish_;
		}

		void insert_aux(iterator position, const T& val)
		{
			if (finish_ != end_of_storage)
			{
				construct(finish_, *(finish_ - 1));
				++finish_;
				mystl::copy_backward(position, finish_ - 2, finish_ - 1);
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
					new_finish = mystl::uninitialized_copy(start_, position, new_start);
					construct(new_finish, val);
					++new_finish;
					new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
				}
				catch (...)
				{
					//commit or rollback
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, new_size);
					throw;
				}

				destroy(start_, finish_);
				data_allocator::deallocate(start_, old_size);

				start_ = new_start;
				finish_ = new_finish;
				end_of_storage = new_start + new_size;
			}
		}

		void insert_aux(iterator position, T&& val)
		{
			if (finish_ != end_of_storage)
			{
				construct(finish_, std::move(*(finish_ - 1)));
				++finish_;
				mystl::copy_backward(position, finish_ - 2, finish_ - 1);
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
					new_finish = mystl::uninitialized_copy(start_, position, new_start);
					construct(new_finish, std::forward<T>(val));
					++new_finish;
					new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
				}
				catch (...)
				{
					//commit or rollback
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, new_size);
					throw;
				}

				destroy(start_, finish_);
				data_allocator::deallocate(start_, old_size);

				start_ = new_start;
				finish_ = new_finish;
				end_of_storage = new_start + new_size;
			}
		}

		template<typename InputIterator>
		void insert_aux(iterator position, InputIterator first, InputIterator last, mystl::__false_type)
		{
			difference_type num_of_insert = distance(first, last);
			difference_type num_of_left = end_of_storage - finish_;

			if (num_of_insert <= num_of_left)
			{
				if (position + num_of_insert < finish_)
				{
					mystl::uninitialized_copy(finish_ - num_of_insert, finish_, finish_);
					mystl::copy_backward(position, finish_ - num_of_insert, finish_);
					mystl::copy(first, last, start_);
				}
				else
				{
					mystl::uninitialized_copy(position, finish_, position + num_of_insert);
					mystl::copy(first, last, position);
				}
				finish_ += num_of_insert;
			}
			else
			{
				const size_type new_capacity = get_new_capacity(num_of_insert);

				iterator new_start = data_allocator::allocate(new_capacity);
				iterator new_finish = new_start;
				try
				{
					new_finish = mystl::uninitialized_copy(start_, position, new_start);
					new_finish = mystl::uninitialized_copy(first, last, new_finish);
					new_finish = mystl::uninitialized_copy(position, finish, new_finish);
				}
				catch (...)
				{
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, distance(new_start, new_finish));
					throw;
				}

				destroy(start_, finish_);
				data_allocator::deallocate(start_, distance(start_, end_of_storage));

				start_ = new_start;
				finish_ = new_finish;
				end_of_storage = new_start + new_capacity;
			}
		}

		void insert_aux(iterator position, size_t n, const T& value, mystl::__true_type)
		{
			difference_type num_of_left = end_of_storage - finish_;
			if (num_of_left >= n)
			{
				if (position + n < finish_)
				{
					mystl::uninitialized_copy(finish_ - n, finish_, finish_);
					mystl::copy_backward(position, finish_ - n, finish_);
					mystl::uninitialized_fill_n(position, n, value);
				}
				else
				{
					mystl::uninitialized_copy(position, finish_, position + n);
					mystl::uninitialized_fill_n(position, n, value);
				}
				finish_ += n;
			}
			else
			{
				const size_type new_capacity = get_new_capacity(n);

				iterator new_start = data_allocator::allocate(new_capacity);
				iterator new_finish = new_start;
				try
				{
					new_finish = mystl::uninitialized_copy(start_, position, new_start);
					new_finish = mystl::uninitialized_fill_n(position, n, value);
					new_finish = mystl::uninitialized_copy(position, finish_, new_finish);
				}
				catch (...)
				{
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, distance(new_start, new_finish));
				}

				destroy(start_, finish_);
				data_allocator::deallocate(new, distance(start_, finish_));

				start_ = new_start;
				finish_ = new_finish;
				end_of_storage = new_start + new_capacity;
			}
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

		size_type get_new_capacity(size_type add_size) const
		{
			size_type old_capacity = end_of_storage - start_;
			size_type tmp = mystl::max(add_size, old_capacity);
			return (old_capacity != 0 ? (old_capacity + tmp) : add_size);
		}
	};
}
