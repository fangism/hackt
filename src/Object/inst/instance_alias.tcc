/**
	\file "Object/inst/instance_alias.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life, and then was split from
		"Object/inst/instance_collection.tcc".
	$Id: instance_alias.tcc,v 1.19.6.3 2006/04/07 22:54:29 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.h"

#include "Object/inst/instance_collection.h"
#include "Object/inst/alias_actuals.tcc"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/substructure_alias_base.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"		// for debug only
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/def/definition_base.h"
#include "Object/type/canonical_type.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "Object/common/cflat_args.h"
#include "Object/inst/alias_printer.h"
#include "common/ICE.h"

#include "util/multikey_set.tcc"
#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/sstream.h"
#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"


namespace HAC {
namespace entity {
using std::string;
using std::ostringstream;
using std::_Select1st;
#include "util/using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class instance_alias_info method definitions

/**
	NOTE: this destructor is non-virtual (protected too) because 
	deleting this type directly is strictly forbidden and prevented.  
	It is only ever invoked by children classes' destructors.  
	NOTE: not making it non-virtual because warning flags 
		catch this and Werror rejects it.  
		(Is there an attribute for such exceptions?)
	Playing with fire here.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
// inline
INSTANCE_ALIAS_INFO_CLASS::~instance_alias_info() {
	STACKTRACE_DTOR_VERBOSE;
#if STACKTRACE_DESTRUCTORS
	STACKTRACE_INDENT << "destroying " << this << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This check is cheap and critical, do not disable it.  
	TODO: add more checks for relationship with parent instance
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::check(const container_type* p) const {
	if (this->container && this->container != p) {
	ICE(cerr, 
		cerr << "FATAL: Inconsistent instance_alias_info parent-child!"
			<< endl;
		cerr << "this->container = " << &*this->container << endl;
		this->container->dump(cerr, dump_flags::verbose) << endl;
		cerr << "should point to: " << p << endl;
		p->dump(cerr, dump_flags::verbose) << endl;
	);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively expands the public ports of the instance hierarchy.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::instantiate(const container_ptr_type p, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(p);
	INVARIANT(!this->container);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "this->container (old) = " << &*this->container << endl;
#endif
	this->container = p;
	substructure_parent_type::unroll_port_instances(*this->container, c);
	// do we ever want to instantiate more than the ports?
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "this->container (new) = " << &*this->container << endl;
	this->dump_hierarchical_name(STACKTRACE_INDENT << "instantiate: ")
		<< endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
never_ptr<const physical_instance_collection>
INSTANCE_ALIAS_INFO_CLASS::get_container_base(void) const {
	return this->container;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the owner of the top-most instance of which this is 
	a member.  So a.b.c will return the owner of a.  
	NOTE: this is pretty much template independent, except for the 
		first initialization.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
never_ptr<const physical_instance_collection>
INSTANCE_ALIAS_INFO_CLASS::get_supermost_collection(void) const {
	never_ptr<const physical_instance_collection> c(this->container);
	NEVER_NULL(c);
{
	// substructure_alias
	instance_collection_base::super_instance_ptr_type
		p(c->get_super_instance());
	while (p) {
		c = p->get_container_base();
		NEVER_NULL(c);
		p = c->get_super_instance();
	}
}
	NEVER_NULL(c);
	return c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
never_ptr<const substructure_alias>
INSTANCE_ALIAS_INFO_CLASS::get_supermost_substructure(void) const {
	never_ptr<const physical_instance_collection> c(this->container);
	NEVER_NULL(c);
	instance_collection_base::super_instance_ptr_type ret(NULL);
{
	// substructure_alias
	instance_collection_base::super_instance_ptr_type
		p(c->get_super_instance());
	NEVER_NULL(p);		// first-time
	while (p) {
		ret = p;
		c = p->get_container_base();
		NEVER_NULL(c);
		p = c->get_super_instance();
	}
}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if this hierarchical alias is a port of the current
		scope or is a subinstance of a port.  
	NOTE: this is pretty much template independent.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::is_port_alias(void) const {
	const never_ptr<const physical_instance_collection>
		c(this->get_supermost_collection());
	const never_ptr<const definition_base>
		def(c->get_owner(). template is_a<const definition_base>());
	if (def) {
		return def->lookup_port_formal(c->get_name());
	} else {
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unlike retrace, this does NOT follow super-instances
	of the argument.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
physical_instance_collection&
INSTANCE_ALIAS_INFO_CLASS::trace_collection(
		const substructure_alias& sup) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "this: ") << endl;
	sup.dump_hierarchical_name(STACKTRACE_INDENT << "arg : ", 
		dump_flags::default_value) << endl;
#endif
	const instance_collection_base::super_instance_ptr_type
		thisp(this->container->get_super_instance());
	if (thisp) {
#if ENABLE_STACKTRACE
		thisp->dump_hierarchical_name(STACKTRACE_INDENT << "parent: ", 
			dump_flags::default_value) << endl;
#endif
		// need to lookup parent instance first
		// p points to a substructured alias
		const substructure_alias&
			pp(thisp->__trace_alias_base(sup));
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "looking up member: " <<
			this->container->get_name() << endl;
#endif
		return *pp.lookup_port_instance(*this->container);
	} else {
		// This case cannot be reached when this is 
		// a subinstanceless type.
		// then we are at top-most level, terminate recursion
		// from the type of sup, lookup the member
		physical_instance_collection&
			ret(*sup.lookup_port_instance(*this->container));
#if ENABLE_STACKTRACE
		ret.dump(STACKTRACE_INDENT << "ret: ",
			dump_flags::default_value) << endl;
#endif
		return ret;
	}
	// then alias type (indexed or keyless) should lookup
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Much simplified algorithm for assigning unique local instance ids. 
	Also performs path compression.  
	TODO: check for alias actuals match.  
	TODO: error handling on allocate_subinstances. 
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
size_t
INSTANCE_ALIAS_INFO_CLASS::assign_local_instance_id(footprint& f) {
	STACKTRACE_VERBOSE;
	if (this->instance_index)
		return this->instance_index;
	// compress path to canonical
	const pseudo_iterator i(this->find());
	// i points to the canonical alias for this set
	if (!i->instance_index) {
		// only allocate if this is the canonical member of the 
		// union-find set, otherwise, just copy.
		// for now the creator will be the canonical back-reference
		typename instance_type::pool_type&
			the_pool(f.template get_pool<Tag>());
		i->instance_index = the_pool.allocate(instance_type(*i));
		// also need to recursively allocate subinstances ids
		i->allocate_subinstances(f);
	}
	// by here, the canonical alias in this set has been processed
	if (&*i != this) {
		// just copy from the canonical alias
		INVARIANT(i->instance_index);
		this->instance_index = i->instance_index;
		// also need to recursively allocate subinstances ids
		this->allocate_subinstances(f);
	}
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT)
		<< " assigned id: " << this << " = "
		<< this->instance_index << endl;
#endif
	INVARIANT(this->instance_index);
	return this->instance_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares collection types of the two instances and then
	TODO: compares their relaxed actuals (if applicable).
	NOTE: relaxed actuals are compared by synchronize/compare_actuals().  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::must_match_type(const this_type& a) const {
	return this->container->must_be_collectibly_type_equivalent(
		*a.container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version of connect is symmetric and tailored to port connections,
	and is called from simple_meta_instance_reference::connect_port.  
	This effectively checks for connectible-type-equivalence.  
	\param l an alias of this type.  
	\param r an alias of this type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::checked_connect_port(this_type& l, this_type& r) {
	if (!l.must_match_type(r)) {
		// already have error message
		return good_bool(false);
	}
	instance_alias_base_type& ll(AS_A(instance_alias_base_type&, l));
	instance_alias_base_type& rr(AS_A(instance_alias_base_type&, r));
	ll = rr;			// union
	return l.connect_port_aliases_recursive(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version of connect is slightly asymmetric and tailored for
	connecting aliases, called by alias_connection::unroll.  
	This effectively checks for connectible-type-equivalence.  
	\param l is the (unchanging) leftmost instance reference, 
		which always contains relaxed actuals IF anything in
		the connection list has actuals.  
	\param r is the varying instance alias referenced (iterating
		along the list) and may inherit the actuals argument.
	\param a the designated relaxed actuals pointer used to propagate
		throughout the alias ring.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::checked_connect_alias(this_type& l, this_type& r) {
	if (!l.must_match_type(r)) {
		// already have error message
		return good_bool(false);
	}
	instance_alias_base_type& ll(AS_A(instance_alias_base_type&, l));
	instance_alias_base_type& rr(AS_A(instance_alias_base_type&, r));
	ll = rr;	// union
	return l.connect_port_aliases_recursive(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by top-level.  
	This reserves the space required by the footprint corresponding
		to this instance's complete canonical type.
	\param ff the footprint frame to initialize using this
		instance's canonical type.  
	\param sm the global state allocation manager, 
		used to allocate private subinstances not covered
		by the port aliases.  
	\param ind is the globally assigned id for this particular instance
		used to identify this as a parent to subinstances.  
	TODO: after assigning globally assigned ports, 
	allocate the remaining unassigned internal substructures.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::allocate_assign_subinstance_footprint_frame(
		footprint_frame& ff, state_manager& sm,
		const port_member_context& pmc, const size_t ind) const {
	STACKTRACE_VERBOSE;
	// this recursively fills up the footprint frame with indices
	// assigned from the external context, mapped onto this
	// instance's public ports.  
	if (!actuals_parent_type::__initialize_assign_footprint_frame(
			*this, ff, sm, pmc, ind).good) {
		cerr << "Error alloc_assign_subinstance_footprint_frame."
			<< endl;
		return good_bool(false);
	}
	// scan footprint_frame for unallocated subinstances, and create them!
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called recusrively.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::assign_footprint_frame(footprint_frame& ff,
		const port_collection_context& pcc, const size_t ind) const {
	STACKTRACE_VERBOSE;
	const size_t local_offset = this->instance_index -1;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "local_offset = " << local_offset << endl;
	STACKTRACE_INDENT << "global_id = " << pcc.id_map[ind] << endl;
#endif
	footprint_frame_map_type& fm(ff.template get_frame_map<Tag>());
	INVARIANT(ind < pcc.size());
	INVARIANT(local_offset < fm.size());
	fm[local_offset] = pcc.id_map[ind];
#if ENABLE_STACKTRACE
	ff.dump_frame(STACKTRACE_STREAM) << endl;
#endif
	substructure_parent_type::__assign_footprint_frame(
		ff, pcc.substructure_array[ind]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General walker.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::accept(alias_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Rather than take a footprint argument, passed by the collection, 
	we get the footprint each time because collections with
	relaxed types may have different types per element.  
	This executes in a top-down traversal of the hierarchy.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::cflat_aliases(alias_printer& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->valid());
	ostringstream os;
	dump_hierarchical_name(os, dump_flags::no_leading_scope);
	const string& local_name(os.str());
	// construct new prefix from os
	const alias_printer::save_prefix save(c);
	const global_entry_pool<Tag>& gp(c.sm.template get_pool<Tag>());
	size_t gindex;
if (c.fpf) {
	c.prefix += ".";
	// this is not a top-level instance (from recursion)
	const size_t local_offset = this->instance_index -1;
	const footprint_frame_map_type&
		fm(c.fpf->template get_frame_map<Tag>());
	// footprint_frame yields the global offset
	gindex = fm[local_offset];
} else {
	// footprint_frame is null, this is a top-level instance
	// the instance_index can be used directly as the offset into
	// the state_manager's member arrays
	BOUNDS_CHECK(this->instance_index && this->instance_index < gp.size());
	gindex = this->instance_index;
}
	c.prefix += local_name;
	const global_entry<Tag>& e(gp[gindex]);
	__cflat_aliases(c, e, gindex);
	// recursion or termination
}	// end method cflat_aliases

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::collect_base()");
	// shouldn't need to re-visit parent pointer, 
	// UNLESS it is visited from an alias cycle, 
	// in which case, the parent may not have been visited before...

	// this is allowed to be null ONLY if it belongs to a scalar
	// in which case it is not yet unrolled.  
	// recall: null container => uninstantiated (or !valid())
	if (this->container)
		this->container->collect_transient_info(m);
	actuals_parent_type::collect_transient_info_base(m);
	substructure_parent_type::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instance depth.  
	Used for selecting a reasonably short canonical name.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
size_t
INSTANCE_ALIAS_INFO_CLASS::hierarchical_depth(void) const {
	return this->container->hierarchical_depth();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_alias(ostream& o, const dump_flags&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since begin points to next, self will always be printed last.  
	NOTE: always prints with default dump_flags.  
	NOTE: Now only prints the canonical alias's name
		because ring_nodes were iterable, but union-finds are not.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_aliases(ostream& o) const {
	this->dump_hierarchical_name(o);
	// use non-path-conmpressing find() because of const qualifier
	this->find()->dump_hierarchical_name(o << " = ");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_hierarchical_name(ostream& o,
		const dump_flags& df) const {
	return dump_alias(o, df);
	// should call virtually, won't die
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_hierarchical_name(ostream& o) const {
	return dump_alias(o, dump_flags::default_value);
	// should call virtually, won't die
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_iterator
INSTANCE_ALIAS_INFO_CLASS::find(void) {
	ICE_NEVER_CALL(cerr);
	return pseudo_iterator();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_const_iterator
INSTANCE_ALIAS_INFO_CLASS::find(void) const {
	ICE_NEVER_CALL(cerr);
	return pseudo_const_iterator();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::finalize_canonicalize(instance_alias_base_type&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename instance_alias_info<Tag>::substructure_parent_type&
INSTANCE_ALIAS_INFO_CLASS::__trace_alias_base(const substructure_alias&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
INSTANCE_ALIAS_INFO_CLASS::trace_alias(const substructure_alias&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	First lookup canonical placeholder ID, assigned by create phase.
	Then lookup...
	\param pcc the port context to assign to.  
	\param ff the reference footprint_frame.
	\param sm the global state allocation manager.
	\param ind
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::construct_port_context(
		port_collection_context& pcc, const footprint_frame& ff,
		const size_t ind) const {
	STACKTRACE_VERBOSE;
	const size_t local_placeholder_id = this->instance_index -1;
	const footprint_frame_map_type& fm(ff.template get_frame_map<Tag>());
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "local_id = " << local_placeholder_id << endl;
	STACKTRACE_INDENT << "global_id = " << fm[local_placeholder_id] << endl;
#endif
	pcc.id_map[ind] = fm[local_placeholder_id];
	this->__construct_port_context(pcc.substructure_array[ind], ff);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Virtually pure virtual.  Never supposed to be called, 
	yet this definition must exist to allow construction
	of the types that immedately derived from this type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_next_connection(
		const persistent_object_manager&, ostream&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really, pure virtual.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_next_connection(
		const persistent_object_manager&, istream&) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called during loading of the state instances' back-references.  
	Also is counterpart to load_next_connection.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::instance_alias_base_type&
INSTANCE_ALIAS_INFO_CLASS::load_alias_reference(
		const persistent_object_manager& m, istream& i) {
	never_ptr<container_type> next_container;
	m.read_pointer(i, next_container);
	// reconstruction ordering problem:
	// container must have its instances already loaded, though 
	// not necessarily constructed.
	// This is why instance re-population MUST be decoupled from
	// connection re-establishment *GRIN*.  
	// See? there's a reason for everything.  
	NEVER_NULL(next_container);
	// this is the safe way of ensuring that object is loaded once only.
	m.load_object_once(next_container);
	// the CONTAINER should read the key, because it is dimension-specific!
	// it should return a reference to the alias node, 
	// which can then be linked.  
	return next_container->load_reference(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::write_object_base()");
	// let the module take care of saving the state information
	write_value(o, this->instance_index);
	m.write_pointer(o, this->container);
	actuals_parent_type::write_object_base(m, o);
	substructure_parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::load_object_base()");
	// let the module take care of restoring the state information
	read_value(i, this->instance_index);
	m.read_pointer(i, this->container);
	actuals_parent_type::load_object_base(m, i);
	substructure_parent_type::load_object_base(m, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for calling collect_transient_info_base.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::transient_info_collector::operator () (
		const INSTANCE_ALIAS_INFO_CLASS& i) {
	i.collect_transient_info_base(this->manager);
}

//=============================================================================
// typedef instance_alias_base function definitions

INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o,
	const typename INSTANCE_ALIAS_INFO_CLASS::instance_alias_base_type& i) {
	typedef	class_traits<Tag>	traits_type;
	return o << traits_type::tag_name << "-alias @ " << &i;
}

//=============================================================================
// class instance_alias method definitions

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_CLASS::~instance_alias() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Prints out the next instance alias in the connected set.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_CLASS::dump_alias(ostream& o, const dump_flags& df) const {
	// STACKTRACE_VERBOSE;
	NEVER_NULL(this->container);
	this->container->dump_hierarchical_name(o, df) <<
		multikey<D, pint_value_type>(this->key);
		// casting to multikey for the sake of printing [i] for D==1.
		// could use specialization to accomplish this...
		// bah, not important
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::pseudo_iterator
INSTANCE_ALIAS_CLASS::find(void) {
	return instance_alias_base_type::find();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::pseudo_const_iterator
INSTANCE_ALIAS_CLASS::find(void) const {
	return instance_alias_base_type::find();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Manually update the next pointer of the union-find.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::finalize_canonicalize(instance_alias_base_type& n) {
	this->set(&n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
INSTANCE_ALIAS_CLASS::trace_alias(const substructure_alias& a) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "at: ") << endl;
#endif
	physical_instance_collection& pp(this->trace_collection(a));
#if ENABLE_STACKTRACE
	pp.dump(STACKTRACE_INDENT << "got: ",
		dump_flags::default_value) << endl;
	STACKTRACE_INDENT << "key: " << this->key << endl;
#endif
	// this seems wasteful but is required for correctness, 
	// as explained in the comments.  
	// assert dynamic cast
	container_type& c(IS_A(container_type&, pp));
	this_type& ret(*c[this->key]);		// asserts validity
#if ENABLE_STACKTRACE
	ret.dump_hierarchical_name(STACKTRACE_INDENT << "is: ") << endl;
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses self key to retrace an indentical hierarchical structure.
	\pre type-checked.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::substructure_parent_type&
INSTANCE_ALIAS_CLASS::__trace_alias_base(const substructure_alias& p) const {
	// NOTE: this will never be called for non-structured types (like bool)
	// because it is always called through parent (substructure_alias)
	return this->trace_alias(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves alias connection information persistently.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::write_next_connection()");
	NEVER_NULL(this->container);
	m.write_pointer(o, this->container);
	value_writer<key_type> write_key(o);
	write_key(this->key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-links connection.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::load_next_connection()");
	instance_alias_base_type& n(this->load_alias_reference(m, i));
	this->set(&n);	// manual unionization without path compression
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	if (this->next != this)
		this->next->collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves persistent information for reconstruction.  
 */
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,D>::write_object()");
	// DO NOT write out key now, that is the job of the next phase!
	INSTANCE_ALIAS_INFO_CLASS::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("int_alias::load_object()");
	// DO NOT load in key now, that is the job of the next phase!
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
}

