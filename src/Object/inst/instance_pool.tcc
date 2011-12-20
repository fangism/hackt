/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.15 2010/04/07 00:12:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_POOL_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_POOL_TCC__

#include <iostream>
#include <algorithm>
#include "Object/inst/instance_pool.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/def/footprint.h"
#include "util/persistent_object_manager.tcc"	// for STACKTRACE macros
#include "util/persistent_functor.tcc"
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
	Default constructor, when we don't care about chunk size.  
	Note: this constructor initializes with one element pre-allocated, 
	so the first index returned by allocator is nonzero.  
 */
template <class T>
instance_pool<T>::instance_pool() : parent_type(),
		_port_entries(0), private_entry_map() {
	STACKTRACE_CTOR_VERBOSE;
	STACKTRACE_CTOR_PRINT("at: " << this << endl);
	STACKTRACE_CTOR_PRINT("entry-map-size: " << private_entry_map.size() << endl);
	const pool_private_map_entry_type t(0, 0);
	private_entry_map.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
instance_pool<T>::~instance_pool() {
	STACKTRACE_DTOR_VERBOSE;
	STACKTRACE_DTOR_PRINT("at: " << this << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
typename instance_pool<T>::const_iterator
instance_pool<T>::local_private_begin(void) const {
	return this->begin() +_port_entries;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
static
bool
offset_less(const size_t l, const pool_private_map_entry_type& r) {
	return l < r.second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search for the entry that corresponds to the private local
	substructure process.  
	\param li mapped index must be 0-based.
 */
template <class T>
const pool_private_map_entry_type&
instance_pool<T>::locate_private_entry(const size_t li) const {
	pool_private_entry_map_type::const_iterator
		f(std::upper_bound(private_entry_map.begin(),
			private_entry_map.end(), 
			li, &offset_less));
	INVARIANT(f != private_entry_map.begin());
	--f;
	return *f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline
static
bool
pid_less(const pool_private_map_entry_type& l, const size_t r) {
	return l.first < r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search for the entry that corresponds to the private local
	substructure process, the last entry before (up to) ID pi.  
	\param pi the 1-based process index.
 */
template <class T>
const pool_private_map_entry_type&
instance_pool<T>::locate_cumulative_entry(const size_t pi) const {
	pool_private_entry_map_type::const_iterator
		f(std::lower_bound(private_entry_map.begin(),
			private_entry_map.end(), 
			pi, &pid_less));
	INVARIANT(f != private_entry_map.end());
	return *f;
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
// no-sentinel or dummy instance
if (this->size()) {
	o << auto_indent << traits_type::tag_name << " instance pool:";
	o << " (" << this->port_entries() << " ports, " <<
		this->local_private_entries() << " local, " <<
		this->non_local_private_entries() << " mapped)";
	o << endl;
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	size_t j = 1;
	for ( ; i!=e; i++, j++) {
		i->dump(o << auto_indent << j << '\t') << endl;
	}
}
	// else pool is empty
	pool_private_entry_map_type::const_iterator
		i(private_entry_map.begin()), e(private_entry_map.end());
	INVARIANT(i != e);
if (private_entry_map.size() > 1) {
	o << auto_indent << "private sub-" << traits_type::tag_name
		<< " index map:" << endl;
	INDENT_SECTION(o);
	for ( ; i!=e; ++i) {
		// first: process index, second: subordinate index lower bound
		o << auto_indent << '(' << i->first << " -> " << i->second
			<< ")" << endl;
	}
	// last entry reveals the total number of non-local private entries
} else {
#if 0
	o << auto_indent << "private sub-" << traits_type::tag_name
		<< " index map:";
	o << " empty (" << i->first << " -> " << i->second
		<< ")" << endl;
#else
	// silence empty sub maps
#endif
}
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
	for_each(this->begin(), this->end(), util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
instance_pool<T>::write_object_base(const collection_pool_bundle_type& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	util::write_persistent_sequence(m, o, AS_A(const parent_type&, *this));
	// other member fields are reconstructed
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
	util::read_persistent_sequence_resize(m, i, AS_A(parent_type&, *this));
	// other member fields are reconstructed
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_POOL_TCC__
