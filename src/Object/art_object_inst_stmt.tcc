/**
	\file "Object/art_object_inst_stmt.tcc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.tcc,v 1.5.4.10 2005/07/09 01:23:26 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_TCC__
#define	__OBJECT_ART_OBJECT_INST_STMT_TCC__

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
#ifndef	STACKTRACE_DESTRUCTORS
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#endif
#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include <algorithm>

#include "Object/art_object_type_ref.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/expr/meta_range_list.h"

#include "util/what.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.h"
#include "util/stacktrace.h"

// conditional defines, after inclusion of "stacktrace.h"
#ifndef	STACKTRACE_DTOR
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif
#endif

#ifndef	STACKTRACE_PERSISTENT
#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif
#endif

//=============================================================================
namespace ART {
namespace entity {
class const_param_expr_list;
USING_STACKTRACE
#include "util/using_ostream.h"
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
		const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i, 
		const const_relaxed_args_type& a) :
		parent_type(i), type_ref_parent_type(t, a), inst_base(NULL) {
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
INSTANTIATION_STATEMENT_CLASS::dump(ostream& o) const {
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!this->inst_base);
	this->inst_base = i.template is_a<collection_type>();
	NEVER_NULL(this->inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
never_ptr<instance_collection_base>
INSTANTIATION_STATEMENT_CLASS::get_inst_base(void) {
	NEVER_NULL(this->inst_base);
	return this->inst_base.template as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
never_ptr<const instance_collection_base>
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
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
instantiation_statement_base::const_relaxed_args_type
INSTANTIATION_STATEMENT_CLASS::get_relaxed_actuals(void) const {
	return type_ref_parent_type::get_relaxed_actuals();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets a physical instantiation statement and instantiates
	the members of the collection specified.  
	(this will require some serious specialization)
	\param c the unroll context.  
	\return good if successful, else false.
	TODO: clean-up the hack in here
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
INSTANTIATION_STATEMENT_CLASS::unroll(unroll_context& c) const {
	NEVER_NULL(this->inst_base);
#if 1
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
	const bool first_time = !this->inst_base->is_partially_unrolled();
	if (first_time) {
		const type_ref_ptr_type
			ft(type_ref_parent_type::get_resolved_type(c));
		if (!ft) {
			// already have error message
#if 0
			t->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
#endif
			return good_bool(false);
		}
		// ft will either be strict or relaxed.  
		type_ref_parent_type::commit_type_first_time(
			*this->inst_base, ft);
		// this->inst_base->establish_collection_type(ft);
	}
#endif
	// unroll_type_check is specialized for each tag type.  
	// NOTE: this results in a "fused" type that combines
	// the relaxed template actuals.  
	const type_ref_ptr_type
		final_type_ref(type_ref_parent_type::unroll_type_reference(c));
	if (!final_type_ref) {
		this->get_type()->what(cerr << "ERROR: unable to resolve ") <<
			" during unroll." << endl;
		return good_bool(false);
	}
	// this would be a good time to "unroll" a complete definition's
	// substructure map, before comitting it to the collection.  
	// Details: register the type with the principle base definition.
#if 0
	final_type_ref->unroll_register_complete_type();
#endif
	// TODO: decide what do to about relaxed type parameters
	// 2005-07-07: answer is above under "HACK"
	const good_bool
		tc(type_ref_parent_type::commit_type_check(
			*this->inst_base, final_type_ref));
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
#if 0
		// need to pass in relaxed arguments from final_type_ref!
		this->inst_base->instantiate_indices(crl);
		// final_type_ref->get_template_params().get_relaxed_actuals());
#else
#if 0
		final_type_ref->dump(cerr) << endl;
#endif
		const count_ptr<const param_expr_list>
			relaxed_actuals(final_type_ref->get_template_params()
				.get_relaxed_args());
		// should correspond to
		//	instance_collection_base::instance_relaxed_actuals_type
		const count_ptr<const const_param_expr_list>
			relaxed_const_actuals(relaxed_actuals.
				template is_a<const const_param_expr_list>());
#if 0
		if (relaxed_const_actuals)
			relaxed_const_actuals->dump(cerr << '<') << '>' << endl;
#endif
		// really shouldn't be dynamic if the actuals have been resolved
		if (relaxed_actuals && !relaxed_const_actuals) {
			cerr << "Internal compiler error: expected "
				"const_param_expr_list in "
				"instantiation_statement<>::unroll." << endl;
			relaxed_actuals->dump(cerr << "\tgot: ") << endl;
			THROW_EXIT;
		}
		// actuals are allowed to be NULL, and in some cases,
		// will be required to be NULL, e.g. for types that never
		// have relaxed actuals.  
		return type_ref_parent_type::instantiate_indices_with_actuals(
				*this->inst_base, crl, relaxed_const_actuals);
#endif
	} else {
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
good_bool
INSTANTIATION_STATEMENT_CLASS::unroll_meta_instantiate(
		unroll_context& c) const {
	// would've exited already
	return this->unroll(c);
}

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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_TCC__