//=============================================================================
// class KEYLESS_INSTANCE_ALIAS_CLASS method definitions

KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
KEYLESS_INSTANCE_ALIAS_CLASS::~instance_alias() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
ostream&
KEYLESS_INSTANCE_ALIAS_CLASS::dump_alias(ostream& o,
		const dump_flags& df) const {
	NEVER_NULL(this->container);
	return this->container->dump_hierarchical_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::pseudo_iterator
KEYLESS_INSTANCE_ALIAS_CLASS::find(void) {
	return instance_alias_base_type::find();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::pseudo_const_iterator
KEYLESS_INSTANCE_ALIAS_CLASS::find(void) const {
	return instance_alias_base_type::find();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Manually update the next pointer of the union-find.  
 */
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::finalize_canonicalize(
		instance_alias_base_type& n) {
	this->set(&n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
KEYLESS_INSTANCE_ALIAS_CLASS::trace_alias(const substructure_alias& p) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "at: ") << endl;
#endif
	physical_instance_collection& pp(this->trace_collection(p));
#if ENABLE_STACKTRACE
	pp.dump(STACKTRACE_INDENT << "got: ",
		dump_flags::default_value) << endl;
#endif
	container_type& c(IS_A(container_type&, pp));
	return c.get_the_instance();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: physical instance collection is not const in this case.  
 */
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::substructure_parent_type&
KEYLESS_INSTANCE_ALIAS_CLASS::__trace_alias_base(
		const substructure_alias& p) const {
	// NOTE: this will never be called for non-structured types (like bool)
	// because it is always called through parent (substructure_alias)
	return this->trace_alias(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::write_next_connection(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::write_next_connection()");
	m.write_pointer(o, this->container);
	// no key to write!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is the SAME as template version, this function need not be
	virtual!  
	Only issue: merge is a member function of bool_instance_alias_base, 
	which is a ring_node_derived<...>.
	May require another argument with a safe up-cast?
	Or just have this return the bool_instance_alias_base?
 */
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::load_next_connection(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::load_next_connection()");
	instance_collection_generic_type* next_container;
	m.read_pointer(i, next_container);
	NEVER_NULL(next_container);	// true?
	// no key to read!
	// problem: container is a never_ptr<const ...>, yucky
	m.load_object_once(next_container);
	instance_alias_base_type& n(next_container->load_reference(i));
	this->set(&n);	// manual unionization without path compression
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	if (this->next != this) {
		this->peek()->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::write_object(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::write_object()");
	INSTANCE_ALIAS_INFO_CLASS::write_object_base(m, o);
	// no key to write!
	// continuation pointer?
	NEVER_NULL(this->next);
	if (this->next == this) {
		write_value<char>(o, 0);
	} else {
#if STACKTRACE_PERSISTENTS
		cerr << "Writing a real connection!" << endl;
#endif
		write_value<char>(o, 1);
		this->peek()->write_next_connection(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::load_object(const persistent_object_manager& m,
		istream& i) {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::load_object()");
	INSTANCE_ALIAS_INFO_CLASS::load_object_base(m, i);
	// no key to load!
	char c;
	read_value(i, c);
	if (c) {
#if STACKTRACE_PERSISTENTS
		cerr << "Loading a real connection!" << endl;
#endif
		this->load_next_connection(m, i);
	}
}

//=============================================================================
// class instance_alias method definitions

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const instance_alias<Tag,D>& b) {
	typedef	class_traits<Tag>	traits_type;
	INVARIANT(b.valid());
	return o << '(' << traits_type::tag_name << "-alias-" << D << ')';
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__

