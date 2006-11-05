/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.10.4.2 2006/11/05 01:23:07 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <set>
#include "util/macros.h"
#include "Object/def/footprint_manager.h"
#if HEAP_ALLOCATE_FOOTPRINTS
#include "Object/def/footprint.h"
#endif
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"

// need these until footprint is heap allocated
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"

#include "util/persistent_object_manager.tcc"
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
// class footprint_entry method definitions

#if HEAP_ALLOCATE_FOOTPRINTS
footprint_entry::footprint_entry() : ptr_type(new footprint) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructive transfer of ownership, coercively.  
 */
footprint_entry::footprint_entry(const footprint_entry& t) :
		ptr_type(const_cast<footprint_entry&>(t)) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_entry::~footprint_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_entry&
footprint_entry::operator = (ptr_type& p) {
	static_cast<ptr_type&>(*this) = p;
	return *this;
}

#endif	// HEAP_ALLOCATE_FOOTPRINTS

//=============================================================================
// class footprint_manager method definitions

// see if this default constructor can be avoided
footprint_manager::footprint_manager() :
		parent_type(), _arity(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::footprint_manager(const size_t N) :
		parent_type(), _arity(N) {
#if HEAP_ALLOCATE_FOOTPRINTS
	if (!_arity) {
		parent_type::operator[](key_type());
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::~footprint_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Warning: calling this will always clear the map!
 */
void
footprint_manager::set_arity(const size_t a) {
	INVARIANT(!size());
	INVARIANT(!_arity);
	_arity = a;
#if HEAP_ALLOCATE_FOOTPRINTS
	clear();
	if (!_arity) {
		parent_type::operator[](key_type());
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_manager::dump(ostream& o, const expr_dump_context& dc) const {
//	return dump(o, dump_flags::default_value);
	// const dump_flags& df(dump_flags::no_leading_scope);
	const dump_flags& df(dump_flags::default_value);
if (_arity) {
	o << "footprint collection: {" << endl;
	{	// indentation scope
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++) {
			i->first.dump(o << auto_indent << '<', dc)
				<< "> {" << endl;
			{
				INDENT_SECTION(o);
#if HEAP_ALLOCATE_FOOTPRINTS
				NEVER_NULL(i->second);
				i->second->dump_with_collections(o, df, dc);
#else
				i->second.dump_with_collections(o, df, dc);
#endif
			}
			o << auto_indent << '}' << endl;
		}
	}
	return o << auto_indent << '}';
} else if (size()) {
	// check size to see of the only complete type has been unrolled
	o << "footprint: {" << endl;
	{
		INDENT_SECTION(o);
		only().dump_with_collections(o, df, dc);
	}
	return o << auto_indent << '}';
} else {
	return o;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_manager::dump(ostream& o, const dump_flags& df) const {
	const expr_dump_context& dc(expr_dump_context::default_value);
if (_arity) {
	o << "footprint collection: {" << endl;
	{	// indentation scope
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++) {
			i->first.dump(o << auto_indent << '<', dc)
				<< "> {" << endl;
			{
				INDENT_SECTION(o);
#if HEAP_ALLOCATE_FOOTPRINTS
				NEVER_NULL(i->second);
				i->second->dump_with_collections(o, df, dc);
#else
				i->second.dump_with_collections(o, df, dc);
#endif
			}
			o << auto_indent << '}' << endl;
		}
	}
	return o << auto_indent << '}';
} else if (size()) {
	// check size to see of the only complete type has been unrolled
	o << "footprint: {" << endl;
	{
		INDENT_SECTION(o);
		only().dump_with_collections(o, df, dc);
	}
	return o << auto_indent << '}';
} else {
	return o;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped associative lookup, making sure key has correct arity.  
	NOTE: key comparison is done with std::lexicographical_compare().  
	\param k set of constant template parameters as key 
		for footprint lookup.  
 */
footprint_manager::mapped_type&
footprint_manager::operator [] (const key_type& k) {
	INVARIANT(k.size() == _arity);
#if HEAP_ALLOCATE_FOOTPRINTS
	return *(parent_type::operator[](k));
#else
	return parent_type::operator[](k);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
footprint_manager::mapped_type&
footprint_manager::only(void) {
	INVARIANT(!_arity);
	if (!size()) {
		(*this)[key_type()];
	}
#if HEAP_ALLOCATE_FOOTPRINTS
	return *(begin()->second);
#else
	return begin()->second;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient method that handles both cases.  
 */
footprint_manager::mapped_type&
footprint_manager::operator [] (const count_ptr<const key_type>& k) {
	if (k) {
		INVARIANT(k->size() == arity());
		return (*this)[*k];
	} else {
		return only();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
const footprint_manager::mapped_type&
footprint_manager::only(void) const {
	INVARIANT(!_arity);
	INVARIANT(size() == 1);
#if HEAP_ALLOCATE_FOOTPRINTS
	return *(begin()->second);
#else
	return begin()->second;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects reachable pointers in the footprint_map.  
 */
void
footprint_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	STACKTRACE_INDENT_PRINT("this @ " << this << ": size = " << size() << endl);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		i->first.collect_transient_info_base(m);
#if HEAP_ALLOCATE_FOOTPRINTS
		NEVER_NULL(i->second);
		i->second->collect_transient_info(m);
#else
		i->second.collect_transient_info_base(m);
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves a footprint map.  
 */
void
footprint_manager::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	STACKTRACE_INDENT_PRINT("this @ " << this << ": size = " << size() << endl);
	write_value(o, _arity);
	write_value(o, size());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		i->first.write_object(m, o);	// same as write_object_base
#if HEAP_ALLOCATE_FOOTPRINTS
		const excl_ptr<footprint>& p(i->second);
		m.write_pointer(o, p);
#else
		i->second.write_object_base(m, o);
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores a footprint map.  
 */
void
footprint_manager::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, _arity);
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		// read in key-value pairs
		key_type temp_key;
		temp_key.load_object(m, i);
		// load value in-place
		INVARIANT(temp_key.size() == _arity);
#if HEAP_ALLOCATE_FOOTPRINTS
		excl_ptr<footprint> f;
		m.read_pointer(i, f);
		parent_type::operator[](temp_key) = f;
		// transfer ownership
		// let persistent_object_manager to the loading
#else
		(*this)[temp_key].load_object_base(m, i);
#endif
	}
	INVARIANT(size() == s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

