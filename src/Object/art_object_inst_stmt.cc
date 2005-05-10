/**
	\file "Object/art_object_inst_stmt.cc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.cc,v 1.18 2005/05/10 04:51:15 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_CC__
#define	__OBJECT_ART_OBJECT_INST_STMT_CC__

// for debugging only, before inclusion of any header files
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

#include <iostream>
#include <algorithm>

#include "Object/art_object_type_ref.h"
#include "Object/art_object_instance.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_inst_stmt.h"
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_expr_base.h"
#include "Object/art_built_ins.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_unroll_context.h"
#include "Object/art_object_classification_details.h"
#include "Object/art_object_instance_collection.h"
#include "Object/art_object_value_collection.h"

#include "Object/art_object_inst_stmt_param.h"
#include "Object/art_object_inst_stmt_data.h"
#include "Object/art_object_inst_stmt_chan.h"
#include "Object/art_object_inst_stmt_proc.h"

#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/static_trace.h"

// conditional defines, after inclusion of "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif

#include "Object/art_object_inst_stmt.tcc"

//=============================================================================
// local specializations
// Alternatively, explicit specialization here guarantees that the
// static initialization occurs in the correct order in this module.  
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::data_instantiation_statement,
	"data_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_instantiation_statement,
	"pint_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_instantiation_statement,
	"pbool_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::process_instantiation_statement,
	"process_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::channel_instantiation_statement,
	"channel_instantiation_statement")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_instantiation_statement, 
		PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_instantiation_statement, 
		PINT_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instantiation_statement, 
		PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_instantiation_statement, 
		CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::data_instantiation_statement, 
		DATA_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
// start of static initializations
STATIC_TRACE_BEGIN("inst_stmt")

//=============================================================================
namespace ART {
namespace entity {
USING_STACKTRACE
using util::persistent_traits;
#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// class instantiation_statement_base method definitions

#if 0
/**	Private empty constructor. */
instantiation_statement_base::instantiation_statement_base(void) :
//		inst_base(NULL), type_base(NULL),
		indices(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement_base::instantiation_statement_base(
		const index_collection_item_ptr_type& i) :
		indices(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement_base::~instantiation_statement_base() {
	STACKTRACE_DTOR("~instantiation_statement_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement_base::dump(ostream& o) const {
//	STACKTRACE("instantation_statement::dump()");
	const count_ptr<const fundamental_type_reference>
		type_base(get_type_ref());
	NEVER_NULL(type_base);
	type_base->dump(o) << " ";
	never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	if(inst_base) {
		o << inst_base->get_name();
	} else {
		o << "<unknown>";
	}
	if (indices)
		indices->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_statement_base::get_name(void) const {
	const never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	NEVER_NULL(inst_base);
	return inst_base->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference-counted pointer to instantiating indices, 
		which contains expressions, and may be null.  
 */
index_collection_item_ptr_type
instantiation_statement_base::get_indices(void) const {
	return indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This sets the range list passed by reference according to 
	the resolved values at unroll-time.  
	\pre Only call this if indices is NOT null.  
 */
good_bool
instantiation_statement_base::resolve_instantiation_range(
		const_range_list& r, const unroll_context& c) const {
if (indices) {
	INVARIANT(r.empty());	// not already constructed
	const good_bool ret(indices->unroll_resolve(r, c));
	if (!ret.good) {
		// ranges is passed and returned by reference
		// fail
		cerr << "ERROR: unable to resolve indices of " <<
			get_inst_base()->get_qualified_name() <<
			" for instantiation: ";
		indices->dump(cerr) << endl;
		THROW_EXIT;	// temporary non-error-handling
	}
	return ret;
} else {
	INVARIANT(r.empty());
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instantiation_statement_base::dimensions(void) const {
	if (indices)
		return indices->dimensions();
	else return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary, should be pure virtual in the end.
 */
void
instantiation_statement_base::unroll(unroll_context& c) const {
	cerr << "instantiation_statement_base::unroll(): Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"instantiation_statement_base::collect_transient_info_base()");
	if (indices)
		indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, indices);
}

//=============================================================================
// class param_instantiation_statement method definitions

#if 0
/**
	Private empty constructor.
 */
param_instantiation_statement::param_instantiation_statement() :
		parent_type() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::param_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		parent_type(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
param_instantiation_statement::~param_instantiation_statement() {
}
#endif

//=============================================================================
// explicit template class instantiations

template class instantiation_statement<pbool_tag>;
template class instantiation_statement<pint_tag>;
template class instantiation_statement<datatype_tag>;
template class instantiation_statement<channel_tag>;
template class instantiation_statement<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("inst_stmt")

// responsibly, anally, undefining macros local to this module
#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __OBJECT_ART_OBJECT_INST_STMT_CC__

