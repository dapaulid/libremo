//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
//
// C++
#include <stddef.h> // size_t
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------

class ListItem;

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class BaseList
{
// ctor/dtor
public:
	BaseList();
	virtual ~BaseList();

// public member functions
public:
	size_t get_count() const { return m_count; }
	bool is_empty() const { return m_count == 0; }

	void push_front(ListItem* a_item);
	void push_back(ListItem* a_item);
	void remove(ListItem* a_item);

	ListItem* get_first() { return m_first; }
	ListItem* get_last() { return m_last; }

// private members
private:
	ListItem* m_first;
	ListItem* m_last;
	size_t m_count;	

};


//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
class ListItem
{
// ctor/dtor
public:
	ListItem(BaseList* a_list = nullptr):
		m_list(nullptr),
		m_prev(nullptr),
		m_next(nullptr)
	{
		set_list(a_list);
	}
	virtual ~ListItem() {
		if (m_list) m_list->remove(this);
	}

// public member functions
public:
	const BaseList* get_list() const { return m_list; }	
	BaseList* get_list() { return m_list; }

	const ListItem* get_prev() const { return m_prev; }
	ListItem* get_prev() { return m_prev; }

	const ListItem* get_next() const { return m_next; }
	ListItem* get_next() { return m_next; }

// protected member functions used by BaseList
protected:
	friend class BaseList;
	void set_list(BaseList* a_list) { m_list = a_list;	}
	void set_prev(ListItem* a_prev) { m_prev = a_prev; }
	void set_next(ListItem* a_next) { m_next = a_next; }

protected:
	BaseList* m_list;
	ListItem* m_prev;
	ListItem* m_next;
};

//------------------------------------------------------------------------------
// class declaration
//------------------------------------------------------------------------------
//
template<typename T>
class List: public BaseList
{
// public types
public:
	typedef T Item;

// ctor/dtor
public:
	List(): BaseList() {}
	virtual ~List() {}

// public member functions
public:
	void push_front(Item* a_item) {
		BaseList::push_front(a_item);
	}
	void push_back(Item* a_item) {
		BaseList::push_back(a_item);
	}
	void remove(Item* a_item) {
		BaseList::remove(a_item);
	}

	void move_to_front(Item* a_item) {
		remove(a_item);
		push_front(a_item);
	}
	void move_to_back(Item* a_item) {
		remove(a_item);
		push_back(a_item);
	}

	bool contains(const Item* a_item) const { 
		return a_item->get_list() == this;
	}

	Item* get_first() { 
		return static_cast<Item*>(BaseList::get_first());
	 }
	Item* get_last() { 
		return static_cast<Item*>(BaseList::get_last());
	}


};

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
