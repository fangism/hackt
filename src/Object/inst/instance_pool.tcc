/**
	\file "Object/inst/instance_pool.tcc"
	Implementation of instance pool.
	$Id: instance_pool.tcc,v 1.9.8.1 2006/02/17 07:52:02 fang Exp $
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

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
// this is needed to fix a horrible hack, see comments in ::compact.
#define	DEFER_COMPACTION_REASSIGNMENT		1
#endif

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
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	, free_list()
#endif
	{
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
instance_pool<T>::instance_pool() : parent_type()
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	, free_list()
#endif
	{
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
instance_pool<T>::instance_pool(const this_type& t) : parent_type(t)
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
	, free_list()
#endif
	{
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
#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
template <class T>
typename instance_pool<T>::size_type
instance_pool<T>::allocate(void) {
	if (free_list.empty()) {
		return parent_type::allocate();
	} else {
		const size_type ret = free_list.top();
		free_list.pop();
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
typename instance_pool<T>::size_type
instance_pool<T>::allocate(const value_type& v) {
	if (free_list.empty()) {
		return parent_type::allocate(v);
	} else {
		const size_type ret = free_list.top();
		free_list.pop();
		(*this)[ret] = v;
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre free_list does NOT already contain i.  
 */
template <class T>
void
instance_pool<T>::deallocate(const size_type i) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "released pool index " << i << endl;
#endif
	free_list.push(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	20060127:
	Hack to go through freelist and plug holes by moving
	the tail end instances to fill them in.  
	This operation can be potentially expensive in the worst case, 
	but it should be rather infrequent.  
	\post the instance_indices of the back-references of the
		state_instances' aliases should be updated to the 
		new index, obtained from the free list.  
		What about canonical?
	Backpatching remap is kept in the 'remap' member.
 */
template <class T>
void
instance_pool<T>::compact(void) {
	STACKTRACE_VERBOSE;
	size_type move_index = this->size() -1;
while (!free_list.empty()) {
	const size_type free = free_list.top();
	free_list.pop();
//	this->compact();	// recursive solution to reverse ordering
#if ENABLE_STACKTRACE
//	const size_type _size = this->size();
//	STACKTRACE_INDENT << "pool size remaining = " << _size << endl;
	STACKTRACE_INDENT << "considering free entry = " << free << endl;
#endif
#if DEFER_COMPACTION_REASSIGNMENT
	// ******** IMPORTANT COMMENT (aren't they all?) ************
	// HOWEVER this alone does NOT guarantee coverage of ALL
	// aliases because ring connections are not pushed down
	// recursively!  This may leave nodes stranded!
	// Sadly, we must defer the backpatching to the caller, 
	// footprint::compact().  
	// This hack is SO bad... there is no word appropriate
	// for this.  
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "remapping index " << move_index <<
		" to " << free << endl;
#endif
	remap[move_index] = free;
	--move_index;
	// This just transfers the free-list to the caller.  
	// do NOT push back, defer that to the caller too.  
#else
#if 0
	if (free+1 < _size) {	// +1 because is 1-indexed, 0th entry reserved
		// the tail entry needs to be copied and updated
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "hole in instance pool as position "
			<< free << endl;
#endif
		// state-instances are copy-able, 
		// they just contain an alias pointer
		(*this)[free] = this->back();	// copy-move
		typedef	typename value_type::back_ref_type	alias_ptr_type;
		const alias_ptr_type a((*this)[free].get_back_ref());
		NEVER_NULL(a);
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "old index of alias ring is "
			<< a->instance_index << " being replaced with "
			<< free << endl;
#endif
		// will need const_cast
		typedef	typename alias_ptr_type::element_type	alias_type;
		typedef	typename util::remove_const<alias_type>::type	mod_type;
		const_cast<mod_type&>(*a).force_update_index(free);
		// will cover aliases in the ring too...
	}
#if ENABLE_STACKTRACE
	else {
		STACKTRACE_INDENT << "discarding back()" << endl;
	}
#endif
	// we're done using the tail entry, either copied or discarding
	// for each free_list entry removed, we shrink-compact the array
	this->pop_back();
#endif
#endif	// DEFER_COMPACTION_REASSIGNMENT
}	// else free_list is empty, nothing to be checked
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Remaps 'deallocated' index to new index.  
	Effectively compacts the indices.  
	Pardon the hack.  
	This is used in a separate remapping pass.  
 */             
template <class Tag>
size_t
instance_pool<Tag>::translate_remap(const size_t i) const {
	if (!remap.empty()) {
		typedef typename index_remap_type::const_iterator
							remap_iter;
		const remap_iter f(remap.find(i));
		if (f == remap.end())  // entry not found
			return i;
		else    return f->second;
	} else  return i;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deletes the last few (unused) entries of the pool.  
 */
template <class T>
void
instance_pool<T>::truncate(void) {
	const size_t n = remap.size();
	size_t i = 0;
	while (i<n) {
		this->pop_back();
		i++;
	}
	remap.clear();
}

#endif	// INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST

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

#endif	// __HAC_OBJECT_INST_INSTANCE_POOL_TCC__
