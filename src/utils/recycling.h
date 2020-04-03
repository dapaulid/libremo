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

#include "l0_system/error.h"

//------------------------------------------------------------------------------
namespace remo {
//------------------------------------------------------------------------------	


template<typename Recyclable>
class RecyclingPool {
public:
	RecyclingPool():
		m_first(nullptr)
	{
	}

	virtual ~RecyclingPool()
	{
		Recyclable* obj = m_first;
		while (obj) {
			Recyclable* tmp = obj;
			obj = obj->get_next();
			delete tmp;
		}
	}

	void add(Recyclable* a_object) 
	{
		a_object->set_pool(nullptr);
		a_object->set_next(m_first);
		m_first = a_object;
	}

	Recyclable* take() 
	{
		if (m_first) {
			// remove first object from our list
			Recyclable* object = m_first;
			m_first = m_first->get_next();
			// remember where you come from
			object->set_pool(this);
			return object;
		} else {
			// out of objects
			return nullptr;
		}
	}

private:
	Recyclable* m_first;
};

//------------------------------------------------------------------------------

template<typename SubClass>
class Recyclable {
public:
	Recyclable(): m_pool(nullptr), m_next(nullptr)
	{
	}

	virtual ~Recyclable() noexcept(false)
	{
		// pool assigned?
		if (m_pool) {
			// yes -> destroy not allowed
			REMO_THROW_NOLOG(ErrorCode::ERR_MUST_RECYCLE,
				"not allowed to destroy object, call recycle() instead");
		}
	}

protected:
	virtual void recycle()
	{
		// pool assigned?
		if (m_pool) {
			// yes -> give it back
			m_pool->add(static_cast<SubClass*>(this));
		} else {
			// no -> cannot recycle
			REMO_THROW_NOLOG(ErrorCode::ERR_CANNOT_RECYCLE, 
				"cannot recycle object, destroy it instead");
		}
	}

public:
	struct Recycler {  
		void operator()(Recyclable<SubClass>* a_recyclable) {
			a_recyclable->recycle();
		}
	};	

private:
	typedef RecyclingPool<SubClass> Pool; 
	friend Pool;

	void set_pool(Pool* a_pool) {
		m_pool = a_pool;
	}

	SubClass* get_next() {
		return m_next;
	}

	void set_next(SubClass* a_object) {
		m_next = a_object;
	}


private:
	Pool* m_pool;
	SubClass* m_next;
};

//------------------------------------------------------------------------------	


//------------------------------------------------------------------------------
} // end namespace remo
//------------------------------------------------------------------------------
