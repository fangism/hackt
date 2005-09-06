/**
	\file "Object/state_manager.cc"
	This module has been obsoleted by the introduction of
		the footprint class in "Object/def/footprint.h".
	$Id: state_manager.cc,v 1.3.2.1 2005/09/06 05:56:46 fang Exp $
 */

#include <iostream>
#include "Object/state_manager.h"
#include "Object/global_entry.tcc"
#include "Object/traits/proc_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"
#include "util/list_vector.tcc"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class global_entry_pool method definitions

template <class Tag>
global_entry_pool<Tag>::global_entry_pool() :
		pool_type() {
	this->set_chunk_size(class_traits<Tag>::instance_pool_chunk_size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry_pool<Tag>::~global_entry_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
global_entry_pool<Tag>::allocate(void) {
	const size_t ret = this->size();
	push_back(entry_type());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
global_entry_pool<Tag>::allocate(const entry_type& t) {
	const size_t ret = this->size();
	push_back(t);
	return ret;
}

//=============================================================================
// class state_manager method definitions

state_manager::state_manager() :
		process_pool_type(), channel_pool_type(), 
		struct_pool_type(), enum_pool_type(), 
		int_pool_type(), bool_pool_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
state_manager::~state_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
state_manager::allocate(void) {
	return global_entry_pool<Tag>::allocate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
state_manager::allocate(const typename global_entry_pool<Tag>::entry_type& t) {
	return global_entry_pool<Tag>::allocate(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, just written to assist instantiation.  
 */
template <class Tag>
void
state_manager::__allocate_test(void) {
	const typename global_entry_pool<Tag>::entry_type	_e;
	allocate<Tag>();
	allocate<Tag>(_e);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Never called, just written to force instantiation.  
 */
void
state_manager::allocate_test(void) {
	__allocate_test<process_tag>();
	__allocate_test<channel_tag>();
	__allocate_test<datastruct_tag>();
	__allocate_test<enum_tag>();
	__allocate_test<int_tag>();
	__allocate_test<bool_tag>();
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

