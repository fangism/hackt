/**
	\file "Object/unroll/instantiation_statement.cc"
	Method definitions for instantiation statement classes.  
	This file was moved from "Object/art_object_inst_stmt.cc".
 	$Id: instantiation_statement.cc,v 1.3.2.1 2005/08/15 21:12:25 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_INSTANTIATION_STATEMENT_CC__
#define	__OBJECT_UNROLL_INSTANTIATION_STATEMENT_CC__

// for debugging only, before inclusion of any header files
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <algorithm>

#include "Object/type/fundamental_type_reference.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/param_value_collection.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/expr/param_expr_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/persistent_type_hash.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/class_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/value_collection.h"

// required by use of canonical_type
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"

#include "Object/unroll/param_instantiation_statement.h"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/unroll/channel_instantiation_statement.h"
#include "Object/unroll/process_instantiation_statement.h"
#include "Object/unroll/instantiation_statement.tcc"
#include "Object/unroll/param_instantiation_statement.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"


//=============================================================================
// local specializations
// Alternatively, explicit specialization here guarantees that the
// static initialization occurs in the correct order in this module.  
namespace util {
using ART::entity::pbool_tag;
using ART::entity::pint_tag;

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
namespace ART {
namespace entity {
USING_STACKTRACE
using util::persistent_traits;
#include "util/using_ostream.h"
#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// class instantiation_statement_base method definitions

#if 0
/**
	Private empty constructor.
	Consider a separate inline-header with definitions for these.  
 */
instantiation_statement_base::instantiation_statement_base() :
		instance_management_base(), 
		indices(NULL), relaxed_args(NULL) {
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
	type_base->dump(o);
	// is this ok: reference to automatic object?
	const const_relaxed_args_type& ra(get_relaxed_actuals());
	if (ra) {
		ra->dump(o << '<') << '>';
	}
	o << " ";
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
good_bool
instantiation_statement_base::unroll(unroll_context& c) const {
	cerr << "instantiation_statement_base::unroll(): Fang, finish me!" << endl;
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Method for the instantiation pass of meta-unrolling.  
	Default (un-implemented) is no-op.  
 */
good_bool
instantiation_statement_base::unroll_meta_instantiate(unroll_context& c) const {
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Method for the connection pass of meta-unrolling.  
	Default (un-implemented) is no-op.  
 */
good_bool
instantiation_statement_base::unroll_meta_connect(unroll_context& c) const {
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"instantiation_statement_base::collect_transient_info_base()");
	if (indices)
		indices->collect_transient_info(m);
#if 0
	if (relaxed_args)
		relaxed_args->collect_transient_info(m);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, indices);
//	m.write_pointer(o, relaxed_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, indices);
//	m.read_pointer(i, relaxed_args);
}

//=============================================================================
// class param_instantiation_statement_base method definitions

#if 0
/**
	Private empty constructor.
 */
param_instantiation_statement_base::param_instantiation_statement_base() :
		parent_type() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement_base::param_instantiation_statement_base(
		const index_collection_item_ptr_type& i) :
		parent_type(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
param_instantiation_statement_base::~param_instantiation_statement_base() {
}
#endif

//=============================================================================
// explicit template class instantiations

#if 0
template class instantiation_statement<pbool_tag>;
template class instantiation_statement<pint_tag>;
#else
template class param_instantiation_statement<pbool_tag>;
template class param_instantiation_statement<pint_tag>;
#endif
template class instantiation_statement<datatype_tag>;
template class instantiation_statement<channel_tag>;
template class instantiation_statement<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

DEFAULT_STATIC_TRACE_END

// responsibly, anally, undefining macros local to this module
#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __OBJECT_UNROLL_INSTANTIATION_STATEMENT_CC__

