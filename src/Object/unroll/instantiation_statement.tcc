/**
	\file "Object/unroll/instantiation_statement.tcc"
	Method definitions for instantiation statement classes.  
	This file's previous revision history is in
		"Object/art_object_inst_stmt.tcc"
 	$Id: instantiation_statement.tcc,v 1.29 2010/01/03 01:34:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TCC__
#define	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overrideable debug switches


#ifndef	DEBUG_LIST_VECTOR_POOL
#define	DEBUG_LIST_VECTOR_POOL				0
#endif
#ifndef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#endif

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE				0
#endif

#include <iostream>
#include <algorithm>

#include "Object/type/fundamental_type_reference.hh"
#include "Object/unroll/instantiation_statement.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/meta_range_list.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/def/footprint.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/common/dump_flags.hh"
#include "Object/inst/instance_placeholder.hh"
#include "Object/inst/value_placeholder.hh"

#include "common/ICE.hh"

#include "util/what.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.hh"

//=============================================================================
namespace HAC {
namespace entity {
class const_param_expr_list;
#include "util/using_ostream.hh"
using util::persistent_traits;
#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// class INSTANTIATION_STATEMENT_CLASS method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(instantiation_statement, 128)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
INSTANTIATION_STATEMENT_CLASS::instantiation_statement() :
		parent_type(), type_ref_parent_type(), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
INSTANTIATION_STATEMENT_CLASS::instantiation_statement(
		const index_collection_item_ptr_type& i) :
		parent_type(i), type_ref_parent_type(), inst_base(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
INSTANTIATION_STATEMENT_CLASS::instantiation_statement(
		const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(i), type_ref_parent_type(t), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
INSTANTIATION_STATEMENT_CLASS::instantiation_statement(
		const placeholder_ptr_type p, 
		const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(i), type_ref_parent_type(t), inst_base(p) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
INSTANTIATION_STATEMENT_CLASS::~instantiation_statement() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
ostream&
INSTANTIATION_STATEMENT_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
ostream&
INSTANTIATION_STATEMENT_CLASS::dump(ostream& o,
		const expr_dump_context& dc) const {
	return parent_type::dump(o, dc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
	NOTE: not attaching this to inst_base here anymore... hassle.
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::attach_collection(
		const never_ptr<const instance_placeholder_base> i) {
	INVARIANT(!this->inst_base);
	NEVER_NULL(i);
	const placeholder_ptr_type c(i.template is_a<const placeholder_type>());
	NEVER_NULL(c);
	this->inst_base = c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
never_ptr<const instance_placeholder_base>
INSTANTIATION_STATEMENT_CLASS::get_inst_base(void) const {
	NEVER_NULL(this->inst_base);
	return this->inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
INSTANTIATION_STATEMENT_CLASS::get_type_ref(void) const {
	return type_ref_parent_type::get_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Covariant return version.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
typename INSTANTIATION_STATEMENT_CLASS::type_ref_ptr_type
INSTANTIATION_STATEMENT_CLASS::get_type_ref_subtype(void) const {
	return type_ref_parent_type::get_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: rewrite cleaner after reworking code (2006-10-03)
	Interprets a physical instantiation statement and instantiates
	the members of the collection specified.  
	(this will require some serious specialization)
	\param c the unroll context.  
	\return good if successful, else false.
	TODO: clean-up the hack(s) in here
	TODO: use a consistent policy/trait class to specialize implementations
		of type checking for each meta class.  
		Don't go through common class for type-references.  
	TODO: we unroll the canonical_type if it is complete, 
		i.e. not relaxed or relaxed with relaxed actuals.  
		For now, we punt on relaxed types until a future date.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
INSTANTIATION_STATEMENT_CLASS::unroll(const unroll_context& c) const {
	typedef	typename type_ref_ptr_type::element_type	element_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT << "statement: ",
		expr_dump_context::default_value) << endl;
	c.dump(cerr << "context: ") << endl;
#endif
	NEVER_NULL(this->inst_base);

	// only lookup target footprint in context!
	never_ptr<collection_type>
		inst_ptr(c.lookup_target_collection(*this->inst_base)
			.template is_a<collection_type>());
	if (!inst_ptr) {
		STACKTRACE_INDENT_PRINT("collection not found in target footprint" << endl);
		// we pass the whole footprint because only the caller
		// knows whether to allocate a value_collection 
		// vs. instance_collection
		inst_ptr = never_ptr<collection_type>(
			this->inst_base->make_collection(
				c.get_target_footprint()));
		NEVER_NULL(inst_ptr);
	}
	collection_type& _inst(*inst_ptr);
	// 2005-07-07:
	// HACK: detect that this is the first type commit to the 
	// collection, because unroll_type_reference combines the
	// strict and relaxed actuals of the instantiation_statement, 
	// we can't tell from the unrolled type whether or not the 
	// relaxed parameters were intended for the whole collection
	// or just the specified indices.  
	// Resolution: detect the first time, and do a first-time
	// commit using the type_ref_parent_type's original type, 
	// which will be distinguishably strict or relaxed.  
	const bool first_time = !_inst.is_partially_unrolled();
	if (first_time) {
		STACKTRACE_INDENT_PRINT("first type-commit to collection" << endl);
		const instance_collection_parameter_type
			cft(type_ref_parent_type::get_canonical_type(c));
		if (!cft) {
			// already have error message
			return good_bool(false);
		}
		// note: commit_type_first_time also unrolls the complete type
		if (!type_ref_parent_type::commit_type_first_time(
				_inst, cft, *c.get_top_footprint()).good) {
			type_ref_parent_type::get_type()->dump(
				cerr << "Instantiated from: ") << endl;
			return good_bool(false);
		}
		// _inst.establish_collection_type(ft);
	}
	// unroll_type_check is specialized for each tag type.  
	// NOTE: this results in a "fused" type that combines
	// the relaxed template actuals.  
	// we forgot to canonicalize?
	const type_ref_ptr_type
		temp_type_ref(type_ref_parent_type::unroll_type_reference(c));
	if (!temp_type_ref) {
		this->get_type()->what(cerr << "ERROR: unable to resolve ") <<
			" during unroll." << endl;
		return good_bool(false);
	}
	const instance_collection_parameter_type
		final_type_ref(temp_type_ref->make_canonical_type());
	if (!final_type_ref) {
		this->get_type()->what(cerr << "ERROR: unable to resolve ") <<
			" during unroll." << endl;
		return good_bool(false);
	}
	// note: commit_type_check also unrolls the complete type
	STACKTRACE_INDENT_PRINT("commit_type_check" << endl);
	const good_bool
		tc(type_ref_parent_type::commit_type_check(
			_inst, final_type_ref, *c.get_top_footprint()));
	// should be optimized away where there is no type-check to be done
	if (!tc.good) {
		cerr << "ERROR: type-mismatch during " <<
			util::what<this_type>::name() <<
			"::unroll." << endl;
		return good_bool(false);
	}
	// indices can be resolved to constants with unroll context.  
	// still implicit until expanded by the collection itself.  
	const_range_list crl;
	const good_bool rr(this->resolve_instantiation_range(crl, c));
	if (rr.good) {
		// actuals are allowed to be NULL, and in some cases,
		// will be required to be NULL, e.g. for types that never
		// have relaxed actuals.  
		STACKTRACE_INDENT_PRINT("&_inst = " << &_inst << endl);
		// doesn't actually carry actuals any more
		return type_ref_parent_type::instantiate_indices_with_actuals(
				_inst, crl, c);
	} else {
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
		return good_bool(false);
	}
	return good_bool(true);
}	// end instantiation_statement<>::unroll

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this does NOT touch the referenced instance_collection, 
		since that refers to a port-formal.  
	TODO: instate a check to make sure that only top-level
		instances are unrolled, and only port-formals call this.  
	\pre once and only instantiation for the port collection.  
	TODO: 2005-07-18 problem: the type-reference may depend on
		parameters passed into through the context, and
		self-dependent (dependent on own parameters, such as defaults)
		parameters in the same actuals list, however, unroll_context 
		currently only supports one level context.  
		Thus, we need to partially resolve SOME of the 
		parameters first.  
	TODO: what about placeholder?
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
INSTANTIATION_STATEMENT_CLASS::instantiate_port(const unroll_context& c,
		physical_instance_collection& p) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	c.dump(cerr << "context: ") << endl;
#endif
	// dynamic cast assertion, until we fix class hierarchy
	collection_type& coll(IS_A(collection_type&, p));
	INVARIANT(!coll.is_partially_unrolled());
	const instance_collection_parameter_type
		ft(type_ref_parent_type::get_canonical_type(c));
	// ft will either be strict or relaxed.  
	type_ref_parent_type::commit_type_first_time(
		coll, ft, *c.get_top_footprint());
	// no need to re-evaluate type, since get_resolved_type is
	// (for now) the same as unroll_type_reference.
	const bool good __ATTRIBUTE_UNUSED_CTOR__((
		type_ref_parent_type::commit_type_check(
			coll, ft, *c.get_top_footprint()).good));
	INVARIANT(good);
	// everything below is copied from unroll(), above
	// TODO: factor out common code.  
	// TODO: anything related to relaxed actuals should be policy-determined
	const_range_list crl;
	const good_bool rr(this->resolve_instantiation_range(crl, c));
	if (!rr.good) {
		// consider different message
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
		return good_bool(false);
	}
	// actuals are allowed to be NULL, and in some cases,
	// will be required to be NULL, e.g. for types that never
	// have relaxed actuals.  
	return type_ref_parent_type::instantiate_indices_with_actuals(
			coll, crl, c);
}	// end method instantiate_port

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	NEVER_NULL(this->inst_base);
	// let the scopespace take care of it
	inst_base->collect_transient_info(m);
	type_ref_parent_type::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, this->inst_base);
	parent_type::write_object_base(m, f);
	type_ref_parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, this->inst_base);
	parent_type::load_object_base(m, f);
	type_ref_parent_type::load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TCC__

