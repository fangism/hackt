/**
	\file "Object/inst/subinstance_manager.cc"
	Class implementation of the subinstance_manager.
	$Id: subinstance_manager.cc,v 1.7.10.1 2005/11/02 06:18:02 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "Object/inst/subinstance_manager.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/port_context.h"
#include "common/ICE.h"
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
	STACKTRACE_DTOR_VERBOSE;
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
void
subinstance_manager::push_back(const entry_value_type& v) {
	subinstance_array.push_back(v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::reserve(const size_t s) {
	util::reserve(subinstance_array, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i reference to formal instance, to be translated to an
		actual (unrolled) instance reference.  
	TODO: invariant check for matching definitions and types.  
 */
subinstance_manager::value_type
subinstance_manager::lookup_port_instance(
		const instance_collection_type& i) const {
	const size_t index = i.is_port_formal();
	if (index > subinstance_array.size()) {
	ICE(cerr, 
		cerr << "got port index of " << index
			<< " when limit is " << subinstance_array.size()
			<< endl;
		i.dump(cerr << "\twhile looking up: ") << endl;
//		cerr << "Here\'s the complete dump of this subinstance set: "
//			"at " << this << endl;
//		dump(cerr) << endl;
	)
	}
	INVARIANT(index);
	INVARIANT(index <= subinstance_array.size());
	return subinstance_array[index-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::collect_port_aliases(port_alias_tracker& t) const {
	const_iterator pi(subinstance_array.begin());
	const const_iterator pe(subinstance_array.end());
	for ( ; pi!=pe; pi++) {
		(*pi)->collect_port_aliases(t);
	}
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
	iterator pi(subinstance_array.begin());	// instance_collection_type
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
	Copied from footprint::create_dependent_types
	and module::create_dependent_types.
 */
good_bool
subinstance_manager::replay_internal_aliases(void) const {
	STACKTRACE_VERBOSE;
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e ; i++) {
		// creating dependent types also connects internal aliases
		if (!(*i)->create_dependent_types().good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
subinstance_manager::synchronize_port_actuals(this_type& l, this_type& r) {
	typedef	connection_references_type::const_iterator
						const_ref_iterator;
	array_type& la(l.subinstance_array);
	array_type& ra(r.subinstance_array);
	INVARIANT(la.size() == ra.size());
	iterator li(la.begin());	// instance_collection_type
	iterator ri(ra.begin());
	const iterator le(la.end());
	// const const_ref_iterator re(cr.end());
	for ( ; li!=le; li++, ri++) {
	// references may be NULL (no-connect)
		if (*li && *ri) {
			if (!(*li)->synchronize_actuals(**ri).good) {
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
/**
	Allocates each port entirely.  
 */
void
subinstance_manager::allocate(footprint& f) {
	STACKTRACE("subinstance_manager::allocate()");
	iterator i(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		(*i)->allocate_state(f);	// expands the whole collection
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: clean up the const-hack.  
 */
good_bool
subinstance_manager::create_state(const this_type& s, footprint& f) {
	STACKTRACE("subinstance_manager::create_state()");
	this_type& t(const_cast<this_type&>(s));
	INVARIANT(subinstance_array.size() == s.subinstance_array.size());
	iterator i(subinstance_array.begin());
	iterator j(t.subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++, j++) {
		// merge created state (instance_collection_types)
		const count_ptr<physical_instance_collection>& pi(*i), pj(*j);
		NEVER_NULL(pi);
		NEVER_NULL(pj);
		if (!pi->merge_created_state(*pj, f).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: write comment
 */
void
subinstance_manager::inherit_state(const this_type& s, const footprint& f) {
	STACKTRACE("subinstance_manager::inherit_state()");
	INVARIANT(subinstance_array.size() == s.subinstance_array.size());
	iterator i(subinstance_array.begin());
	const_iterator j(s.subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++, j++) {
		// merge created state (instance_collection_types)
		const count_ptr<physical_instance_collection>& pi(*i), pj(*j);
		NEVER_NULL(pi);
		NEVER_NULL(pj);
		pi->inherit_created_state(*pj, f);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::construct_port_context(port_member_context& pmc, 
		const footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	pmc.resize(size());
	size_t j = 0;
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; i++, j++) {
		(*i)->construct_port_context(pmc[j], ff);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::assign_footprint_frame(footprint_frame& ff, 
		const port_member_context& pmc) const {
	INVARIANT(pmc.size() == subinstance_array.size());
	size_t j = 0;
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; i++, j++) {
		(*i)->assign_footprint_frame(ff, pmc[j]);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	new footprint?
 */
void
subinstance_manager::cflat_aliases(const cflat_aliases_arg_type& c) const {
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		const count_ptr<physical_instance_collection>& pi(*i);
		pi->cflat_aliases(c);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT << "collected " << subinstance_array.size() <<
		" subinstances." << endl;
#endif
	m.collect_pointer_list(subinstance_array);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer_list(o, subinstance_array);
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT << "wrote " << subinstance_array.size() <<
		" subinstances." << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
subinstance_manager::load_object_base(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer_list(i, subinstance_array);
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT << "loaded " << subinstance_array.size() <<
		" subinstances." << endl;
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

