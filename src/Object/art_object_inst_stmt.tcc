/**
	\file "art_object_inst_stmt.tcc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.tcc,v 1.1.2.7 2005/03/11 08:38:34 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_TCC__
#define	__ART_OBJECT_INST_STMT_TCC__

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

#include "art_object_inst_stmt.h"

#include "what.tcc"
#include "memory/list_vector_pool.tcc"
#include "persistent_object_manager.h"
#include "stacktrace.h"

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
USING_STACKTRACE
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
INSTANTIATION_STATEMENT_CLASS::~instantiation_statement() {
}

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
/**
	this will require some serious specialization
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::unroll(unroll_context& c) const {
	NEVER_NULL(this->inst_base);
	// unroll_type_check is specialized for each tag type.  
	const type_ref_ptr_type
		final_type_ref(type_ref_parent_type::unroll_type_reference(c));
	if (!final_type_ref) {
		this->get_type()->what(cerr << "ERROR: unable to resolve ") <<
			" during unroll." << endl;
		THROW_EXIT;
	}
	const good_bool
		tc(type_ref_parent_type::commit_type_check(
			*this->inst_base, final_type_ref));
	// should be optimized away where there is no type-check to be done
	if (!tc.good) {
		cerr << "ERROR: type-mismatch during " <<
			util::what<this_type>::name() <<
			"::unroll." << endl;
		THROW_EXIT;
	}
	// indices can be resolved to constants with unroll context.  
	// still implicit until expanded by the collection itself.  
	const_range_list crl;
	const good_bool rr(this->resolve_instantiation_range(crl, c));
	if (rr.good) {
		this->inst_base->instantiate_indices(crl);
	} else {
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
void
INSTANTIATION_STATEMENT_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(this->inst_base);
	// let the scopespace take care of it
	inst_base->collect_transient_info(m);
	type_ref_parent_type::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
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

#endif	// __ART_OBJECT_INST_STMT_TCC__

