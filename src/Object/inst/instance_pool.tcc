/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.13.88.1 2010/01/09 03:30:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_TCC__

#include <iostream>
#include "Object/inst/instance_pool.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/def/footprint.h"
#include "util/persistent_object_manager.tcc"	// for STACKTRACE macros
#include "util/list_vector.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/memory/index_pool.tcc"
#include "util/type_traits.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;
//=============================================================================
// class instance_pool method definitions

/**
	Note: this constructor initializes with one element pre-allocated, 
	so the first index returned by allocator is nonzero.  
 */
template <class T>
instance_pool<T>::instance_pool(const size_type s) : parent_type()
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		, _port_entries(0), private_entry_map()
#endif
		{
	STACKTRACE_CTOR_VERBOSE;
	STACKTRACE_CTOR_PRINT("at: " << this << endl);
	this->set_chunk_size(s);
	allocate();
	INVARIANT(this->size());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// start with a dummy sentinel entry
	private_entry_map.push_back(std::make_pair(0, 0));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default constructor, when we don't care about chunk size.  
 */
template <class T>
instance_pool<T>::instance_pool() : parent_type()
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		, _port_entries(0), private_entry_map()
#endif
		{
	STACKTRACE_CTOR_VERBOSE;
	STACKTRACE_CTOR_PRINT("at: " << this << endl);
	this->set_chunk_size(default_chunk_size);
	allocate();
	INVARIANT(this->size());
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	private_entry_map.push_back(std::make_pair(0, 0));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
instance_pool<T>::~instance_pool() {
	STACKTRACE_DTOR_VERBOSE;
	STACKTRACE_DTOR_PRINT("at: " << this << endl);
}

//-----------------------------------------------------------------------------
/**
	Dumps the state of the entire pool of instances.  
	Displays the unique ID number and each instance's state information.  
	NOTE: this will print the terminating end-line, 
		the caller need not call it.  
		This also includes the first auto_indent.  
 */
template <class T>
ostream&
instance_pool<T>::dump(ostream& o) const {
if (this->size() > 1) {
	o << auto_indent << traits_type::tag_name << " instance pool:" << endl;
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		i->dump(o << auto_indent << j << '\t') << endl;
	}
}
	// else pool is empty
	return o;
}

//-----------------------------------------------------------------------------
/**
	Visit reachable state information.  
 */
template <class T>
void
instance_pool<T>::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	STACKTRACE_PERSISTENT_PRINT("at: " << this << endl);
	INVARIANT(this->size());
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
		i->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_pool<T>::write_object_base(const collection_pool_bundle_type& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const_iterator i(++this->begin());	// skip first element (NULL)
	const const_iterator e(this->end());
	const size_t s = this->size();
	INVARIANT(s);
	STACKTRACE_PERSISTENT_PRINT("size = " << s << endl);
	write_value(o, s-1);
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		i->write_object_base(m, o);
	}
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	// technically, this information can be reconstructed
	write_value(o, _port_entries);
	write_sequence(o, private_entry_map);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: pre-reserving memory is not necessary, because the 
	instance_pool implementation (list_vector) already reserves
	chunks in advance as necessary.  
	\pre the pool is already globally statically initialized.  
 */
template <class T>
void
instance_pool<T>::load_object_base(const collection_pool_bundle_type& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	size_t s;
	read_value(i, s);
	STACKTRACE_PERSISTENT_PRINT("size = " << s << endl);
	size_t j=0;
	for ( ; j<s; j++) {
		T temp;
		temp.load_object_base(m, i);
		this->allocate(temp);
		// works because this_type is copy-constructible
	}
#if 0 && MEMORY_MAPPED_GLOBAL_ALLOCATION
	// technically, this information can be reconstructed
	read_value(i, _port_entries);
	read_sequence_resize(i, private_entry_map);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_POOL_TCC__
