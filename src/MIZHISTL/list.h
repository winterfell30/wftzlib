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

		__list_iterator& operator=(const __list_iterator& it);
		reference operator*();
		pointer operator->();
		__list_iterator& operator++();
		__list_iterator operator++(int);
		__list_iterator& operator--();
		__list_iterator operator--(int);

		template <typename T>
		friend bool operator==(const __list_iterator<T>& lhs, const __list_iterator<T>& rhs);
		template <typename T>
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
	public:
		list();
		explicit list(size_t n, const value_type& val = value_type());
		template <typename InputIterator>
		list(InputIterator first, InputIterator last);
		list(const list& l);
		list(list&& l) noexcept;
		list(std::initializer_list<value_type> il);
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

		void merge(list&& l);
		template <typename Compare>
		void merge(list&&, Compare);

		void splice(const_iterator position, list& l);
		void splice(const_iterator position, list&& l);
		void splice(const_iterator position, list& l, const_iterator it);
		void splice(const_iterator position, list&& l, const_iterator it);
		void splice(const_iterator position, list& l, const_iterator first, const_iterator last);
		void splice(const_iterator position, list&& l, const_iterator first, const_iterator last);

		void remove(const value_type& val);
		template <typename Predicate>
		void remove_if(Predicate pred);

		void reverse();

		void unique();
		template <typename BinaryPredicate>
		void unique(BinaryPredicate pred);

		void sort();
		template <typename Compare>
		void sort(Compare cmp);

	protected:
		list_node new_node(const value_type& val = value_type());
		void del_node(list_node node);
		list_node insert_aux(list_node position, list_node node);
		list_node insert_aux(list_node position, list_node first, list_node last, size_type cnt);
		list_node yield(list_node node);
		list_node yield(list_node first, list_node last, size_type cnt);
		list_node erase(list_node node);
	protected:
		list_node head;             //dummy，head->prev为链表的尾节点，head->next为链表的起点
		size_type size_;
		using data_allocator = Alloc;
	};

	template <typename T>
	__list_iterator<T>& __list_iterator<T>::operator=(const __list_iterator<T>& it)
	{
		node = it.node;
	}

	template <typename T>
	T& __list_iterator<T>::operator*()
	{
		return node->data;
	}

	template <typename T>
	T* __list_iterator<T>::operator->()
	{
		return &(node->data);
	}

	template <typename T>
	__list_iterator<T>& __list_iterator<T>::operator++()
	{
		node = node->next;
		return *this;
	}

	template <typename T>
	__list_iterator<T> __list_iterator<T>::operator++(int)
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	template <typename T>
	__list_iterator<T>& __list_iterator<T>::operator--()
	{
		node = node->prev;
		return *this;
	}

	template <typename T>
	__list_iterator<T> __list_iterator<T>::operator--(int)
	{
		auto tmp = *this;
		--(*this);
		return tmp;
	}

	template <typename T>
	bool operator==(const __list_iterator<T>& lhs, const __list_iterator<T>& rhs)
	{
		return lhs.node == lhs.node;
	}

	template <typename T>
	bool operator!=(const __list_iterator<T>& rhs, const __list_iterator<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::new_node(const list<T, Alloc>::value_type& val = value_type())
	{
		list_node ptr = data_allocator::allocate(1);
		mystl::construct(ptr, val);
		return ptr;
	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::del_node(list<T, Alloc>::list_node node)
	{
		mystl::destroy(node);
		data_allocator::deallocate(node);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::insert_aux(list<T, Alloc>::list_node pos, list<T, Alloc>::list_node node)
	{
		return insert_aux(pos, node, node, 1);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::insert_aux(list<T, Alloc>::list_node pos, list<T, Alloc>::list_node first, list<T, Alloc>::list_node last, size_t n)
	{
		size_ += n;
		first->prev = pos->prev;
		pos->prev->next = first;
		last->next = pos;
		pos->prev = last;
		return first;
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::yield(list<T, Alloc>::list_node node)
	{
		return yield(node, node, 1);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::yield(list<T, Alloc>::list_node first, list<T, Alloc>::list_node last, size_t n)
	{
		size_ -= n;
		first->prev->next = last->next;
		last->next->prev = first->prev;
		return first;
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list_node list<T, Alloc>::erase(list<T, Alloc>::list_node pos)
	{
		list<T, Alloc>::list_node res = pos->next;
		del_node(yield(pos));
		return res;
	}

	template <typename T, typename Alloc>
	explicit list<T, Alloc>::list() :
		head(new_node()), size_(0)
	{}

	template <typename T, typename Alloc>
	list<T, Alloc>::list(size_t n, const list<T, Alloc>::value_type& val = value_type()) :
		head(new_node()), size_(0)
	{
		for (size_t i = 0; i < n; i++)
			push_back(val);
	}

	template <typename T, typename Alloc>
	template <typename InputIterator>
	list<T, Alloc>::list(InputIterator first, InputIterator last) :
		head(new_node()), size_(0)
	{
		for (; first != last; first++)
			push_back(*first);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list(const list& l) :
		list(l.begin(), l.end())
	{
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list(list&& l) :
		list(std::move(l))
	{
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::list(std::initializer_list<T> il) :
		list(l.begin(), l.end())
	{
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::~list() noexcept
	{
		list_node p = head;
		while (p != nullptr)
		{
			list_node tmp = p;
			p = p->next;
			del_node(tmp);
		}
	}

	template <typename T, typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(const list& l)
	{
		list(l).swap(*this);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(list&& l)
	{
		list(l).swap(*this);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(std::initializer_list<T> il)
	{
		list(l).swap(*this);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::iterator list<T, Alloc>::begin() noexcept
	{
		return iterator(head->next);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::iterator list<T, Alloc>::end() noexcept
	{
		return iterator(head);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::const_iterator list<T, Alloc>::begin() const noexcept
	{
		return const_iterator(head->next);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::const_iterator list<T, Alloc>::end() const noexcept
	{
		return const_iterator(head);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const noexcept
	{
		return const_iterator(head->next);
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::const_iterator list<T, Alloc>::cend() const noexcept
	{
		return const_iterator(head);
	}

	template <typename T, typename Alloc>
	bool list<T, Alloc>::empty() const
	{
		return size_ == 0;
	}

	template <typename T, typename Alloc>
	size_t list<T, Alloc>::size() const
	{
		return size_;
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::value_type& list<T, Alloc>::front()
	{
		return *(this->begin());
	}

	template <typename T, typename Alloc>
	const list<T, Alloc>::value_type& list<T, Alloc>::front() const
	{
		return *(this->begin());
	}

	template <typename T, typename Alloc>
	list<T, Alloc>::value_type& list<T, Alloc>::back()
	{
		return *(--this->end());
	}

	template <typename T, typename Alloc>
	const list<T, Alloc>::value_type& list<T, Alloc>::back() const
	{
		return *(--this->end());
	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::clear()
	{
		resize(0);
	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::resize(size_t n)
	{
		
	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::resize(size_t n, const list<T, Alloc>::value_type& val)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::swap(list<T, Alloc>& l)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::push_front(const list<T, Alloc>::value_type& val)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::push_front(list<T, Alloc>::value_type&& val)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::push_back(const list<T, Alloc>::value_type& val)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::push_back(list<T, Alloc>&& val)
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::pop_front()
	{

	}

	template <typename T, typename Alloc>
	void list<T, Alloc>::pop_back()
	{

	}
}
