//------------------------------------------------------------------------------
/**
 * @license
 * Copyright (c) Daniel Pauli <dapaulid@gmail.com>
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
//------------------------------------------------------------------------------
#include "list.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//
// project
#include "utils/contracts.h"
#include "utils/logger.h"
//
// C++ 
//
// system
//
//
//------------------------------------------------------------------------------
namespace remo {
	namespace utils {
//------------------------------------------------------------------------------	

//! logger instance
static Logger logger("List");


//------------------------------------------------------------------------------
// class implementation
//------------------------------------------------------------------------------
//
BaseList::BaseList():
	m_first(nullptr),
	m_last(nullptr),
	m_count(0)
{
}

//------------------------------------------------------------------------------
//
BaseList::~BaseList()
{
}

//------------------------------------------------------------------------------
//
void BaseList::push_front(ListItem* a_item)
{
	REMO_PRECOND({
		REMO_ASSERT(!a_item->get_list(), 
			"item must not yet be in a list");
	});

	a_item->set_list(this);
	a_item->set_next(m_first);
	if (is_empty()) {
		m_first = a_item;
		m_last = a_item;
	} else {
		m_first->set_prev(a_item);
	}
	m_first = a_item;
	++m_count;
}

//------------------------------------------------------------------------------
//
void BaseList::push_back(ListItem* a_item)
{
	REMO_PRECOND({
		REMO_ASSERT(!a_item->get_list(), 
			"item must not yet be in a list");
	});

	a_item->set_list(this);
	a_item->set_prev(m_last);
	if (is_empty()) {
		m_first = a_item;
		m_last = a_item;
	} else {
		m_last->set_next(a_item);
	}
	m_last = a_item;
	++m_count;
}

//------------------------------------------------------------------------------
//
void BaseList::remove(ListItem* a_item)
{
	REMO_PRECOND({
		REMO_ASSERT(a_item->get_list() == this, 
			"item must be in this list");
	});

	if (m_first == a_item) {
		// handle first
		m_first = a_item->get_next();
	} else {
		a_item->get_prev()->set_next(a_item->get_next());
	}
	if (m_last == a_item) {
		// handle first
		m_last = a_item->get_prev(); 
	} else {
		a_item->get_next()->set_prev(a_item->get_prev());
	}

	--m_count;

	a_item->set_prev(nullptr);
	a_item->set_next(nullptr);
	a_item->set_list(nullptr);
}

//------------------------------------------------------------------------------
	} // end namespace utils
} // end namespace remo
//------------------------------------------------------------------------------
