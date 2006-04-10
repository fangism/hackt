/**
	\file "Object/def/footprint_manager.cc"
	Implementation of footprint_manager class. 
	$Id: footprint_manager.cc,v 1.7.8.1 2006/04/10 23:21:25 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "util/macros.h"
#include "Object/def/footprint_manager.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/common/dump_flags.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "util/indent.h"

#if GROUPED_DIRECTIVE_ARGUMENTS
#include <set>
#endif

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;

//=============================================================================
// class footprint_manager method definitions

// see if this default constructor can be avoided
footprint_manager::footprint_manager() :
		parent_type(), _arity(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::footprint_manager(const size_t N) :
		parent_type(), _arity(N) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_manager::~footprint_manager() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_manager::set_arity(const size_t a) {
	INVARIANT(!size());
	INVARIANT(!_arity);
	_arity = a;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_manager::dump(ostream& o) const {
	return dump(o, dump_flags::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_manager::dump(ostream& o, const dump_flags& df) const {
if (_arity) {
	o << "footprint collection: {" << endl;
	{	// indentation scope
		INDENT_SECTION(o);
		const_iterator i(begin());
		const const_iterator e(end());
		for ( ; i!=e; i++) {
			i->first.dump(o << auto_indent << '<', 
				expr_dump_context::default_value)
				<< "> {" << endl;
			{
				INDENT_SECTION(o);
				i->second.dump_with_collections(o, df);
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
		only().dump_with_collections(o, df);
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
	return parent_type::operator[](k);
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
	return begin()->second;
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
	return begin()->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects reachable pointers in the footprint_map.  
 */
void
footprint_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		i->first.collect_transient_info_base(m);
		i->second.collect_transient_info_base(m);
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
	write_value(o, _arity);
	write_value(o, size());
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		i->first.write_object(m, o);	// same as write_object_base
		i->second.write_object_base(m, o);
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
	for ( ; j<s; j++) {
		// read in key-value pairs
		key_type temp_key;
		temp_key.load_object(m, i);
		// load value in-place
		INVARIANT(temp_key.size() == _arity);
		(*this)[temp_key].load_object_base(m, i);
	}
	INVARIANT(size() == s);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

