/**
	\file "Object/inst/instance_alias.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life, and then was split from
		"Object/inst/instance_collection.tcc".
	$Id: instance_alias.tcc,v 1.1.2.1 2005/09/06 20:55:37 fang Exp $
	TODO: trim includes
 */

#ifndef	__OBJECT_INST_INSTANCE_ALIAS_TCC__
#define	__OBJECT_INST_INSTANCE_ALIAS_TCC__

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
#include "common/ICE.h"

#include "util/multikey_set.tcc"
#include "util/ring_node.tcc"
#include "util/packed_array.tcc"
#include "util/memory/count_ptr.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/ptrs_functional.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"


namespace ART {
namespace entity {
using std::string;
using std::_Select1st;
#include "util/using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
USING_STACKTRACE
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
		this->container->dump(cerr) << endl;
		cerr << "should point to: " << p << endl;
		p->dump(cerr) << endl;
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
	NEVER_NULL(p);
	INVARIANT(!this->container);
	this->container = p;
	substructure_parent_type::unroll_port_instances(*this->container, c);
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
	sup.dump_hierarchical_name(STACKTRACE_INDENT << "arg : ") << endl;
#endif
	const instance_collection_base::super_instance_ptr_type
		thisp(this->container->get_super_instance());
	if (thisp) {
#if ENABLE_STACKTRACE
		thisp->dump_hierarchical_name(STACKTRACE_INDENT << "parent: ")
			<< endl;
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
		ret.dump(STACKTRACE_INDENT << "ret: ") << endl;
#endif
		return ret;
	}
	// then alias type (indexed or keyless) should lookup
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	OBSOLETE.
	This is retained for historical record of how up-and-down
	instance alias retracing worked.  

