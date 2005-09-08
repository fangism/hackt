/**
	\file "Object/state_manager.cc"
	This module has been obsoleted by the introduction of
		the footprint class in "Object/def/footprint.h".
	$Id: state_manager.cc,v 1.3.2.3 2005/09/08 05:47:33 fang Exp $
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
#include "util/IO_utils.h"

namespace ART {
namespace entity {
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"
//=============================================================================
// class global_entry_pool method definitions

template <class Tag>
global_entry_pool<Tag>::global_entry_pool() :
		pool_type() {
	this->set_chunk_size(class_traits<Tag>::instance_pool_chunk_size);
	this->allocate();	// reserve the 0-index as NULL
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry_pool<Tag>::~global_entry_pool() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
global_entry_pool<Tag>::dump(ostream& o) const {
if (this->size() > 1) {
	o << "global " << class_traits<Tag>::tag_name << " entries" << endl;
	size_t j = 1;
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++, j++) {
		i->dump(o << j << '\t') << endl;
	}
}
	return o;
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	// nothing yet
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
		i->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	write_value(o, this->size() -1);
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
		i->write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry_pool<Tag>::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	size_t s;
	read_value(i, s);
	// consider setting chunk size to s+1 for optimization
	size_t j = 0;
	for ( ; j<s; j++) {
		(*this)[this->allocate()].load_object_base(m, i);
	}
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
ostream&
state_manager::dump(ostream& o) const {
	global_entry_pool<process_tag>::dump(o);
	global_entry_pool<channel_tag>::dump(o);
	global_entry_pool<datastruct_tag>::dump(o);
	global_entry_pool<enum_tag>::dump(o);
	global_entry_pool<int_tag>::dump(o);
	global_entry_pool<bool_tag>::dump(o);
	return o;
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::collect_transient_info_base(persistent_object_manager& m) const {
	// for now, pools contain no pointers.  
#if 0
	global_entry_pool<process_tag>::collect_transient_info_base(m);
	global_entry_pool<datastruct_tag>::collect_transient_info_base(m);
	global_entry_pool<channel_tag>::collect_transient_info_base(m);
	global_entry_pool<enum_tag>::collect_transient_info_base(m);
	global_entry_pool<int_tag>::collect_transient_info_base(m);
	global_entry_pool<bool_tag>::collect_transient_info_base(m);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	global_entry_pool<process_tag>::write_object_base(m, o);
	global_entry_pool<datastruct_tag>::write_object_base(m, o);
	global_entry_pool<channel_tag>::write_object_base(m, o);
	global_entry_pool<enum_tag>::write_object_base(m, o);
	global_entry_pool<int_tag>::write_object_base(m, o);
	global_entry_pool<bool_tag>::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
state_manager::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	global_entry_pool<process_tag>::load_object_base(m, i);
	global_entry_pool<datastruct_tag>::load_object_base(m, i);
	global_entry_pool<channel_tag>::load_object_base(m, i);
	global_entry_pool<enum_tag>::load_object_base(m, i);
	global_entry_pool<int_tag>::load_object_base(m, i);
	global_entry_pool<bool_tag>::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

