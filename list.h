#pragma once

#include "Iterator.h"
#include "allocator.h"

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
		using iterator = __list_iterator<T>;
	protected:
		list_node node;
		using data_allocator = Alloc;
	public:
	};

}