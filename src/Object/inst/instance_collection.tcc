/**
	\file "Object/inst/instance_collection.tcc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	This file originally came from 
		"Object/art_object_instance_collection.tcc"
		in a previous life.  
	$Id: instance_collection.tcc,v 1.5.2.17 2005/08/26 21:11:04 fang Exp $
 */

#ifndef	__OBJECT_INST_INSTANCE_COLLECTION_TCC__
#define	__OBJECT_INST_INSTANCE_COLLECTION_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif
#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#endif

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
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"		// for debug only
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
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

//=============================================================================
// module-local specializations

namespace std {
using ART::entity::instance_alias;

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select1st<INSTANCE_ALIAS_CLASS> :
	public _Select1st<typename INSTANCE_ALIAS_CLASS::parent_type> {
};      // end struct _Select1st

INSTANCE_ALIAS_TEMPLATE_SIGNATURE
struct _Select2nd<INSTANCE_ALIAS_CLASS> :
	public _Select2nd<typename INSTANCE_ALIAS_CLASS::parent_type> {
};	// end struct _Select2nd
}	// end namespace std

//=============================================================================

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
// class instance_array member class definitions

/**
	Functor to collect transient info in the aliases.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_collector {
	persistent_object_manager& pom;
public:
	element_collector(persistent_object_manager& m) : pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct element_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to write alias elements.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_writer {
	ostream& os;
	const persistent_object_manager& pom;
public:
	element_writer(const persistent_object_manager& m, ostream& o)
		: os(o), pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct element_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to load alias elements.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::element_loader {
	istream& is;
	const persistent_object_manager& pom;
	collection_type& coll;
public:
	element_loader(const persistent_object_manager& m,
		istream& i, collection_type& c) :
		is(i), pom(m), coll(c) { }

	void
	operator () (void);
};      // end class element_loader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to write alias connections, continuation pointers.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::connection_writer {
	ostream& os;
	const persistent_object_manager& pom;
public:
	connection_writer(const persistent_object_manager& m,
		ostream& o) : os(o), pom(m) { }

	void
	operator () (const element_type& ) const;
};      // end struct connection_writer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to load alias connections, continuation pointers.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
class INSTANCE_ARRAY_CLASS::connection_loader {
	istream& is;
	const persistent_object_manager& pom;
public:
	connection_loader(const persistent_object_manager& m,
		istream& i) : is(i), pom(m) { }

	void
	operator () (const element_type& );
};      // end class connection_loader

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor to dump keys and alias information.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
struct INSTANCE_ARRAY_CLASS::key_dumper {
	ostream& os;

	key_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const value_type& );
};      // end struct key_dumper


//=============================================================================
// class instance_alias_info method definitions

INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
// inline
INSTANCE_ALIAS_INFO_CLASS::~instance_alias_info() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add more checks for relationship with parent instance
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
void
INSTANCE_ALIAS_INFO_CLASS::check(const container_type* p) const {
	if (this->container && this->container != p) {
		cerr << "FATAL: Inconsistent instance_alias_info parent-child!" << endl;
		cerr << "this->container = " << &*this->container << endl;
		this->container->dump(cerr) << endl;
		cerr << "should point to: " << p << endl;
		p->dump(cerr) << endl;
		DIE;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#if 1
		// check if this collection is top-level in the current
		// scope.  if so, then terminate upward recursion.  
		// this should be able to replace the is_port_formal() check.  
		if (f[this->container->get_name()] == this->container) {
			STACKTRACE("is top-level in current scope");
			return good_bool(true);
		}
#endif
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
#if 1
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
#endif
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
	typename instance_type::pool_type&
		the_pool(footprint_pool_getter<Tag>().operator()(f));
	_this.instance_index = the_pool.allocate(instance_type(*this));
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
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	const complete_type_type
		_type(actuals_parent_type::complete_type_actuals(
			*this->container));
	if (!_type) {
		// already have error message
		this->dump_hierarchical_name(cerr << "Failed to instantiate ")
			<< endl;
		return 0;
	}
	else if (!container_type::collection_type_manager_parent_type
			::create_definition_footprint(_type).good) {
		// have error message already
		this->dump_hierarchical_name(cerr << "Instantiated by: ")
			<< endl;
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
			const pool_type& the_pool
				// WTF, I can't just use (f) ?
				(footprint_pool_getter<Tag>().operator()(f));
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
		cerr << "Conflicting actuals in hierarchichal connections."
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
	INVARIANT(!this->instance_index);
	INVARIANT(t.instance_index);
	iterator i(this->begin());
	const iterator e(this->end());
	const instance_type&
		inst(footprint_pool_getter<Tag>()
			.operator()(f)[t.instance_index]);
	for ( ; i!=e; i++) {
		INVARIANT(!i->instance_index);
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
	DIE;
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
	STACKTRACE_VERBOSE;
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
// class instance_collection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), collection_type_manager_parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::instance_collection(const this_type& t, 
		const footprint& f) :
		parent_type(t, f),
		collection_type_manager_parent_type(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS::~instance_collection() {
	STACKTRACE_DTOR("~instance_collection<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
INSTANCE_COLLECTION_CLASS::type_dump(ostream& o) const {
	typename collection_type_manager_parent_type::dumper dump_it(o);
	return dump_it(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
typename INSTANCE_COLLECTION_CLASS::type_ref_ptr_type
INSTANCE_COLLECTION_CLASS::get_type_ref_subtype(void) const {
	return collection_type_manager_parent_type::get_type(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::must_match_type(const this_type& c) const {
	return collection_type_manager_parent_type::must_match_type(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::establish_collection_type(
		const instance_collection_parameter_type& t) {
	NEVER_NULL(t);
	collection_type_manager_parent_type::commit_type_first_time(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bool
INSTANCE_COLLECTION_CLASS::has_relaxed_type(void) const {
	return collection_type_manager_parent_type::is_relaxed_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	During unroll phase, this commits the type of the collection.  
	\param t the data integer type reference, containing width, 
		must already be resolved to a const_param_expr_list.  
	\return false on success, true on error.  
	\post the integer width is fixed for the rest of the program.  
	TODO: rename this!, doesn't commit anymore, just checks (const)
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_COLLECTION_CLASS::check_established_type(
		const instance_collection_parameter_type& t) const {
	// functor, specialized for each class
	return collection_type_manager_parent_type::check_type(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: update this description, nothing to do with context
	Create a meta instance reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
INSTANCE_COLLECTION_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<meta_instance_reference_base>	return_type;
	return return_type(new simple_meta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a nonmeta instance reference.  
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
INSTANCE_COLLECTION_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<nonmeta_instance_reference_base>	return_type;
	return return_type(new simple_nonmeta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
typename INSTANCE_COLLECTION_CLASS::member_inst_ref_ptr_type
INSTANCE_COLLECTION_CLASS::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new member_simple_meta_instance_reference_type(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to return a legitmate reference to a parameter list!
 */
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
never_ptr<const const_param_expr_list>
INSTANCE_COLLECTION_CLASS::get_actual_param_list(void) const {
	STACKTRACE("instance_collection::get_actual_param_list()");
	return never_ptr<const const_param_expr_list>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
INSTANCE_COLLECTION_CLASS*
INSTANCE_COLLECTION_CLASS::make_array(
		const scopespace& o, const string& n, const size_t d) {
	switch(d) {
		case 0: return new instance_array<Tag,0>(o, n);
		case 1: return new instance_array<Tag,1>(o, n);
		case 2: return new instance_array<Tag,2>(o, n);
		case 3: return new instance_array<Tag,3>(o, n);
		case 4: return new instance_array<Tag,4>(o, n);
		default:
			cerr << "FATAL: dimension limit is 4!" << endl;
			return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
	collection_type_manager_parent_type::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::write_base()");
	parent_type::write_object_base(m, o);
	// specialization functor parameter writer
	collection_type_manager_parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_COLLECTION_TEMPLATE_SIGNATURE
void
INSTANCE_COLLECTION_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_collection<Tag>::load_base()");
	parent_type::load_object_base(m, i);
	// specialization functor parameter loader
	collection_type_manager_parent_type::load_object_base(m, i);
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
// class array method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array() : parent_type(D), collection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, D), collection() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep-copy constructor for footprint management.  
	NOTE: this collection should be empty, but we copy it anyhow.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::instance_array(const this_type& t, const footprint& f) :
		parent_type(t, f), collection(t.collection) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
INSTANCE_ARRAY_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep copy constructor for the footprint.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
instance_collection_base*
INSTANCE_ARRAY_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::is_partially_unrolled(void) const {
	return !collection.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
ostream&
INSTANCE_ARRAY_CLASS::dump_unrolled_instances(ostream& o) const {
	for_each(this->collection.begin(), this->collection.end(),
		key_dumper(o));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE 
ostream&
INSTANCE_ARRAY_CLASS::key_dumper::operator () (const value_type& p) {
	os << auto_indent << _Select1st<value_type>()(p);
	NEVER_NULL(p.container);
	if (p.container->has_relaxed_type())
		p.dump_actuals(os);
	os << " = ";
	NEVER_NULL(p.get_next());
	p.get_next()->dump_alias(os);
	if (p.instance_index)
		os << " (" << p.instance_index << ')';
	p.dump_ports(os << ' ');
	return os << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instantiates integer parameters at the specified indices.
	TODO: change argument to take a const_range_list.  
	\param i fully-specified range of indices to instantiate.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::instantiate_indices(const const_range_list& ranges, 
		const instance_relaxed_actuals_type& actuals, 
		const unroll_context& c) {
	STACKTRACE("instance_array<Tag,D>::instantiate_indices()");
	// now iterate through, unrolling one at a time...
	// stop as soon as there is a conflict
	// later: factor this out into common helper class
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool err = false;
	do {
		const const_iterator iter(this->collection.find(key_gen));
		if (iter == collection.end()) {
			// then we can insert a new one
			// create with back-ref!
			const iterator
				new_elem(this->collection.insert(
					element_type(key_gen)));
			// recursive instantiation happens on construction
			// alternative to constructing and copying:
			// construct empty, then initialize the new reference.
			// establish back-link here.  
			// This is actually necessary for correctness as well.
			const_cast<instance_alias_base_type&>(
				static_cast<const instance_alias_base_type&>(
				*new_elem)).instantiate(
					never_ptr<const this_type>(this), c);
			// set its relaxed actuals!!! (if appropriate)
			if (actuals) {
			const bool attached(new_elem->attach_actuals(actuals));
			if (!attached) {
				cerr << "ERROR: attaching relaxed actuals to "
					<< this->get_qualified_name() <<
					key_gen << endl;
				err = true;
			}
			}
		} else {
			// found one that already exists!
			// more detailed message, please!
			cerr << "ERROR: Index " << key_gen << " of ";
			what(cerr) << ' ' << this->get_qualified_name() <<
				" already instantiated!" << endl;
			err = true;
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return good_bool(!err);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates uniquely allocated space for aliases instances.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::create_unique_state(const const_range_list& ranges, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	multikey_generator<D, pint_value_type> key_gen;
	ranges.make_multikey_generator(key_gen);
	key_gen.initialize();
	do {
		// should be iterator, not const_iterator
		const const_iterator iter(this->collection.find(key_gen));
		INVARIANT(iter != collection.end());
		if (!iter->allocate_state(f))
			return good_bool(false);
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Like create_unique_state except it operates on the entire collection.
	Called from subinstance_manager::create_state.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::allocate_state(footprint& f) {
	STACKTRACE_VERBOSE;
	iterator i(collection.begin());
	const iterator e(collection.end());
	for ( ; i!=e; i++) {
		if (!i->__allocate_state(f))
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Expands indices which may be under-specified into explicit
        indices for the implicit subslice, if it is densely packed.
        Depends on the current state of the collection.
        \param l is list of indices, which may be under-specified,
                or even empty.
        \return fully-specified index list, or empty list if there is error.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_ARRAY_CLASS::resolve_indices(const const_index_list& l) const {
	const size_t l_size = l.size();
	if (D == l_size) {
		// already fully specified
		return l;
	}
	// convert indices to pair of list of multikeys
	if (!l_size) {
		return const_index_list(l, this->collection.is_compact());
	}
	// else construct slice
	list<pint_value_type> lower_list, upper_list;
	transform(l.begin(), l.end(), back_inserter(lower_list),
		unary_compose(
			mem_fun_ref(&const_index::lower_bound),
			dereference<count_ptr<const const_index> >()
		)
	);
	transform(l.begin(), l.end(), back_inserter(upper_list),
		unary_compose(
			mem_fun_ref(&const_index::upper_bound),
			dereference<count_ptr<const const_index> >()
		)
	);
	return const_index_list(l,
		collection.is_compact_slice(lower_list, upper_list));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_ptr_type
INSTANCE_ARRAY_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	typename INSTANCE_ARRAY_CLASS::instance_alias_base_ptr_type
							return_type;
	INVARIANT(D == i.dimensions());
	const key_type index(i);
	const const_iterator it(this->collection.find(index));
	if (it == this->collection.end()) {
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			i << endl;
		return return_type(NULL);
	}
	const element_type& b(*it);
	if (b.valid()) {
		// unfortunately, this cast is necessary
		// safe because we know b is not a reference to a temporary
		return return_type(const_cast<element_type*>(&b));
	} else {
		// remove the blank we added?
		// not necessary, but could keep the collection "clean"
		this->type_dump(
			cerr << "ERROR: reference to uninstantiated ") <<
			" " << this->get_qualified_name() << " at index: " <<
			i << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param l list in which to accumulate instance references.
	\param r the ranges, must be valid, and fully resolved.
	\return false on error, e.g. if value doesn't exist or
		is uninitialized; true on success.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bool
INSTANCE_ARRAY_CLASS::lookup_instance_collection(
		list<instance_alias_base_ptr_type>& l,
		const const_range_list& r) const {
	INVARIANT(!r.empty());
	key_generator_type key_gen;
	r.make_multikey_generator(key_gen);
	key_gen.initialize();
	bool ret = true;
	do {
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection.end()) {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
			ret = false;
		} else {
		const element_type& pi(*it);
		// pi MUST be valid if it belongs to an array
		if (pi.valid()) {
			l.push_back(instance_alias_base_ptr_type(
				const_cast<element_type*>(&pi)));
		} else {
			this->type_dump(
				cerr << "FATAL: reference to uninstantiated ")
					<< " index " << key_gen << endl;
			l.push_back(instance_alias_base_ptr_type(NULL));
			ret = false;
		}
		}
		key_gen++;
	} while (key_gen != key_gen.get_lower_corner());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Packs resolved range of aliases into a collection.  
	\pre array a must already be sized properly, do not resize here.  
	\return true on error, else false.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_ARRAY_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	typedef	typename alias_collection_type::key_type
						collection_key_type;
	typedef	typename alias_collection_type::iterator
						alias_collection_iterator;
	const key_type lower(l);	// this will assert dimension match!
	const key_type upper(u);	// this will assert dimension match!
	key_generator_type key_gen(lower, upper);
	key_gen.initialize();
	bool ret = false;
	alias_collection_iterator a_iter(a.begin());
	const const_iterator collection_end(this->collection.end());
	// maybe INVARIANT(sizes == iterations)
	do {
		// really is a monotonic incremental search, 
		// don't need log(N) lookup each time, fix later...
		const const_iterator it(this->collection.find(key_gen));
		if (it == collection_end) {
			this->type_dump(
			cerr << "FATAL: reference to uninstantiated ") <<
				" index " << key_gen << endl;
			*a_iter = never_ptr<element_type>(NULL);
			ret = true;
		} else {
			const element_type& pi(*it);
			*a_iter = never_ptr<element_type>(
				const_cast<element_type*>(&pi));
		}
		a_iter++;
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
	INVARIANT(a_iter == a.end());
	return bad_bool(ret);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands this collection into a copy for a port formal.  
	\return owned pointer to new created collection.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
count_ptr<physical_instance_collection>
INSTANCE_ARRAY_CLASS::unroll_port_only(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<this_type> ret(new this_type(*this));
	NEVER_NULL(ret);
	// Is this really copy-constructible?
	// TODO: unroll first instantiation statement
	INVARIANT(this->index_collection.size() == 1);	// port constraint
	const index_collection_type::const_iterator
		b(this->index_collection.begin());
	INVARIANT(*b);
	if ((*b)->instantiate_port(c, *ret).good)
		return ret;
	else 	return count_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Merges two instance collections, assigning them the same allocated
	state.  
	Called by subinstance_manager::create_state(), 
		during create-unique phase.  
	this and t must be port subinstances, 
		and hence, must be densely packed.  
	Since collection p was type/size checked during connection, 
	we can conclude that they have the same size.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::merge_created_state(physical_instance_collection& p, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		if (!ii.merge_allocate_state(jj, f).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::inherit_created_state(
		const physical_instance_collection& p, const footprint& f) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	const_iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		ii.inherit_subinstances_state(*j, f);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively synchronize relaxed actuals of ports.  
	Even though this type may not have relaxed actuals, 
	the ports (and ports-of-ports) might, so recursion is necessary.  
	This is called at create-time. 
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
good_bool
INSTANCE_ARRAY_CLASS::synchronize_actuals(physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	INVARIANT(this->collection.size() == t.collection.size());
	iterator i(this->collection.begin());
	iterator j(t.collection.begin());
	const iterator e(this->collection.end());
	for ( ; i!=e; i++, j++) {
		// unfortunately, set iterators only return const refs
		// we only intend to modify the value without modifying the key
		element_type& ii(const_cast<element_type&>(
			AS_A(const element_type&, *i)));
		element_type& jj(const_cast<element_type&>(
			AS_A(const element_type&, *j)));
		if (!element_type::synchronize_actuals_recursive(ii, jj).good) {
			// error message?
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads a key from binary stream then returns a reference to the 
	indexed instance alias.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
typename INSTANCE_ARRAY_CLASS::instance_alias_base_type&
INSTANCE_ARRAY_CLASS::load_reference(istream& i) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_reference()");
	key_type k;
	value_reader<key_type> read_key(i);
	read_key(k);
	const iterator it(collection.find(k));
	INVARIANT(it != this->collection.end());
	// need const cast because set only returns const references/iterators
	return const_cast<element_type&>(*it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_collector::operator () (
		const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_collector::operator()");
	e.collect_transient_info_base(pom);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Going to need some sort of element_reader counterpart.
	\param e is a reference to a INSTANCE_ALIAS_CLASS.
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_writer::operator () (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_writer::operator()");
	value_writer<key_type> write_key(os);
	write_key(e.key);
	e.write_object_base(pom, os);
	// postpone connection writing until next phase
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This must perfectly complement element_writer::operator().
	construct the element locally first, then insert it into set.
	BUG FIX NOTE: 2005-07-16:
		Loading the object *after* inserting it into the collection
		is not only an enhancement (eliminating copy-overhead), 
		but absolutely necessary for correctness, because effective 
		loading of the object base requires that the parent object 
		be stable, i.e.  not have short lifetime.  
		Operating on the newly inserted reference guarantees 
		proper lifetime.  This also applies to 
		instance_collection<>::instantiate_indices.
		This is not an issue with scalar instances, because
		they have the same lifetime as their parent collection, 
		by construction.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::element_loader::operator () (void) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::element_loader::operator()");
	typedef	typename collection_type::iterator	local_iterator;
	key_type temp_key;
	value_reader<key_type> read_key(this->is);
	read_key(temp_key);
	const element_type temp_elem(temp_key);
	const local_iterator i(this->coll.insert(temp_elem));
	const_cast<element_type&>(static_cast<const element_type&>(*i))
		.load_object_base(this->pom, this->is);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_writer::operator() (const element_type& e) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_writer::operator()");
	const instance_alias_base_type* const next(e.get_next());
	NEVER_NULL(next);
	if (next != &e) {
		write_value<char>(os, 1);
		next->write_next_connection(pom, os);
	} else {
		write_value<char>(os, 0);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must complement connection_writer::operator().
	const_cast is an unfortunate consequence of set only
	returning const references and const iterators, where we intend
	the non-key part of the object to me mutable.  
 */
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::connection_loader::operator() (const element_type& e) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::connection_loader::operator()");
	char c;
	read_value(this->is, c);
	if (c) {
		element_type& elem(const_cast<element_type&>(e));
		// lookup the instance in the collection referenced
		// and connect them
		elem.load_next_connection(this->pom, this->is);
	}
	// else just leave it pointing to itself, 
	// which was how it was constructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, D)) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::collect_transients()");
	parent_type::collect_transient_info_base(m);
	for_each(this->collection.begin(), this->collection.end(), 
		element_collector(m)	// added 2005-07-07
	);
	// optimization for later: factor this out into a policy
	// so that collections without pointers to collect
	// may be skipped.
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::write_object()");
	parent_type::write_object_base(m, f);
	// need to know how many members to expect
	write_value(f, this->collection.size());
	for_each(this->collection.begin(), this->collection.end(),
		element_writer(m, f)
	);
	for_each(this->collection.begin(), this->collection.end(), 
		connection_writer(m, f)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ARRAY_TEMPLATE_SIGNATURE
void
INSTANCE_ARRAY_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_array<Tag,D>::load_object()");
	parent_type::load_object_base(m, f);
	// procedure:
	// 1) load all instantiated indices *without* their connections
	//      let them start out pointing to themselves.  
	// 2) each element contains information to reconstruct, 
	//      we need temporary local storage for it.
	size_t collection_size;
	read_value(f, collection_size);
	size_t i = 0;
	element_loader load_element(m, f, this->collection);
	for ( ; i < collection_size; i++) {
		// this must perfectly complement element_writer::operator()
		// construct the element locally first, then insert it into set
		load_element();
	}
	for_each(collection.begin(), collection.end(),
		connection_loader(m, f)
	);
}

//=============================================================================
// class array method definitions (specialized)

#if 0
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(bool_scalar, 256)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array() : parent_type(0), the_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const scopespace& o, const string& n) :
		parent_type(o, n, 0), the_instance() {
	// until we eliminate that field from instance_collection_base
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial deep-copy constructor for footprint management.  
	NOTE: this collection should be empty, but we copy it anyhow.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::instance_array(const this_type& t, const footprint& f) :
		parent_type(t, f), the_instance(t.the_instance) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
INSTANCE_SCALAR_CLASS::~instance_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deep (partial) copy-constructor for footprint.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
instance_collection_base*
INSTANCE_SCALAR_CLASS::make_instance_collection_footprint_copy(
		const footprint& f) const {
	return new this_type(*this, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::is_partially_unrolled(void) const {
	return this->the_instance.valid();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
ostream&
INSTANCE_SCALAR_CLASS::dump_unrolled_instances(ostream& o) const {
	// no auto-indent, continued on same line
	// see physical_instance_collection::dump for reason why
	if (this->the_instance.container->has_relaxed_type()) {
		this->the_instance.dump_actuals(o);
	}
	this->the_instance.get_next()->dump_alias(o << " = ");
	if (this->the_instance.instance_index)
		o << " (" << this->the_instance.instance_index << ')';
	this->the_instance.dump_ports(o << ' ');
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
/**
	Instantiates the_instance of integer datatype.
	Ideally, the error should never trigger because
	re-instantiation / redeclaration of a scalar instance
	is easily detected (and actually detected) during the compile phase.
	\param i indices must be NULL because this is not an array.
 */
good_bool
INSTANCE_SCALAR_CLASS::instantiate_indices(
		const const_range_list& r, 
		const instance_relaxed_actuals_type& actuals, 
		const unroll_context& c) {
	STACKTRACE("instance_array<Tag,0>::instantiate_indices()");
	INVARIANT(r.empty());
	if (this->the_instance.valid()) {
		// should never happen, but just in case...
		this->type_dump(cerr << "ERROR: Scalar ") <<
			" already instantiated!" << endl;
		return good_bool(false);
	}
	// here we need an explicit instantiation (recursive)
	this->the_instance.instantiate(never_ptr<const this_type>(this), c);
	const bool attached(actuals ?
		this->the_instance.attach_actuals(actuals) : true);
	if (!attached) {
		cerr << "ERROR: attaching relaxed actuals to scalar ";
		this->type_dump(cerr) << " " << this->get_qualified_name()
			<< endl;
	}
	return good_bool(attached);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates state for a single instance alias.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::create_unique_state(const const_range_list& ranges, 
		footprint& f) {
	STACKTRACE("instance_array<Tag,0>::create_unique_state()");
	INVARIANT(ranges.empty());
	INVARIANT(this->the_instance.valid());
	return good_bool(this->the_instance.allocate_state(f));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as create_unique_state.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::allocate_state(footprint& f) {
	STACKTRACE_VERBOSE;
	INVARIANT(this->the_instance.valid());
	return good_bool(this->the_instance.__allocate_state(f) != 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This specialization isn't ever supposed to be called.
	\param l is list of indices, which may be under-specified,
		or even empty.
	\return empty index list, always.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
const_index_list
INSTANCE_SCALAR_CLASS::resolve_indices(const const_index_list& l) const {
	cerr << "WARNING: instance_array<Tag,0>::resolve_indices(const_index_list) "
		"always returns an empty list!" << endl;
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return valid instance_alias if found, else an invalid one.  
	Caller is responsible for checking return.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
INSTANCE_SCALAR_CLASS::lookup_instance(const multikey_index_type& i) const {
	typedef	typename INSTANCE_SCALAR_CLASS::instance_alias_base_ptr_type
						return_type;
	if (!this->the_instance.valid()) {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return return_type(NULL);
	} else	return return_type(
		const_cast<instance_alias_base_type*>(
			&static_cast<const instance_alias_base_type&>(
				this->the_instance)));
	// ok to return non-const reference to the type, 
	// perhaps it should be declared mutable?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should never be called.  
	\return false to signal error.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bool
INSTANCE_SCALAR_CLASS::lookup_instance_collection(
		list<instance_alias_base_ptr_type>& l,
		const const_range_list& r) const {
	cerr << "WARNING: instance_array<Tag,0>::lookup_instance_collection(...) "
		"should never be called." << endl;
	INVARIANT(r.empty());
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error, false on success.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
bad_bool
INSTANCE_SCALAR_CLASS::unroll_aliases(const multikey_index_type& l, 
		const multikey_index_type& u, alias_collection_type& a) const {
	if (this->the_instance.valid()) {
		*(a.begin()) = instance_alias_base_ptr_type(
			const_cast<instance_alias_base_type*>(
				&static_cast<const instance_alias_base_type&>(
					this->the_instance)));
		return bad_bool(false);
	} else {
		this->type_dump(cerr << "ERROR: Reference to uninstantiated ")
			<< "!" << endl;
		return bad_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands this collection into a copy for a port formal.  
	\return owned pointer to new created collection.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
count_ptr<physical_instance_collection>
INSTANCE_SCALAR_CLASS::unroll_port_only(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<this_type> ret(new this_type(*this));
	NEVER_NULL(ret);
	// Is this really copy-constructible?
	INVARIANT(this->index_collection.size() == 1);	// port constraint
	const index_collection_type::const_iterator
		b(this->index_collection.begin());
	INVARIANT(*b);
	if ((*b)->instantiate_port(c, *ret).good)
		return ret;
	else 	return count_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Merges two instances, assigning them the same allocated state.  
	Called by subinstance_manager::create_state(), 
		during create-unique phase.  
	this and t must be port subinstances.
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::merge_created_state(physical_instance_collection& p, 
		footprint& f) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return this->the_instance.merge_allocate_state(t.the_instance, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::inherit_created_state(
		const physical_instance_collection& p, const footprint& f) {
	STACKTRACE_VERBOSE;
	const this_type& t(IS_A(const this_type&, p));	// assert dynamic_cast
	this->the_instance.inherit_subinstances_state(t.the_instance, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called at create-time to check that the implicit connections
	in the port hierarchy are compatible, w.r.t. relaxed actual
	parameters.  
 */
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
good_bool
INSTANCE_SCALAR_CLASS::synchronize_actuals(physical_instance_collection& p) {
	STACKTRACE_VERBOSE;
	this_type& t(IS_A(this_type&, p));	// assert dynamic_cast
	return instance_type::synchronize_actuals_recursive(this->the_instance,
		t.the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
typename INSTANCE_SCALAR_CLASS::instance_alias_base_type&
INSTANCE_SCALAR_CLASS::load_reference(istream& i) const {
	STACKTRACE_PERSISTENT("instance_scalar::load_reference()");
	// no key to read!
	// const_cast: have to modify next pointers to re-establish connection, 
	// which is semantically allowed because we allow the alias pointers
	// to be mutable.  
	return const_cast<instance_type&>(this->the_instance);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key, 0)) {
	STACKTRACE_PERSISTENT("instance_scalar::collect_transients()");
	parent_type::collect_transient_info_base(m);
	this->the_instance.check(this);
	this->the_instance.collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	STACKTRACE_PERSISTENT("instance_scalar::write_object()");
	parent_type::write_object_base(m, f);
	this->the_instance.write_object(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_SCALAR_TEMPLATE_SIGNATURE
void
INSTANCE_SCALAR_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("instance_scalar::load_object()");
	parent_type::load_object_base(m, f);
	this->the_instance.load_object(m, f);		// problem?
	this->the_instance.check(this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INSTANCE_COLLECTION_TCC__

