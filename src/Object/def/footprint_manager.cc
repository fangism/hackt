/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.12.74.4 2009/03/06 02:50:05 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <set>
#include "util/macros.h"
#include "Object/def/footprint_manager.h"
#include "Object/def/footprint.tcc"		// where is dtor ref'd?
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_param.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/port_alias_tracker.tcc"	// why are symbols ref'd?

#include "util/memory/count_ptr.tcc"
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

#if FOOTPRINT_HAS_PARAMS
footprint_entry::footprint_entry(footprint* f) : ptr_type(f) {
	NEVER_NULL(f);
}
#else
footprint_entry::footprint_entry() : ptr_type(new footprint) { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shared transfer of ownership, coercively.  
 */
footprint_entry::footprint_entry(const ptr_type& t) : ptr_type(t) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_entry::~footprint_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if FOOTPRINT_HAS_PARAMS
bool
footprint_entry::operator < (const footprint_entry& r) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(*this);
	NEVER_NULL(r);
	return (*this)->get_param_key() < r->get_param_key();
}
#endif

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
footprint_manager::footprint_manager(const size_t N
#if FOOTPRINT_OWNER_DEF
		, const definition_base& d
#endif
		) :
		parent_type(), _arity(N) {
	if (!_arity) {
#if FOOTPRINT_HAS_PARAMS
		insert(value_type(new footprint(const_param_expr_list()
#if FOOTPRINT_OWNER_DEF
			, d
#endif
			)));
#else
		parent_type::operator[](key_type());
#endif
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
footprint_manager::set_arity(const size_t a
#if FOOTPRINT_OWNER_DEF
		, const definition_base& d
#endif
		) {
	INVARIANT(!size());
	INVARIANT(!_arity);
	_arity = a;
	clear();
	if (!_arity) {
#if FOOTPRINT_HAS_PARAMS
		insert(value_type(new footprint(const_param_expr_list()
#if FOOTPRINT_OWNER_DEF
			, d
#endif
			)));
#else
		parent_type::operator[](key_type());
#endif
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
#if FOOTPRINT_HAS_PARAMS
			NEVER_NULL(*i);
			(*i)->get_param_key().dump(o, dc);
#else
			i->first.dump(o, dc);
#endif
			o << "> {" << endl;
			{
				INDENT_SECTION(o);
#if FOOTPRINT_HAS_PARAMS
				(*i)->dump_with_collections(o, df, dc);
#else
				NEVER_NULL(i->second);
				i->second->dump_with_collections(o, df, dc);
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
			o << auto_indent << '<';
#if FOOTPRINT_HAS_PARAMS
			NEVER_NULL(*i);
			(*i)->get_param_key().dump(o, dc);
#else
			i->first.dump(o, dc);
#endif
			o << "> {" << endl;
			{
				INDENT_SECTION(o);
#if FOOTPRINT_HAS_PARAMS
				(*i)->dump_with_collections(o, df, dc);
#else
				NEVER_NULL(i->second);
				i->second->dump_with_collections(o, df, dc);
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
	Intend to modify map with new footprint if not found.
	Wrapped associative lookup, making sure key has correct arity.  
	NOTE: key comparison is done with std::lexicographical_compare().  
	\param k set of constant template parameters as key 
		for footprint lookup.  
 */
footprint_manager::mapped_type&
footprint_manager::insert(const key_type& k
#if FOOTPRINT_OWNER_DEF
		, const definition_base& d
#endif
		) {
	INVARIANT(k.size() == _arity);
#if FOOTPRINT_HAS_PARAMS
	const footprint_entry temp(new footprint(k
#if FOOTPRINT_OWNER_DEF
		, d
#endif
		));
	const std::pair<parent_type::const_iterator, bool> i(insert(temp));
	// if inserted use new value, else use existing member
	INVARIANT(i.first != parent_type::end());
	NEVER_NULL(*i.first);
	return const_cast<mapped_type&>(**i.first);	// unfortunate
	// but remember that the param_key member which is used for
	// comparison is immutable
#else
	return *(parent_type::operator[](k));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This lookup uses find() to guarantee non-modification.
 */
footprint_manager::mapped_type&
footprint_manager::lookup(const key_type& k) const {
#if FOOTPRINT_OWNER_DEF
	const definition_base* bogus = NULL;
#endif
	INVARIANT(k.size() == _arity);
	const footprint_entry temp(new footprint(k
#if FOOTPRINT_OWNER_DEF
		, *bogus	// kludge - deref NULL!
#endif
		));
	const parent_type::const_iterator f(find(temp));
	// if inserted use new value, else use existing member
	INVARIANT(f != parent_type::end());
	NEVER_NULL(*f);
	return const_cast<mapped_type&>(**f);	// unfortunate
	// but remember that the param_key member which is used for
	// comparison is immutable
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to the only footprint in the map.  
	\pre this map has zero arity, only contains one footprint.  
 */
footprint_manager::mapped_type&
footprint_manager::only(
#if FOOTPRINT_OWNER_DEF
		const definition_base& d
#else
		void
#endif
		) {
	INVARIANT(!_arity);
	if (!size()) {
		// create if it doesn't yet exist
		insert(key_type()
#if FOOTPRINT_OWNER_DEF
			, d
#endif
			);
	}
#if FOOTPRINT_HAS_PARAMS
	const footprint_entry& ret(*begin());
	NEVER_NULL(ret);
	return *ret;
#else
	return *(begin()->second);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient method that handles both cases.  
	\param k the key is allowed to be NULL
 */
footprint_manager::mapped_type&
footprint_manager::insert(const count_ptr<const key_type>& k
#if FOOTPRINT_OWNER_DEF
		, const definition_base& d
#endif
		) {
	if (k) {
		INVARIANT(k->size() == arity());
		return insert(*k
#if FOOTPRINT_OWNER_DEF
		, d
#endif
		);
	} else {
		return only(
#if FOOTPRINT_OWNER_DEF
		d
#endif
		);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre must already have singleton footprint.
 */
footprint_manager::mapped_type&
footprint_manager::lookup(const count_ptr<const key_type>& k) const {
	if (k) {
		INVARIANT(k->size() == arity());
		return lookup(*k);
	} else {
		INVARIANT(size() == 1);
		const footprint_entry& ret(*begin());
		return *ret;
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
#if FOOTPRINT_HAS_PARAMS
	const footprint_entry& ret(*begin());
	NEVER_NULL(ret);
	return *ret;
#else
	return *(begin()->second);
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
#if FOOTPRINT_HAS_PARAMS
		NEVER_NULL(*i);
		(*i)->collect_transient_info(m);
#else
		i->first.collect_transient_info_base(m);
		NEVER_NULL(i->second);
		i->second->collect_transient_info(m);
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
#if FOOTPRINT_HAS_PARAMS
		const footprint_entry::ptr_type& p(*i);
#else
		i->first.write_object(m, o);	// same as write_object_base
		const footprint_entry::ptr_type& p(i->second);
#endif
		m.write_pointer(o, p);
#if FOOTPRINT_HAS_PARAMS
		// must store key separately for orderly reconstruction!
		p->write_param_key(m, o);
#endif
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Restores a footprint map.  
 */
void
footprint_manager::load_object_base(
		const persistent_object_manager& m, istream& i
#if FOOTPRINT_OWNER_DEF
		, const definition_base& d
#endif
		) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, _arity);
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		// read in key-value pairs
#if FOOTPRINT_HAS_PARAMS
		// \pre const_param_expr_list param_key must be loaded!
		footprint_entry::ptr_type _f;	// static_cast
		m.read_pointer(i, _f);
		NEVER_NULL(_f);
		_f->load_param_key(m, i
#if FOOTPRINT_OWNER_DEF
			, d
#endif
			);	// partial load only!
		insert(footprint_entry(_f));
#if FOOTPRINT_OWNER_DEF
	// TODO: ...
#endif
#else
		key_type temp_key;
		temp_key.load_object(m, i);
		// load value in-place
		INVARIANT(temp_key.size() == _arity);
		footprint_entry::ptr_type f;
		m.read_pointer(i, f);
		parent_type::operator[](temp_key) = f;
		// transfer ownership
		// let persistent_object_manager to the loading
#endif
	}
	INVARIANT(size() == s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

