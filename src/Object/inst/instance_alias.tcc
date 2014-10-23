/**
	\file "Object/inst/instance_alias.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life, and then was split from
		"Object/inst/instance_collection.tcc".
	$Id: instance_alias.tcc,v 1.44 2011/02/08 22:32:48 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#define	STACKTRACE_FIND			(0 && ENABLE_STACKTRACE)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>
#include <vector>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.hh"

#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"
// #include "Object/inst/alias_actuals.tcc"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/alias_visitor.hh"
#include "Object/inst/subinstance_manager.tcc"
#include "Object/inst/instance_pool.tcc"
#include "Object/inst/internal_aliases_policy.hh"
#include "Object/inst/port_alias_tracker.hh"
#include "Object/expr/const_index.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/const_param_expr_list.hh"		// for debug only
#include "Object/expr/const_index_list.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/def/definition_base.hh"
#include "Object/type/canonical_type.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/simple_nonmeta_instance_reference.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/unroll/instantiation_statement_base.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.hh"
#include "Object/common/dump_flags.hh"
#include "Object/common/cflat_args.hh"
#include "Object/inst/alias_printer.hh"
#include "Object/traits/instance_traits.hh"	// need for alias visitor :-/
#include "common/ICE.hh"

#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/sstream.hh"
#include "util/persistent_object_manager.tcc"
#include "util/indent.hh"
#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/static_trace.hh"
#include "util/compose.hh"
#include "util/binders.hh"
#include "util/dereference.hh"


namespace HAC {
namespace entity {
using std::string;
using std::ostringstream;
// using std::_Select1st;
#include "util/using_ostream.hh"
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
	The only time a copy-contructor is allowed is on a new-born
	instance alias.  
	The next pointer is never copied, but re-established. 
	\param t the source instance_alias_info, should be empty.
	\pre nothing points to t, because its life is short.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS::instance_alias_info(const this_type& t) : 
		substructure_parent_type(t), 
		actuals_parent_type(t),
		direction_connection_policy(t), 
		next(this), 
		container(t.container) {
	INVARIANT(t.next == &t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	STACKTRACE_DTOR_PRINT("destroying " << this << endl);
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
	This variant asserts that the parent container of this alias
	has already been established, and that this is only ever called
	when binding relaxed template parameters.  
	\pre If container has relaxed type, then this alias has already
		been bound to relaxed template parameters.  
	Also called by alias_actuals::synchronize_actuals
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::instantiate_actuals_only(target_context& c) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(this->container);
// only if type is complete, expand ports
if (!this->container->get_canonical_collection().has_relaxed_type()
		|| this->get_relaxed_actuals()) {
	if (!substructure_parent_type::unroll_port_instances(
			*this->container, 
			this->get_relaxed_actuals(), c).good) {
		// already have error message
		THROW_EXIT;
	}
#if 0
	// did we forget this accidentally?
	actuals_parent_type::copy_actuals(f);
#endif
	direction_connection_policy::initialize_direction(*this);
	// FIXME: replay port aliases as early as possible, here
// if type is complete...
	typedef	internal_aliases_policy<traits_type::can_internally_alias>
				alias_traits;
	const good_bool g(alias_traits::connect(*this));
	if (!g.good) {
		THROW_EXIT;
	}
} else {
	direction_connection_policy::initialize_direction(*this);
}
#if ENABLE_STACKTRACE
	this->dump_ports(STACKTRACE_INDENT_PRINT(
		"ports after instantiate_actuals_only()\n"), 
		dump_flags::default_value) << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively expands the public ports of the instance hierarchy.  
	The initial state is defined by the meta-type.  
	\param p the parent actual collection
	\param c the unroll context for recursive instantiation
	NOTE: instantiation can now also occur at the time when
		relaxed template parameters are bound to an instance.  
		See Object/unroll/template_type_completion.{hh,tcc}.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::instantiate(const container_ptr_type p, 
		target_context& c) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(p);
	INVARIANT(!this->container);
	STACKTRACE_INDENT_PRINT("this->container (old) = " <<
		&*this->container << endl);
	this->container = p;

	instantiate_actuals_only(c);
	// we do this here for now merely for convenience/coverage:
	// it is certainly correct.  
	// future optimization: loop-transformation to eliminate
	// repeated redundant function calls (only affects channels)
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("this->container (new) = " <<
		&*this->container << endl);
	this->dump_hierarchical_name(STACKTRACE_INDENT << "instantiate: ")
		<< endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Variation of instantiate() used to forward local alias information
		from a formal collection to the actual copy. 
		In particular, we pass relaxed actuals and channel
		connectivity information.  
	\param p the parent actual collection
	\param c the unroll context for recursive instantiation
	\param f the corresponding instance alias belonging to the 
		formal collection.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::instantiate_actual_from_formal(
		const port_actuals_ptr_type p, 
		target_context& c,
		const this_type& f) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(p);
	INVARIANT(!this->container);
	this->container = p;
	// do we ever want to instantiate more than the ports? no
	if (!substructure_parent_type::unroll_port_instances(
			*this->container, 
			f.get_relaxed_actuals(), 
			c).good) {
		// already have error message
		THROW_EXIT;
	}
	import_properties(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f is the corresponding alias in the definition scope, 
		from which properties are to be copied over.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::import_properties(const this_type& f) {
	STACKTRACE_VERBOSE;
	actuals_parent_type::copy_actuals(f);
#if ENABLE_STACKTRACE
	f.dump_hierarchical_name(STACKTRACE_INDENT_PRINT("formal: ")) << endl;
	this->dump_hierarchical_name(STACKTRACE_INDENT_PRINT("this: ")) << endl;
#endif
	direction_connection_policy::initialize_actual_direction(f);
#if ENABLE_STACKTRACE
	f.dump_attributes(STACKTRACE_INDENT_PRINT("f.attributes: ")) << endl;
	this->dump_attributes(STACKTRACE_INDENT_PRINT("this->attributes: ")) << endl;
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
	\return true if this is a subinstance of another collection.
	a member.  So 'a.b' will return true.
	NOTE: this is pretty much template independent.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::is_subinstance(void) const {
	const never_ptr<const physical_instance_collection> c(this->container);
	NEVER_NULL(c);
	const instance_collection_base::super_instance_ptr_type
		p(c->get_super_instance());
	return p;
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
//	NEVER_NULL(p);		// first-time
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
	This DOES NOT check other aliases in the set, 
		that may belong to a port.  :(
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
		STACKTRACE_INDENT_PRINT("looking up member: " <<
			this->container->get_name() << endl);
		return *pp.lookup_port_instance(
			*this->container->get_placeholder_base());
	} else {
		STACKTRACE_INDENT_PRINT("terminal alias" << endl);
		// This case cannot be reached when this is 
		// a subinstanceless type.
		// then we are at top-most level, terminate recursion
		// from the type of sup, lookup the member
		physical_instance_collection&
			ret(*sup.lookup_port_instance(
				*this->container->get_placeholder_base()));
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
	// NOTE: doesn't auto-instantiate based on actuals synchronization
	const pseudo_iterator i(this->find());
	// i points to the canonical alias for this set
	if (!i->instance_index) {
		// only allocate if this is the canonical member of the 
		// union-find set, otherwise, just copy.
		// for now the creator will be the canonical back-reference
		typename instance_type::pool_type&
			the_pool(f.template get_instance_pool<Tag>());
		i->instance_index = the_pool.allocate(instance_type(*i));
		++i->instance_index;	// returned index is now 0-based
		// but instance_id is 1-based
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
	Compares *collection* types of the two instances and then
	TODO: compares their relaxed actuals (if applicable).
	NOTE: relaxed actuals are compared by synchronize/compare_actuals().  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::must_match_type(const this_type& a) const {
	return this->container->get_canonical_collection()
		.must_be_collectibly_type_equivalent(
			a.container->get_canonical_collection());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Alias object assignment should never be called at run-time
	(e.g. due to a realloc) because the address of these alias
	must be stationary.  (non-movable, non-copiable).
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
INSTANCE_ALIAS_INFO_CLASS::operator = (const this_type&) {
	ICE_NEVER_CALL(cerr);
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_key(ostream& o) const {
	NEVER_NULL(this->container);
	return this->container->dump_element_key(o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This version of connect is symmetric and tailored to port connections,
	and is called from simple_meta_instance_reference::connect_port.  
	This effectively checks for connectible-type-equivalence.  
	optimization: any need to check for self-aliases? uncommon.
	\param l an alias of this type.  
	\param r an alias of this type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::checked_connect_port(this_type& l, this_type& r,
		target_context& c) {
	STACKTRACE_VERBOSE;
	if (!l.must_match_type(r)) {
		// already have error message
		l.dump_hierarchical_name(cerr << "\tfrom: ") << endl;
		r.dump_hierarchical_name(cerr << "\tand:  ") << endl;
		return good_bool(false);
	}
	// checking of directions and relaxed actuals is done in unite()
	return good_bool(l.unite(r, c).good &&
		l.connect_port_aliases_recursive(r, c).good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special connection for implicit ports.  
	Can be disconnected!
	USE WITH CAUTION.
	Note: this does NOT synchronize alias attributes!
	Rationale: because connection is undo-able, we do not want
		the effects of an invalidated connection to persist.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::connect_implicit_port(this_type& r) {
	// this clobbers any former connection in the union find structure!
	this->next = &r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if INSTANCE_SUPPLY_DISCONNECT
/**
	Disconnects implicit ports.  
	USE WITH CAUTION.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::disconnect_implicit_port(void) {
	// this clobbers any former connection in the union find structure!
	this->next = this;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper around checked_connect_port, shouldn't require a real
	unroll_context because is only replaying internal alias.  
	Similar to checked_connect_port(), but this asserts that
	type and attributes already match.
	See also relative: checked_connect_port()
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::replay_connect_port(this_type& l, this_type& r) {
	STACKTRACE_VERBOSE;
#if 0
	const footprint* const f = NULL;
	const unroll_context bogus(f, f);
	return checked_connect_port(l, r, bogus);
#else
	INVARIANT(l.must_match_type(r));
	l.reunite(r);
	l.reconnect_port_aliases_recursive(r);
#endif
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
		footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	// this recursively fills up the footprint frame with indices
	// assigned from the external context, mapped onto this
	// instance's public ports.  
	if (!actuals_parent_type::__initialize_assign_footprint_frame(
			*this, ff).good) {
		cerr << "Error alloc_assign_subinstance_footprint_frame."
			<< endl;
		return good_bool(false);
	}
	// scan footprint_frame for unallocated subinstances, and create them!
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	General walker.  
	To traverse subinstances, derive visitor from struct port_visitor!
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::accept(alias_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::collect_base()");
	// shouldn't need to re-visit parent pointer, 
	// UNLESS it is visited from an alias cycle, 
	// in which case, the parent may not have been visited before...
	NEVER_NULL(this->container);
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
INSTANCE_ALIAS_INFO_CLASS::dump_alias(ostream& o, const dump_flags& df) const {
	NEVER_NULL(this->container);
	return this->dump_key(this->container->dump_hierarchical_name(o, df));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the last part of the hierarchical name only.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_leaf_name(ostream& o) const {
	NEVER_NULL(this->container);
	return this->dump_key(o << this->container->get_name());
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
	const pseudo_const_iterator f(this->find());
	INVARIANT(f);
	f->dump_hierarchical_name(o << " = ");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_hierarchical_name(ostream& o,
		const dump_flags& df) const {
	return dump_alias(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_hierarchical_name(ostream& o) const {
	return dump_alias(o, dump_flags::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to check for pointer equality?  Same result either way.
	asymmetric union-find connection: 'this' connects (->) to r
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::unite(this_type& r,
		target_context& c) {
	STACKTRACE_VERBOSE;
	const pseudo_iterator lc(this->find(c));
	this_type* const rc = &*r.find(c);
	lc->next = rc;
	// synchronize direction_connection_flags
		// commutative
	if (!direction_connection_policy::synchronize_flags(*lc, *rc).good) {
		cerr << "Error connecting: ";
		this->dump_hierarchical_name(cerr) << endl;
		r.dump_hierarchical_name(cerr << "\tand:  ") << endl;
		return good_bool(false);
	}
		// symmetric
	const good_bool
		ret(actuals_parent_type::synchronize_actuals(*lc, *rc, c));
	if (!ret.good) {
		this->dump_hierarchical_name(cerr << "\tfrom: ") << endl;
		r.dump_hierarchical_name(cerr << "\tand:  ") << endl;
	}
	return ret;
	// will already have error message
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called to re-connect port aliases.
	Instead of checking for attribute/flag compatibility,
	this should really check that attributes/flags are already
	*identical* -- they should have already been synchronized
	when their types were completed.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::reunite(this_type& r) {
	STACKTRACE_VERBOSE;
	const pseudo_iterator lc(this->find());
	this_type* const rc = &*r.find();
	lc->next = rc;
#if 1
// FIXME:
	INVARIANT(lc->matched_flags(*rc));
	INVARIANT(lc->matched_actuals(*rc));
#endif
	// will already have error message
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: recursive implementation.
	Every call to find() compresses the path to the canonical node.  
	Ref: union-find structure/algorithm.
	TODO: should this copy-propagate relaxed actuals?
	NOTE: this variant does NOT maintain relaxed actuals!
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_iterator
INSTANCE_ALIAS_INFO_CLASS::find(void) {
#if STACKTRACE_FIND
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
	NEVER_NULL(this->next);
	if (this->next != this->next->next) {
		this->next = &*this->next->find();
	}
	return pseudo_iterator(this->next);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant of find() automatically instantiates ports
	as relaxed actuals are synchronized.  
	\param c the unroll_context used to lookup.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_iterator
INSTANCE_ALIAS_INFO_CLASS::find(target_context& c) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
	NEVER_NULL(this->next);
	if (this->next != this->next->next) {
		this->next = &*this->next->find(c);
	}
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT_PRINT("alias: "))
		<< endl;
#endif
	actuals_parent_type::finalize_actuals_and_substructure_aliases(
		*this, c);
	return pseudo_iterator(this->next);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by footprint::...finalize_substructure_aliases.
	TODO: template policy dispatch.
	\throw exception on instantiation error.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::finalize_find(const unroll_context& c) {
	// flatten, attach actuals, instantiate, and connect as necessary
	this->find(c.as_target_footprint());
	actuals_parent_type::__finalize_find(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if type is complete.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::has_complete_type(void) const {
	return actuals_parent_type::__has_complete_type(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\throw exception if this alias has incomplete type.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::assert_complete_type(void) const {
	actuals_parent_type::__assert_complete_type(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return first alias in union-find path with complete type, if any.
	\throw exception if not found
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_const_iterator
INSTANCE_ALIAS_INFO_CLASS::find_complete_type_alias(void) const {
	if (this->has_complete_type()) {
		return pseudo_const_iterator(this);
	} else if (this->next != this) {
		return next->find_complete_type_alias();
	} else {
		// rare path
		// FAIL: just re-use error message for consistency
		this->assert_complete_type();
		// throws to exit
	}
	return pseudo_const_iterator(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This non-modifying variant of find() does NOT perform
	path compression.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::pseudo_const_iterator
INSTANCE_ALIAS_INFO_CLASS::find(void) const {
#if STACKTRACE_FIND
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
	const this_type* tmp = this;
	while (tmp != tmp->next) {
		tmp = tmp->next;
	}
	NEVER_NULL(tmp);	// every union-find element must terminate!
	return pseudo_const_iterator(tmp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre this must be a canonical alias.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
error_count
INSTANCE_ALIAS_INFO_CLASS::check_connection(void) const {
	INVARIANT(this->next == this);
	return direction_connection_policy::__check_connection(*this);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Synchronizes a non-canonical aliases's direction flags with
	its canonical alias's flags, if applicable.
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::update_direction_flags(void) {
	direction_connection_policy::__update_flags(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is called by alias_reference_set::shortest_alias()
	to manually flatten or restructure the union-find.  
	ALERT: Don't do anything else here that uses the next pointer, 
	because union-find structure is momentarily incoherent 
	(as pointers are updated in a for-loop).
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::finalize_canonicalize(this_type& n) {
	this->next = &n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	What does this do again?
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename instance_alias_info<Tag>::substructure_parent_type&
INSTANCE_ALIAS_INFO_CLASS::__trace_alias_base(
		const substructure_alias& p) const {
	return this->trace_alias(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given this alias, find the corresponding element
	in the given substructure.
	This is useful for following instance aliases in identical
	parallel subinstance hierarchies.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
INSTANCE_ALIAS_INFO_CLASS::trace_alias(const substructure_alias& a) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "at: ") << endl;
#endif
	physical_instance_collection& pp(this->trace_collection(a));
#if ENABLE_STACKTRACE
	pp.dump(STACKTRACE_INDENT << "got: ",
		dump_flags::default_value) << endl;
	this->dump_key(STACKTRACE_INDENT << "key: ") << endl;
#endif
	// this seems wasteful but is required for correctness, 
	// as explained in the comments.  
	// assert dynamic cast
	container_type& c(IS_A(container_type&, pp));
	this_type& ret(c.get_corresponding_element(*this->container, *this));
#if ENABLE_STACKTRACE
	ret.dump_hierarchical_name(STACKTRACE_INDENT << "is: ") << endl;
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Now instance aliases are dimension-agnostic, so they must infer
	their position and key from the parent container.  
	The multikey index is translated into a single-valued index.
	The index is omitted for scalar aliases.  
	The index is read back in instance_array::load_reference(), 
		also instance_scalar, port_formal_array, 
		and port_actual_collection.

	Comment is obsolete (20061020):
	Virtually pure virtual.  Never supposed to be called, 
	yet this definition must exist to allow construction
	of the types that immedately derived from this type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_next_connection(
		const collection_pool_bundle_type& pool, ostream& o) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(this->container);
	this->container->write_pointer(o, pool);
	const size_t dim = this->container->get_dimensions();
	STACKTRACE_INDENT_PRINT("dim = " << dim << endl);
	if (dim) {
		const size_t index = this->container->lookup_index(*this);
		// 1-indexed for ALL collections
		INVARIANT(index);
		write_value(o, index);
	}
	// else scalar alias reference doesn't need index
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre everything but the next pointer is already loaded, 
		including the parent container pointer.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_next_connection(
		const collection_pool_bundle_type& pool, istream& i) {
	STACKTRACE_VERBOSE;
#if STACKTRACE_PERSISTENTS
	STACKTRACE_INDENT_PRINT("this = " << this << endl);
#endif
	NEVER_NULL(this->container);
	this->next = &load_alias_reference(pool, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called during loading of the state instances' back-references.  
	Also is counterpart to load_next_connection.
	The key/index load is finished by the call to load_reference.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
INSTANCE_ALIAS_INFO_CLASS&
INSTANCE_ALIAS_INFO_CLASS::load_alias_reference(
		const collection_pool_bundle_type& pool, istream& i) {
	STACKTRACE_VERBOSE;
	const never_ptr<container_type> next_container(pool.read_pointer(i));
	// reconstruction ordering problem:
	// container must have its instances already loaded, though 
	// not necessarily constructed.
	// This is why instance re-population MUST be decoupled from
	// connection re-establishment *GRIN*.  
	// See? there's a reason for everything.  
	NEVER_NULL(next_container);
	// this is the safe way of ensuring that object is loaded once only.
	// collections of like meta-type have already been loaded
	// as guaranteed by the instance_collection_pool_bundle.  
	// the CONTAINER should read the key, because it is dimension-specific!
	// it should return a reference to the alias node, 
	// which can then be linked.  
	return next_container->load_reference(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: factor out into scalar and non-scalar version
	to avoid repeated if branching in write-loop.  
	NOTE: container back-reference is not written anymore, it is the
		responsibility of the container.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::write_object_base(const footprint& f, 
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::write_object_base()");
	// let the module take care of saving the state information
	write_value(o, this->instance_index);
	NEVER_NULL(this->container);	// no need to write out
	actuals_parent_type::write_object_base(m, o);
	substructure_parent_type::write_object_base(f, o);
	direction_connection_policy::write_flags(o);
//	else skip, collection will write connections later...
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: factor out into scalar and non-scalar version
	to avoid repeated if branching in load-loop.  
	NOTE: container back-reference is loaded by the container now.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::load_object_base(const footprint& f, 
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::load_object_base()");
	NEVER_NULL(this->container);	// already set by container!
	// let the module take care of restoring the state information
	read_value(i, this->instance_index);
	actuals_parent_type::load_object_base(m, i);
	substructure_parent_type::load_object_base(f, i);
	direction_connection_policy::read_flags(i);
//	else skip, collection will load connections later...
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
// typedef instance_alias_info function definitions

INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const INSTANCE_ALIAS_INFO_CLASS& i) {
	typedef	class_traits<Tag>	traits_type;
	return o << traits_type::tag_name << "-alias @ " << &i;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_ALIAS_TCC__

