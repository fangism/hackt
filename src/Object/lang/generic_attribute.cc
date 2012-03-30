/**
	\file "Object/lang/generic_attribute.cc"
	Implementation of generic attributes.  
	$Id: generic_attribute.cc,v 1.2 2009/09/14 21:17:05 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <iterator>
#include <algorithm>
#include "Object/expr/param_expr.h"
#include "Object/lang/generic_attribute.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/def/template_formals_manager.h"
#include "Object/type/template_actuals.h"

#include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/stacktrace.h"

//=============================================================================

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;

//=============================================================================
// class attribute method definitions

generic_attribute::generic_attribute() : key(), values() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_attribute::generic_attribute(const string& k) : key(k), values() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_attribute::generic_attribute(const resolved_attribute& r) :
	key(r.key), values(new values_type(*r.values)) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_attribute::~generic_attribute() { }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const generic_attribute::values_type>
generic_attribute::get_values(void) const {
	return values;		// cast to const
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_attribute::operator bool () const {
	return key.length();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Determining whether or not attribute values are equivalent.  
 */
bool
generic_attribute::operator == (const generic_attribute& a) const {
	if (key != a.key)	return false;
	if (!values && !a.values)	return true;
	if (values ^ a.values)		return false;
	if (values->size() != a.values->size())	return false;
	return std::equal(values->begin(), values->end(), a.values->begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute::push_back(const value_type& e) {
	if (!values) {
		values = count_ptr<values_type>(new values_type);
		NEVER_NULL(values);
	}
	values->push_back(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
generic_attribute::dump(ostream& o, const expr_dump_context& c) const {
	o << key;
	if (values) {
		values->dump(o << '=', c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param_expr_list>
generic_attribute::unroll_values(const unroll_context& c) const {
if (values) {
	const count_ptr<const const_param_expr_list>
		ret(values->unroll_resolve_rvalues(c, values));
	if (!ret) {
		cerr << "Error resolving attribute values!" << endl;
	}
	return ret;
} else {
	// return non-null empty value list, b/c null => error
	return count_ptr<const const_param_expr_list>(
		new const_param_expr_list);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does NOT register itself, because called by reference, not pointer.  
 */
void
generic_attribute::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (values)
		values->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute::write_object(const persistent_object_manager& m, ostream& o) const {
	INVARIANT(key.length());
	write_value(o, key);
	m.write_pointer(o, values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute::load_object(const persistent_object_manager& m, istream& i) {
	read_value(i, key);
	INVARIANT(key.length());
	m.read_pointer(i, values);
}

//=============================================================================
// class generic_attribute_list_type method definitions

generic_attribute_list_type::generic_attribute_list_type(
		const resolved_attribute_list_type& a) : parent_type() {
	reserve(a.size());
	copy(a.begin(), a.end(), back_inserter(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
generic_attribute_list_type::dump(ostream& o,
		const expr_dump_context& c) const {
	const_iterator i(begin()), e(end());
if (i!=e) {
	i->dump(o, c);
	for (++i; i!=e; ++i) {
		i->dump(o << ';', c);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute_list_type::collect_transient_info_base(
		persistent_object_manager& m) const {
	for_each(begin(), end(), util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute_list_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	util::write_persistent_sequence(m, o, *this);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_attribute_list_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	util::read_persistent_sequence_resize(m, i, *this);
}

//=============================================================================
// class resolved_attribute method definitions

resolved_attribute::resolved_attribute() : key(), values(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
resolved_attribute::resolved_attribute(const string& k) : key(k), values() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
resolved_attribute::resolved_attribute(const string& k, 
		const values_ptr_type& v) : key(k), values(v) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
resolved_attribute::~resolved_attribute() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
resolved_attribute::dump(ostream& o) const {
	o << key;
	if (values) {
		values->dump(o << '=');
	}
	return o;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (values) {
		values->collect_transient_info(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, key);
	m.write_pointer(o, values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute::load_object(const persistent_object_manager& m, 
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, key);
	m.read_pointer(i, values);
}

//=============================================================================
// class resolved_attribute_list_type method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
resolved_attribute_list_type::dump(ostream& o) const {
	const_iterator i(begin()), e(end());
if (i!=e) {
	i->dump(o);
	for (++i; i!=e; ++i) {
		i->dump(o << ';');
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute_list_type::filter_key(const string& k,
		resolved_attribute_list_type& ret) const {
	// std::copy_if(...)
	resolved_attribute_list_type::const_iterator
		i(begin()), e(end());
	for ( ; i!=e; ++i) {
	if (i->key == k) {
		ret.push_back(*i);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute_list_type::collect_transient_info_base(
		persistent_object_manager& m) const {
	for_each(begin(), end(), util::persistent_collector_ref(m));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute_list_type::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	util::write_persistent_sequence(m, o, *this);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
resolved_attribute_list_type::load_object_base(
		const persistent_object_manager& m, istream& i) {
	util::read_persistent_sequence_resize(m, i, *this);
}


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

