/**
	\file "Object/inst/subinstance_manager.cc"
	Class implementation of the subinstance_manager.
	$Id: subinstance_manager.cc,v 1.31 2010/04/07 00:12:45 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/subinstance_manager.hh"
#include "Object/inst/physical_instance_collection.hh"
#include "Object/inst/port_alias_tracker.hh"
#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/type/fundamental_type_reference.hh"
#include "Object/common/dump_flags.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/inst/port_actual_collection.hh"
#include "Object/inst/bool_instance_collection.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "common/ICE.hh"
#include "Object/def/footprint.hh"
#include "util/IO_utils.hh"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/reserve.hh"
#include "util/indent.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::auto_indent;
using util::read_value;
using util::write_value;

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
/**
	Reminder: subinstance_array's pointers are NOT owned, 
	so we need access to the structure responsible for 
	owning the new copies, which accessed as the 
	target_footprint's instance_collection_pool_bundle.
	See instance_placeholder::unroll_port_only().
 */
void
subinstance_manager::deep_copy(const subinstance_manager& r, footprint& tf) {
	subinstance_array.resize(r.subinstance_array.size());
	// std::transform
	iterator j(subinstance_array.begin());
	const_iterator i(r.subinstance_array.begin());
	const const_iterator e(r.subinstance_array.end());
	for ( ; i!=e; ++i, ++j) {
		*j = (*i)->deep_copy(tf);
	}
	// relink_super_instance_alias();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
subinstance_manager::dump_ports(ostream& o, const dump_flags& df) const {
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
		// unqualified name is sufficient
		o << auto_indent << (*i)->get_name() << " = ";
		(*i)->dump(o, df) << endl;
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
	NOTE: This cannot be used to create an instance reference to 
		private members because this only looks up public ports.  
	TODO: write a version to make arbitrary member references.  
	\param i reference to formal instance, to be translated to an
		actual (unrolled) instance reference.  
	TODO: invariant check for matching definitions and types.  
	\return pointer to the subinstance-collection.
 */
subinstance_manager::value_type
subinstance_manager::lookup_port_instance(
		const lookup_arg_type& i) const {
	STACKTRACE_VERBOSE;
	const size_t s = subinstance_array.size();
	const size_t index = i.is_port_formal();
	if (!index || (index > s)) {
	ICE(cerr, 
		cerr << "got port index of " << index
			<< " when limit is " << s << endl;
		i.dump(cerr << "\twhile looking up: ", 
			dump_flags::verbose) << endl;
//		cerr << "Here\'s the complete dump of this subinstance set: "
//			"at " << this << endl;
//		dump_ports(cerr) << endl;
	)
	}
	INVARIANT(index);
	INVARIANT(index <= s);
	return subinstance_array[index-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if RECURSE_COLLECT_ALIASES
void
subinstance_manager::collect_port_aliases(port_alias_tracker& t) const {
	STACKTRACE_VERBOSE;
	const_iterator pi(subinstance_array.begin());
	const const_iterator pe(subinstance_array.end());
	for ( ; pi!=pe; pi++) {
		(*pi)->collect_port_aliases(t);
	}
}
#endif

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
subinstance_manager::__connect_ports(
		const connection_references_type& cr, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	typedef	connection_references_type::const_iterator
						const_ref_iterator;
	STACKTRACE_INDENT_PRINT("subinst_array.size() = " <<
		subinstance_array.size() << endl);
	STACKTRACE_INDENT_PRINT("cr.size() = " << cr.size() << endl);
	INVARIANT(subinstance_array.size() == cr.size());
	iterator pi(subinstance_array.begin());	// instance_collection_type
	const iterator pe(subinstance_array.end());
	const_ref_iterator ri(cr.begin());
	// const const_ref_iterator re(cr.end());
	for ( ; pi!=pe; ++pi, ++ri) {
	// references may be NULL (no-connect)
		if (*ri && (*ri)->connect_port(**pi, c).bad) {
			// already have error message
			return good_bool(false);
		}	// else good to continue
	}
	// all connections good
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Here, we reverse the order so that the earliest implicit ports
	are trailing defaults.  
	Compare the ordering in "AST/globals.cc" vs.
	"net/netgraph.cc".
	Rationale: keep !GND first
 */
good_bool
subinstance_manager::connect_implicit_ports(
		const connection_references_type& cr, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("subinst_array.size() = " <<
		subinstance_array.size() << endl);
	STACKTRACE_INDENT_PRINT("cr.size() = " << cr.size() << endl);
	INVARIANT(subinstance_array.size() >= cr.size());
	iterator pi(subinstance_array.begin());	// instance_collection_type
#if REVERSE_INSTANCE_SUPPLY_OVERRIDES
	typedef	connection_references_type::const_reverse_iterator
						const_ref_iterator;
	const_ref_iterator ri(cr.rbegin()), re(cr.rend());
#else
	typedef	connection_references_type::const_iterator
						const_ref_iterator;
	const_ref_iterator ri(cr.begin()), re(cr.end());
#endif
for ( ; ri!=re; ++pi, ++ri) {
// references may be NULL (no-connect)
	// see meta_instance_reference<Tag>::connect_port()
if (*ri) {
	STACKTRACE_INDENT_PRINT("overriding implicit port" << endl);
	const count_ptr<const simple_bool_meta_instance_reference>
		pr(ri->is_a<const simple_bool_meta_instance_reference>());
	NEVER_NULL(pr);
	// NOTE: this is not the standard symmetric connect, this only
	// updates the next-pointer of pr, and does not update flags.
	if (pr->connect_implicit_port(**pi, c).bad) {
		// already have error message
		return good_bool(false);
	}	// else good to continue
} else {
#if INSTANCE_SUPPLY_DISCONNECT
	STACKTRACE_INDENT_PRINT("disconnecting implicit port" << endl);
	// *disconnect* implicit port, if port is blank!
	// INVARIANT: such implicit ports are always at the leaves
	// of a connection union-find, and NEVER pointed-to, 
	// hence it is safe to "undo" such a connection.
	typedef	port_actual_collection<bool_tag>	bool_port;
	bool_port& bpr(IS_A(bool_port&, **pi));
	STACKTRACE_INDENT_PRINT("cast was good!" << endl);
	// assert-cast, for now, no need to handle other collection types
	bpr.only_element().disconnect_implicit_port();
#else
	STACKTRACE_INDENT_PRINT("not touching implicit port" << endl);
#endif
}
}
	// all connections good
	return good_bool(true);
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
subinstance_manager::connect_port_aliases_recursive(this_type& r, 
		target_context& c) {
	STACKTRACE_VERBOSE;
	INVARIANT(subinstance_array.size() == r.subinstance_array.size());
	iterator pi(subinstance_array.begin());	// instance_collection_type
	iterator ri(r.subinstance_array.begin());
	const iterator pe(subinstance_array.end());
	for ( ; pi!=pe; ++pi, ++ri) {
		NEVER_NULL(*ri);
		if (!(*ri)->connect_port_aliases_recursive(**pi, c).good) {
			// already have error message?
			return good_bool(false);
		}	// else good to continue
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called to reconnect/replay port aliases, 
	whose types and attributes have already been checked and
	matched.
	Follows same outline as connect_port_aliases_recursive().
 */
void
subinstance_manager::reconnect_port_aliases_recursive(this_type& r) {
	STACKTRACE_VERBOSE;
	INVARIANT(subinstance_array.size() == r.subinstance_array.size());
	iterator pi(subinstance_array.begin());	// instance_collection_type
	iterator ri(r.subinstance_array.begin());
	const iterator pe(subinstance_array.end());
	for ( ; pi!=pe; ++pi, ++ri) {
		NEVER_NULL(*ri);
		(*ri)->reconnect_port_aliases_recursive(**pi);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-links all sub-instances to a parent instance.  
	Needed to maintain the hierarchy.  
 */
void
subinstance_manager::relink_super_instance_alias(
		const substructure_alias& p) {
	STACKTRACE_VERBOSE;
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
	TODO: give this a return value for error handling.  
 */
void
subinstance_manager::allocate_subinstances(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		if (!(*i)->allocate_local_instance_ids(f).good) {
			cerr << "Error allocating local instance IDs" << endl;
			THROW_EXIT;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this doesn't use the visitee's accept method.  
	This forwards the calls to physical_instance_collection::accept.
 */
void
subinstance_manager::accept(alias_visitor& v) const {
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; i++) {
		const never_ptr<physical_instance_collection> pi(*i);
		pi->accept(v);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is there really a need for this when collections are managed by 
	the footprint's pool bundles?  Probably not.  
 */
void
subinstance_manager::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	STACKTRACE_PERSISTENT_PRINT("collected " << subinstance_array.size() <<
		" subinstances." << endl);
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; ++i) {
		(*i)->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need footprint to manage and translate pooled collections.  
 */
void
subinstance_manager::write_object_base(const footprint& f, ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	const size_t s = subinstance_array.size();
	write_value(o, s);
	const_iterator i(subinstance_array.begin());
	const const_iterator e(subinstance_array.end());
	for ( ; i!=e; ++i) {
		// doesn't raelly write a pointer, but index
		(*i)->write_local_pointer(f, o);
	}
	STACKTRACE_PERSISTENT_PRINT("wrote " << subinstance_array.size() <<
		" subinstances." << endl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is there a potential ordering problem with loading?
	Might loading have to be broken up into more phases?
	Load subinstances AFTER collections have been loaded, 
	or at least allocated?
	Footprint should probably pre-allocate before loading...
	\pre footprint needs to be mapped out already, may need a 
		two-pass implementation.  
 */
void
subinstance_manager::load_object_base(const footprint& f, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	size_t s;
	read_value(i, s);
	subinstance_array.resize(s);
	iterator j(subinstance_array.begin());
	const iterator e(subinstance_array.end());
	for ( ; j!=e; ++j) {
		*j = f.read_pointer(i).is_a<physical_instance_collection>();
		NEVER_NULL(*j);
	}
	STACKTRACE_INDENT_PRINT("loaded " << subinstance_array.size() <<
		" subinstances." << endl);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

