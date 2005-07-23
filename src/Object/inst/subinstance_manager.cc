/**
	\file "Object/inst/subinstance_manager.cc"
	Class implementation of the subinstance_manager.
	$Id: subinstance_manager.cc,v 1.3 2005/07/23 06:52:42 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "Object/inst/subinstance_manager.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/type/fundamental_type_reference.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.h"
#include "util/indent.h"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::auto_indent;

//=============================================================================
// class subinstance_manager method definitions

subinstance_manager::subinstance_manager() : subinstance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: not your everyday copy-constructor, needs to 
	re-establish local connections.  
 */
subinstance_manager::subinstance_manager(const this_type& s) :
	subinstance_array(s.subinstance_array) {
	// 2005-07-16: bug fixed, forgot to copy the array!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subinstance_manager::~subinstance_manager() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
subinstance_manager::dump(ostream& o) const {
if (subinstance_array.empty()) {
	return o;
} else {
	o << '(' << endl;
	{
	INDENT_SECTION(o);
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->dump(o << auto_indent) << endl;
	}
	}
	return o << auto_indent << ')';
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i reference to formal instance, to be translated to an
		actual (unrolled) instance reference.  
	TODO: invariant check for matching definitions and types.  
 */
subinstance_manager::value_type
subinstance_manager::lookup_port_instance(
		const instance_collection_base& i) const {
	const size_t index = i.is_port_formal();
	if (index > subinstance_array.size()) {
		cerr << "Internal compiler error: got port index of " << index
			<< " when limit is " << subinstance_array.size()
			<< endl;
		i.dump(cerr << "\twhile looking up: ") << endl;
#if 0
		cerr << "Here\'s the complete dump of this subinstance set: "
			"at " << this << endl;
		dump(cerr) << endl;
#endif
		THROW_EXIT;
	}
	INVARIANT(index);
	INVARIANT(index <= subinstance_array.size());
	return subinstance_array[index-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param cr sequence of meta instance references to resolve and 
		connect to ports.  
	\pre list sizes (ports vs. references) must be equal, 
		already checked earlier.  
	TODO: future, let port instantiation be on-demand, 
		so check whether or not this is expanded first.  
 */
good_bool
subinstance_manager::connect_ports(
		const connection_references_type& cr, 
		const unroll_context& c) {
	typedef	connection_references_type::const_iterator
						const_ref_iterator;
	INVARIANT(subinstance_array.size() == cr.size());
	iterator pi(subinstance_array.begin());	// instance_collection_base
	const iterator pe(subinstance_array.end());
	const_ref_iterator ri(cr.begin());
	// const const_ref_iterator re(cr.end());
	for ( ; pi!=pe; pi++, ri++) {
	// references may be NULL (no-connect)
	if (*ri) {
		if ((*ri)->connect_port(**pi, c).bad) {
			// already have error message
			return good_bool(false);
		}	// else good to continue
	}
	}
	// all connections good
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-links all sub-instances to a parent instance.  
	Needed to maintain the hierarchy.  
 */
void
subinstance_manager::relink_super_instance_alias(
		const substructure_alias& p) {
	iterator i(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->relink_super_instance(p);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	cerr << "collected " << subinstance_array.size() << " subinstances."
		<< endl;
#endif
	m.collect_pointer_list(subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_VERBOSE;
	m.write_pointer_list(o, subinstance_array);
#if ENABLE_STACKTRACE
	cerr << "wrote " << subinstance_array.size() << " subinstances."
		<< endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::load_object_base(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_VERBOSE;
	m.read_pointer_list(i, subinstance_array);
#if ENABLE_STACKTRACE
	cerr << "loaded " << subinstance_array.size() << " subinstances."
		<< endl;
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

