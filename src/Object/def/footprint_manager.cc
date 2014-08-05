/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.14 2009/03/14 01:46:21 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <set>
#include "util/macros.h"
#include "Object/def/footprint_manager.hh"
#include "Object/def/footprint.tcc"		// where is dtor ref'd?
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/const_param.hh"
#include "Object/common/dump_flags.hh"
#include "Object/inst/port_alias_tracker.tcc"	// why are symbols ref'd?
#include "Object/traits/instance_traits.hh"

#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.hh"
#include "util/IO_utils.hh"
#include "util/indent.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;
using util::auto_indent;

//=============================================================================
// class footprint_entry method definitions

footprint_entry::footprint_entry(footprint* f) : ptr_type(f) {
	NEVER_NULL(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shared transfer of ownership, coercively.  
 */
footprint_entry::footprint_entry(const ptr_type& t) : ptr_type(t) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_entry::~footprint_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
footprint_entry::operator < (const footprint_entry& r) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(*this);
	NEVER_NULL(r);
	return (*this)->get_param_key() < r->get_param_key();
}

//=============================================================================
// class footprint_manager method definitions

// see if this default constructor can be avoided
footprint_manager::footprint_manager() :
		parent_type(), _arity(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	if 0-dimensions, then initialize with exactly one footprint.  
 */
footprint_manager::footprint_manager(const size_t N, 
		const definition_base& d) :
		parent_type(), _arity(N) {
	if (!_arity) {
		insert(value_type(new footprint(const_param_expr_list(), d)));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::~footprint_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This resets the arity of the footprint map keys.  
	Warning: calling this will always clear the map!
 */
void
footprint_manager::set_arity(const size_t a, const definition_base& d) {
	INVARIANT(!size());
	INVARIANT(!_arity);
	_arity = a;
	clear();
	if (!_arity) {
		insert(value_type(new footprint(const_param_expr_list(), d)));
	}
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
			o << auto_indent << '<';
			NEVER_NULL(*i);
			(*i)->get_param_key().dump(o, dc);
			o << "> {" << endl;
			{
				INDENT_SECTION(o);
				(*i)->dump_with_collections(o, df, dc);
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
			o << auto_indent << '<';
			NEVER_NULL(*i);
			(*i)->get_param_key().dump(o, dc);
			o << "> {" << endl;
			{
				INDENT_SECTION(o);
				(*i)->dump_with_collections(o, df, dc);
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
	Intend to modify map with new footprint if not found.
	Wrapped associative lookup, making sure key has correct arity.  
	NOTE: key comparison is done with std::lexicographical_compare().  
	\param k set of constant template parameters as key 
		for footprint lookup.  
 */
footprint_manager::mapped_type&
footprint_manager::insert(const key_type& k, const definition_base& d) {
	INVARIANT(k.size() == _arity);
	// ALERT: AVOID EXPENSIVE ALLOCATION OF TEMPORARY
	const footprint_entry
		temp(new footprint(k, util::uninitialized_tag));
	const std::pair<parent_type::const_iterator, bool> i(insert(temp));
	// if inserted use new value, else use existing member
	INVARIANT(i.first != parent_type::end());
	NEVER_NULL(*i.first);
	mapped_type& ret(const_cast<mapped_type&>(**i.first));	// unfortunate
	// but remember that the param_key member which is used for
	// comparison is immutable
	if (i.second) {
		// then it was actually newly inserted, finish construction
		ret.__reconstruct(k, d);
	}	// else existing copy is assumed to be properly constructed
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This lookup uses find() to guarantee non-modification.
 */
footprint_manager::mapped_type*
footprint_manager::lookup(const key_type& k) const {
	INVARIANT(k.size() == _arity);
	// ALERT: AVOID EXPENSIVE ALLOCATION OF TEMPORARY
	const footprint_entry
		temp(new footprint(k, util::uninitialized_tag));
		// kludge - deref NULL!
	const parent_type::const_iterator f(find(temp));
	// if inserted use new value, else use existing member
	if (f != parent_type::end()) {
		NEVER_NULL(*f);
		return &const_cast<mapped_type&>(**f);	// unfortunate
		// but remember that the param_key member which is used for
		// comparison is immutable
	} else {
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
footprint_manager::mapped_type&
footprint_manager::only(const definition_base& d) {
	INVARIANT(!_arity);
	if (!size()) {
		// create if it doesn't yet exist
		insert(key_type(), d);
	}
	const footprint_entry& ret(*begin());
	NEVER_NULL(ret);
	return *ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient method that handles both cases.  
	\param k the key is allowed to be NULL
 */
footprint_manager::mapped_type&
footprint_manager::insert(const count_ptr<const key_type>& k,
		const definition_base& d) {
	if (k) {
		INVARIANT(k->size() == arity());
		return insert(*k, d);
	} else {
		return only(d);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre must already have singleton footprint.
 */
footprint_manager::mapped_type*
footprint_manager::lookup(const count_ptr<const key_type>& k) const {
	if (k) {
		INVARIANT(k->size() == arity());
		return lookup(*k);
	} else {
		INVARIANT(size() == 1);
		const footprint_entry& ret(*begin());
		return &*ret;
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
	const footprint_entry& ret(*begin());
	NEVER_NULL(ret);
	return *ret;
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
		NEVER_NULL(*i);
		(*i)->collect_transient_info(m);
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
		const footprint_entry::ptr_type& p(*i);
		m.write_pointer(o, p);
		// must store key separately for orderly reconstruction!
		p->write_param_key(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores a footprint map.  
 */
void
footprint_manager::load_object_base(
		const persistent_object_manager& m, istream& i,
		const definition_base& d) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, _arity);
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		// read in key-value pairs
		// \pre const_param_expr_list param_key must be loaded!
		footprint_entry::ptr_type _f;	// static_cast
		m.read_pointer(i, _f);
		NEVER_NULL(_f);
		_f->load_param_key(m, i, d);	// partial load only!
		insert(footprint_entry(_f));
	}
	INVARIANT(size() == s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

