/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.4.18.1 2005/12/11 00:45:36 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_POOL_TCC__
#define	__OBJECT_INST_INSTANCE_POOL_TCC__

#include <iostream>
#include "Object/inst/instance_pool.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/def/footprint.h"
#include "util/persistent_object_manager.tcc"	// for STACKTRACE macros
#include "util/list_vector.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "util/indent.h"

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
instance_pool<T>::instance_pool(const size_t s) : parent_type() {
	STACKTRACE_CTOR_VERBOSE;
#if STACKTRACE_CONSTRUCTORS
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
	this->set_chunk_size(s);
	allocate();
	INVARIANT(this->size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default constructor, when we don't care about chunk size.  
 */
template <class T>
instance_pool<T>::instance_pool() : parent_type() {
	STACKTRACE_CTOR_VERBOSE;
#if STACKTRACE_CONSTRUCTORS
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
	this->set_chunk_size(default_chunk_size);
	allocate();
	INVARIANT(this->size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
instance_pool<T>::instance_pool(const this_type& t) : parent_type(t) {
	STACKTRACE_CTOR_VERBOSE;
#if STACKTRACE_CONSTRUCTORS
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
	INVARIANT(this->size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
instance_pool<T>::~instance_pool() {
	STACKTRACE_DTOR_VERBOSE;
#if STACKTRACE_DESTRUCTORS
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates one element.  
	Allocation is extremely efficient because of the underlying
	list_vector implementation -- never reallocs and is pool-reserved
	per chunk.  
	\return the index of the newly allocated element.  
 */
template <class T>
size_t
instance_pool<T>::allocate(void) {
//	STACKTRACE_VERBOSE;
	STACKTRACE("instance_pool::allocate()");
	const size_t ret = this->size();
	push_back(T());
	INVARIANT(this->size());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates one element with an initial value.  
	Requires that type T is copy-constructible.
 */
template <class T>
size_t
instance_pool<T>::allocate(const T& t) {
//	STACKTRACE_VERBOSE;
	STACKTRACE("instance_pool::allocate(const T&)");
	const size_t ret = this->size();
	push_back(t);
	INVARIANT(this->size());
	return ret;
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
	o << auto_indent << class_traits<tag_type>::tag_name <<
		" instance pool:" << endl;
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
#if 0
template <class T>
good_bool
instance_pool<T>::expand_footprint(footprint& f) const {
if (this->size() > 1) {
	const_iterator i(++this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++) {
		if (!i->get_back_ref()->
				allocate_subinstance_footprint(f).good) {
			return good_bool(false);
		}
	}
}
	return good_bool(true);
}
#endif

//-----------------------------------------------------------------------------
/**
	Visit reachable state information.  
 */
template <class T>
void
instance_pool<T>::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
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
instance_pool<T>::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const_iterator i(++this->begin());	// skip first element (NULL)
	const const_iterator e(this->end());
	const size_t s = this->size();
	INVARIANT(s);
	write_value(o, s-1);
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		i->write_object_base(m, o);
	}
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
instance_pool<T>::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	size_t s;
	read_value(i, s);
	size_t j=0;
	for ( ; j<s; j++) {
		T temp;
		temp.load_object_base(m, i);
		this->allocate(temp);
		// works because this_type is copy-constructible
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_INSTANCE_POOL_TCC__