	Follows an identical hierarchy to find the matching corresponding 
	instance alias.  
	\param sup will determine when recursion ends.
	Useful for reproducing internal aliases externally.  
	NOTE: this is entirely template independent.  
		Factor this shit out later.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
physical_instance_collection&
INSTANCE_ALIAS_INFO_CLASS::retrace_collection(
		const substructure_alias& sup) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "this: ") << endl;
	sup.dump_hierarchical_name(STACKTRACE_INDENT << "arg : ") << endl;
#endif
	const instance_collection_base::super_instance_ptr_type
		thisp(this->container->get_super_instance());
	const never_ptr<const physical_instance_collection>
		supc(sup.get_container_base());
	NEVER_NULL(supc);
	const instance_collection_base::super_instance_ptr_type
		supp(supc->get_super_instance());
	NEVER_NULL(supp);
	if (thisp) {
#if ENABLE_STACKTRACE
		thisp->dump_hierarchical_name(STACKTRACE_INDENT << "parent: ")
			<< endl;
#endif
		// need to lookup parent instance first
		// p points to a substructured alias
		const substructure_alias&
			pp(thisp->__retrace_alias_base(
				const_cast<RETRACE_ALIAS_BASE_ARG_TYPE>(*supp)));
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
			ret(*supp->lookup_port_instance(*this->container));
#if ENABLE_STACKTRACE
		ret.dump(STACKTRACE_INDENT << "ret: ") << endl;
#endif
		return ret;
	}
	// then alias type (indexed or keyless) should lookup
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this container is a port formal, then this will invoke
	it's parent super-instance's state allocation.  
	Upward-recursive.  
	Inside a definition scope however, the top-most instances
	should not invoke allocation of their canonical parents.  
	Q: how do we know our contect?
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::create_super_instance(footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "inspecting: ")
		<< endl;
#endif
	if (this->container->is_port_formal()) {
		STACKTRACE("is subinstance");
		// check if this collection is top-level in the current
		// scope.  if so, then terminate upward recursion.  
		// this should be able to replace the is_port_formal() check.  
		if (f[this->container->get_name()] == this->container) {
			STACKTRACE("is top-level in current scope");
			return good_bool(true);
		}
		// super_instance is a const substructure_alias*
		if (!this->container->get_super_instance()
				->allocate_state(f)) {
			this->dump_hierarchical_name(
				cerr << "ERROR creating placeholder state "
				"for super-instance of ") << endl;
			return good_bool(false);
		} else {
			return good_bool(true);
		}
	} else {
		STACKTRACE("is NOT subinstance");
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This (public) variant of state allocation is intended for top-level
	construction, where we need to check for super-instance of aliases
	and create them top-down.  
	We obviously don't want to do that when we're already processing 
	top-down from a port expansion.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
size_t
INSTANCE_ALIAS_INFO_CLASS::allocate_state(footprint& f) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump_hierarchical_name(STACKTRACE_INDENT << "allocating: ");
	this->dump_aliases(STACKTRACE_STREAM << " with aliases: ") << endl;
#endif
	this_type& _this = const_cast<this_type&>(*this);
	// BUG FIX: need to see if aliases have super-instances
	// upward recursion!
	// remember, begin starts with the NEXT item
	// end -1 will point to self, to we need to stop one short
	iterator i(_this.begin());
	iterator j(i++);
	const iterator e(_this.end());
	for ( ; i!=e; i++, j++) {
		if (!j->create_super_instance(f).good) {
			// already have partial error message
			return 0;
		}
	}
	return _this.__allocate_state(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates space for a sub-instance, during the create phase  
	Checks whether or not this alias already points to allocated
	state, and if so, just returns.  
	Otherwise, allocate state and propagate the newly allocated
	state to all other equivalent aliases (downward).  
	TODO: bottom up construction of dependent types to form definition
		footprints.  Bottom up is necessary because external aliases
		between ports need to take into account internal aliases
		which are determined by complete definitions.  

	IMPORTANT: 2005-08-16:
	TODO: unroll the type referenced by this instance.  
		We need to find the alias in the ring that contains
		valid relaxed actuals, if applicable, to form a 
		complete canonical type.  
		However, the alias with actuals may actually
		exist only in a ring somewhere else in the hierarchy!
		This will have to be developed in pieces.  
		Need to impose temporary limitations.  
	UPDATE: 2005-08-18
		No longer need to search rings, because we enforce
		that all aliases in a ring have actuals or none.
		Null-actuals rings are merged when discovered.  

	BUG: test case parser/connect/118.in
		check aliases for super-instances, must create those first!
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
size_t
INSTANCE_ALIAS_INFO_CLASS::__allocate_state(footprint& f) const {
	STACKTRACE_VERBOSE;
	if (this->instance_index)
		return this->instance_index;
	// else we haven't visited this one yet
	// hideous const_cast :S consider mutability?
	this_type& _this = const_cast<this_type&>(*this);
	// for now the creator will be the canonical back-reference
	typename instance_type::pool_type& the_pool(f.template get_pool<Tag>());
	_this.instance_index = the_pool.allocate(instance_type(*this));
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "assigned id: " << this << " = "
		<< this->instance_index << endl;
#endif
	INVARIANT(this->instance_index);
	// NOTE: can't _this.allocate_subinstances() yet because there
	// may be aliases between the ports, see comment below.  
	// Visit each alias in the ring and connect
	iterator j(_this.begin());	// begin points to next! (ring_node)
	// skip itself, the start
	iterator i(j++);
	// j stays one-ahead of i
	// stop one-short of the end, which points to itself
	const iterator e(_this.end());
#if 0
{
	// PUNTING this BUG FIX
	iterator ii(i), jj(j);
	for ( ; jj!=e; ii=jj, jj++) {
		if (!synchronize_actuals_recursive(_this, *ii).good)
			return 0;
	}
}
#endif
	for ( ; j!=e; i=j, j++) {
#if 0
		if (i->instance_index) {
			ICE(cerr, 
			cerr << "expected instance_index to be 0, but got " <<
				i->instance_index << endl;
			this->dump_hierarchical_name(cerr << "this = ") << endl;
			the_pool[i->instance_index]
				.get_back_ref()->dump_hierarchical_name(
					cerr << "alias = ") << endl;
			)
		}
#else
		INVARIANT(!i->instance_index);
#endif
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "assigned id: " << &*i << " = "
			<< this->instance_index << endl;
#endif
		i->instance_index = this->instance_index;
		// when to propagate relaxed actuals?
		// it is possible that the first few entries actuals may be NULL
		// while later actuals are valid in the ring.
		// should they have been propagated during 
		// instance_reference_connection::unroll?
		if (!synchronize_actuals(_this, *i).good)
			return 0;
		// recursive connections, merging ports.
		if (!_this.create_subinstance_state(*i, f).good)
			return 0;	// 0 means error
	}
	// after having synchronized relaxed actuals
	// create footprints of dependent types bottom up
	// this must be done before allocating subinstances
	// because of possible internal aliases
	// 1) get complete type first and merge in relaxed actuals
	//	if type is still relaxed (incomplete), this is an error
	// this really shouldn't be necessary, but it doesn't hurt.
	if (!create_dependent_types(*this).good) {
		// already have error message
		return 0;
	}
	// This must be done AFTER processing aliases because
	// ports may be connected to each other externally
	// Postponing guarantees that port aliases are resolved first.
	// Then remaining unaliased ports may be allocated, 
	// once we can deduce that no further aliases exist.  
	// a test case that demonstrates this is parser/connect/111.in
	_this.allocate_subinstances(f);
	return this->instance_index;
}	// end method __allocate_state

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't forget to merge all aliases too!
	TODO: compare-and-update relaxed actuals as connections are made!
		using alias_parent type: spread contagiously!
	\return good if successful.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::merge_allocate_state(this_type& t, footprint& f) {
	STACKTRACE_VERBOSE;
	const size_t ind = this->instance_index;
	const size_t tind = t.instance_index;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "this = " << this << ", &t = " << &t << endl;
	STACKTRACE_INDENT << "ind = " << ind << ", tind = " << tind << endl;
#endif
	if (ind) {
		if (tind) {
			// possible both are already connected and allocated
#if 1
			if (ind != tind) {
			typedef	typename instance_type::pool_type pool_type;
			const pool_type& the_pool(f.template get_pool<Tag>());
			ICE(cerr, 
				cerr << "connecting two instances already "
					"assigned to different IDs: got " <<
					ind << " and " << tind << endl;
				the_pool[ind].get_back_ref()
					->dump_hierarchical_name(cerr << '\t')
					<< endl;
				the_pool[tind].get_back_ref()
					->dump_hierarchical_name(cerr << '\t')
					<< endl;
			)
			}
#else
			INVARIANT(ind == tind);
#endif
		} else {
			// this already assigned, assign to t
			t.inherit_subinstances_state(*this, f);
		}
	} else {
		if (!tind) {
			// neither has been created yet
			if (!t.__allocate_state(f))
				return good_bool(false);
		}
		this->inherit_subinstances_state(t, f);
	}
#if 0
	// punting possible bug fix
	return synchronize_actuals_recursive(*this, t);
#else
	if (!synchronize_actuals(*this, t).good) {
		// already have partial error message from compare
		cerr << "Conflicting actuals in hierarchical connections."
			<< endl;
		this->dump_hierarchical_name(cerr << "between: ") << endl;
		t.dump_hierarchical_name(cerr << "    and: ") << endl;
		return good_bool(false);
	}
	return good_bool(true);
#endif
}	// end method merge_allocate_state

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simply copy state_instance indices from source to this destination.  
	\param t the source alias from which to inherit the ID.  
	\param f the footprint from which to get connection information.  
	\pre this is not at all state assigned, 
		and t is completely state assigned.  
	Q: in for-loop, what difference between calling inherit_state
		and merge_allocate_state?
	A: ?
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::inherit_subinstances_state(const this_type& t,
		const footprint& f) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "this = " << this << " and t = " << &t << endl;
	STACKTRACE_INDENT << "have this index = " << this->instance_index <<
		" and t.index = " << t.instance_index << endl;
#endif
	INVARIANT(t.instance_index);
#if 0
	INVARIANT(!this->instance_index);
#else
	// quick patch to fix bug exhibited by process/066.in (crash)
	// don't know if this is wrong, but it seems to work
	// should prove this later.  
	if (this->instance_index) {
		INVARIANT(this->instance_index == t.instance_index);
		return;
	}
#endif
	iterator i(this->begin());
	const iterator e(this->end());
	const instance_type&
		inst(f.template get_pool<Tag>()[t.instance_index]);
	for ( ; i!=e; i++) {
		INVARIANT(!i->instance_index);
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "assigned id: " << &*i << " = "
			<< t.instance_index << endl;
#endif
		i->instance_index = t.instance_index;
		i->inherit_state(inst, f);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compares collection types of the two instances and then
	(TODO) compares their relaxed actuals (if applicable).
	NOTE: relaxed actuals are compared by synchronize/compare_actuals().  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
bool
INSTANCE_ALIAS_INFO_CLASS::must_match_type(const this_type& a) const {
	return this->container->must_match_type(*a.container);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre every alias in this ring must have NULL actuals.  
	\post every alias in this ring has the same valid actuals, 
		same pointer too.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::propagate_actuals(const relaxed_actuals_type& a) {
#if 0
	copy(this->begin(), this->end(), actuals_inserter());
#else
	iterator i(this->begin());
	const iterator e(this->end());
	for ( ; i!=e; i++) {
		const bool b = i->attach_actuals(a);
		INVARIANT(b);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For aliases with actuals, this compares against the actuals argument.  
	If this doesn't have actuals yet, then it copies the actuals
	around the ring, maintaining the all-or-none invariant.  
	TODO: specialize this implementation based on policy for
	actuals-less meta-types.  
	Specialize through the actuals_base_type.
	Might also check parent collection type for when this is necessary.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::compare_and_propagate_actuals(
		const relaxed_actuals_type& a) {
	return actuals_parent_type::__compare_and_propagate_actuals(a, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::synchronize_actuals(this_type& l, this_type& r) {
	return actuals_parent_type::__symmetric_synchronize(l, r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ALIAS_INFO_CLASS::synchronize_actuals_recursive(
		this_type& l, this_type& r) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	l.dump_hierarchical_name(STACKTRACE_INDENT << "with: ");
		l.dump_actuals(STACKTRACE_STREAM) << endl;
	r.dump_hierarchical_name(STACKTRACE_INDENT << "and : ");
		r.dump_actuals(STACKTRACE_STREAM) << endl;
#endif
	if (!synchronize_actuals(l, r).good) {
		// already have partial error message from compare
		cerr << "Conflicting actuals in hierarchichal connections."
			<< endl;
		l.dump_hierarchical_name(cerr << "between: ") << endl;
		r.dump_hierarchical_name(cerr << "    and: ") << endl;
		return good_bool(false);
	}
	// need to hierarchically check relaxed actuals of subinstances
	// in all cases?
	else if (!synchronize_port_actuals(l, r).good) {
		cerr << "Conflicting actuals found in implicit port connections"
			<< endl;
		l.dump_hierarchical_name(cerr << "between: ") << endl;
		r.dump_hierarchical_name(cerr << "    and: ") << endl;
		return good_bool(false);
	}
#if ENABLE_STACKTRACE
	l.dump_actuals(STACKTRACE_INDENT << "now : ") << endl;
	r.dump_actuals(STACKTRACE_INDENT << "and : ") << endl;
#endif
	return good_bool(true);
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
	// TODO: this only necessary for types with relaxed actuals
	// specialize to optimize later.  
	// note: should not be recursive at unroll-time
	if (!synchronize_actuals(l, r).good) {
		// already have error message
		return good_bool(false);
	}
	instance_alias_base_type& ll(AS_A(instance_alias_base_type&, l));
	instance_alias_base_type& rr(AS_A(instance_alias_base_type&, r));
	ll.merge(rr);
	return good_bool(true);
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
INSTANCE_ALIAS_INFO_CLASS::checked_connect_alias(this_type& l, this_type& r, 
		const relaxed_actuals_type& a) {
	if (!l.must_match_type(r)) {
		// already have error message
		return good_bool(false);
	}
	if (!r.compare_and_propagate_actuals(a).good) {
		// already have partial error message
		l.dump_hierarchical_name(cerr << "\tand :") << endl;
		return good_bool(false);
	}
	instance_alias_base_type& ll(AS_A(instance_alias_base_type&, l));
	instance_alias_base_type& rr(AS_A(instance_alias_base_type&, r));
	ll.merge(rr);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias_info<Tag>::collect_base()");
	// eventually need to implement this...

	// shouldn't need to re-visit parent pointer, 
	// UNLESS it is visited from an alias cycle, 
	// in which case, the parent may not have been visited before...

	// this is allowed to be null ONLY if it belongs to a scalar
	// in which case it is not yet unrolled.  
	if (this->container)
		this->container->collect_transient_info(m);
	actuals_parent_type::collect_transient_info_base(m);
	substructure_parent_type::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_alias(ostream& o) const {
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since begin points to next, self will always be printed last.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_aliases(ostream& o) const {
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	i->dump_alias(o);
	for (i++; i!=e; i++) {
		i->dump_alias(o << " = ");
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ALIAS_INFO_CLASS::dump_hierarchical_name(ostream& o) const {
	// STACKTRACE_VERBOSE;
	return dump_alias(o);	// should call virtually, won't die
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::const_iterator
INSTANCE_ALIAS_INFO_CLASS::begin(void) const {
	ICE_NEVER_CALL(cerr);
	return const_iterator(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::const_iterator
INSTANCE_ALIAS_INFO_CLASS::end(void) const {
	ICE_NEVER_CALL(cerr);
	return const_iterator(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::iterator
INSTANCE_ALIAS_INFO_CLASS::begin(void) {
	ICE_NEVER_CALL(cerr);
	return iterator(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_INFO_CLASS::iterator
INSTANCE_ALIAS_INFO_CLASS::end(void) {
	ICE_NEVER_CALL(cerr);
	return iterator(NULL);
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
	return o << class_traits<Tag>::tag_name << "-alias @ " << &i;
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
INSTANCE_ALIAS_CLASS::dump_alias(ostream& o) const {
	// STACKTRACE_VERBOSE;
	NEVER_NULL(this->container);
	this->container->dump_hierarchical_name(o) <<
		multikey<D, pint_value_type>(this->key);
		// casting to multikey for the sake of printing [i] for D==1.
		// could use specialization to accomplish this...
		// bah, not important
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::const_iterator
INSTANCE_ALIAS_CLASS::begin(void) const {
	return instance_alias_base_type::begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::const_iterator
INSTANCE_ALIAS_CLASS::end(void) const {
	return instance_alias_base_type::end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::iterator
INSTANCE_ALIAS_CLASS::begin(void) {
	return instance_alias_base_type::begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename INSTANCE_ALIAS_CLASS::iterator
INSTANCE_ALIAS_CLASS::end(void) {
	return instance_alias_base_type::end();
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
	pp.dump(STACKTRACE_INDENT << "got: ") << endl;
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
#if STACKTRACE_PERSISTENTS
	cerr << "ring size before = " << this->size();
#endif
	this->merge(n);       // re-link
	// this->unsafe_merge(n);       // re-link (undeclared!??)
	// unsafe is OK because we've already checked linkage when it was made!
#if STACKTRACE_PERSISTENTS
	cerr << ", after = " << this->size() << endl;
#endif
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
KEYLESS_INSTANCE_ALIAS_CLASS::dump_alias(ostream& o) const {
	NEVER_NULL(this->container);
	return this->container->dump_hierarchical_name(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::const_iterator
KEYLESS_INSTANCE_ALIAS_CLASS::begin(void) const {
	return instance_alias_base_type::begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::const_iterator
KEYLESS_INSTANCE_ALIAS_CLASS::end(void) const {
	return instance_alias_base_type::end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::iterator
KEYLESS_INSTANCE_ALIAS_CLASS::begin(void) {
	return instance_alias_base_type::begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
typename KEYLESS_INSTANCE_ALIAS_CLASS::iterator
KEYLESS_INSTANCE_ALIAS_CLASS::end(void) {
	return instance_alias_base_type::end();
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
	pp.dump(STACKTRACE_INDENT << "got: ") << endl;
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
#if STACKTRACE_PERSISTENTS
	cerr << "ring size before = " << this->size();
#endif
	instance_alias_base_type& n(next_container->load_reference(i));
	this->merge(n);
#if STACKTRACE_PERSISTENTS
	cerr << ", after = " << this->size() << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
KEYLESS_INSTANCE_ALIAS_TEMPLATE_SIGNATURE
void
KEYLESS_INSTANCE_ALIAS_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_alias<Tag,0>::collect_transients()");
	// this isn't truly a persistent type, so we don't register this addr.
	INSTANCE_ALIAS_INFO_CLASS::collect_transient_info_base(m);
	if (this->next != this)
		this->next->collect_transient_info_base(m);
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
		this->next->write_next_connection(m, o);
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
	INVARIANT(b.valid());
	return o << '(' << class_traits<Tag>::tag_name << "-alias-" << D << ')';
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_ALIAS_TCC__

