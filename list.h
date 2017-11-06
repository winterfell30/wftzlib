#pragma once

#include "Iterator.h"
#include "allocator.h"
#include <initializer_list>

namespace mystl 
{
	template <typename T>
	struct __list_node
	{
		__list_node<T>* prev;
		__list_node<T>* next;
		T data;
	};

	template <typename T>
	struct __list_iterator : public iterator<bidirectional_iterator_tag, T>
	{
	public:
		using list_node = __list_node<T>*;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
	public:
		list_node node;
	public:
		explicit __list_iterator(list_node ptr = nullptr) :
			node(ptr) {}

		reference operator*();
		pointer operator->();
		__list_iterator& operator++();
		__list_iterator operator++(int);
		__list_iterator& operator--();
		__list_iterator operator--(int);

		friend bool operator==(const __list_iterator<T>& lhs, const __list_iterator<T>& rhs);
		friend bool operator!=(const __List_iterator<T>& lhs, const __list_iterator<T>& rhs);
	};

	template <typename T, typename Alloc = mystl::allocator<T> >
	class list
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using list_node = __list_node<value_type>*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator = __list_iterator<value_type>;
		using const_iterator = __list_iterator<const value_type>;
	protected:
		list_node node;
		using data_allocator = Alloc;
	public:
		list();
		explicit list(size_t n, const value_type& val = value_type());
		template <typename InputIterator>
		list(InputIterator first, InputIterator last);
		list(const list& l);
		list(list&& l) noexcept;
		list(std::initializer_list<value_type>& il);
		~list() noexcept;
		list& operator=(const list& l);
		list& operator=(list&& l) noexcept;
		list& operator=(std::initializer_list<value_type>& il);

		iterator begin() noexcept;
		iterator end() noexcept;
		const_iterator begin() const noexcept;
		const_iterator end() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		bool empty() const;
		size_type size() const;
		value_type& front();
		const value_type& front() const;
		value_type& back();
		const value_type& back() const;

		void clear();
		void resize(size_type n);
		void resize(size_type n, const value_type& val);
		void swap(list<value_type>& l);

		void push_front(const value_type& val);
		void push_front(value_type&& val);
		void push_back(const value_type& val);
		void push_back(value_type&& val);
		void pop_front();
		void pop_back();

		template <typename... Args>
		iterator emplace(const_iterator position, Args&&... args);
		template <typename... Args>
		void emplace_back(Args&&... args);
		template <typename... Args>
		void emplace_front(Args&&... args);

		iterator insert(const_iterator position, const value_type& val);
		iterator insert(const_iterator position, value_type&& val);
		void insert(const_iterator position, size_type n, const value_type& val);
		template <typename InputIterator>
		void insert(const_iterator position, InputIterator first, InputIterator last);
		iterator erase(const_iterator position);
		iterator erase(const_iterator first, const_iterator last);
	};

}