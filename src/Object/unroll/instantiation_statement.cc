/**
	\file "Object/unroll/instantiation_statement.cc"
	Method definitions for instantiation statement classes.  
	This file was moved from "Object/art_object_inst_stmt.cc".
 	$Id: instantiation_statement.cc,v 1.23 2011/04/02 01:46:13 fang Exp $
 */

// for debugging only, before inclusion of any header files
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <algorithm>

#include "Object/type/fundamental_type_reference.h"
#include "Object/inst/param_value_collection.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/expr/param_expr_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/persistent_type_hash.h"
#include "Object/unroll/unroll_context.h"
#include "Object/traits/class_traits.h"
#include "Object/traits/instance_traits.h"
#include "Object/inst/instance_placeholder.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/param_value_placeholder.h"

// required by use of canonical_type
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"

#include "Object/unroll/param_instantiation_statement.h"
#include "Object/unroll/datatype_instantiation_statement.h"
#include "Object/unroll/channel_instantiation_statement.h"
#include "Object/unroll/process_instantiation_statement.h"
#include "Object/unroll/instantiation_statement.tcc"
// #include "Object/traits/proc_traits.h"
#include "Object/unroll/template_type_completion.tcc"
#include "Object/unroll/instance_attribute.tcc"
#include "Object/unroll/direction_declaration.tcc"
#include "Object/inst/internal_aliases_policy.tcc"

#include "common/ICE.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"


//=============================================================================
// local specializations
// Alternatively, explicit specialization here guarantees that the
// static initialization occurs in the correct order in this module.  
namespace util {
using namespace HAC::entity;

SPECIALIZE_UTIL_WHAT(data_instantiation_statement,
	"data_instantiation_statement")
SPECIALIZE_UTIL_WHAT(pint_instantiation_statement,
	"pint_instantiation_statement")
SPECIALIZE_UTIL_WHAT(pbool_instantiation_statement,
	"pbool_instantiation_statement")
SPECIALIZE_UTIL_WHAT(preal_instantiation_statement,
	"preal_instantiation_statement")
SPECIALIZE_UTIL_WHAT(pstring_instantiation_statement,
	"pstring_instantiation_statement")
SPECIALIZE_UTIL_WHAT(process_instantiation_statement,
	"process_instantiation_statement")
SPECIALIZE_UTIL_WHAT(channel_instantiation_statement,
	"channel_instantiation_statement")
SPECIALIZE_UTIL_WHAT(process_template_type_completion,
	"process_template_type_completion")
SPECIALIZE_UTIL_WHAT(process_instance_attribute,
	"process_instance_attribute")
SPECIALIZE_UTIL_WHAT(channel_instance_attribute,
	"channel_instance_attribute")
SPECIALIZE_UTIL_WHAT(bool_instance_attribute,
	"bool_instance_attribute")
SPECIALIZE_UTIL_WHAT(int_instance_attribute,
	"int_instance_attribute")
SPECIALIZE_UTIL_WHAT(enum_instance_attribute,
	"enum_instance_attribute")
SPECIALIZE_UTIL_WHAT(process_direction_declaration,
	"process_direction_declaration")
SPECIALIZE_UTIL_WHAT(channel_direction_declaration,
	"channel_direction_declaration")
SPECIALIZE_UTIL_WHAT(bool_direction_declaration,
	"bool_direction_declaration")
SPECIALIZE_UTIL_WHAT(int_direction_declaration,
	"int_direction_declaration")
SPECIALIZE_UTIL_WHAT(enum_direction_declaration,
	"enum_direction_declaration")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pbool_instantiation_statement, 
		PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pint_instantiation_statement, 
		PINT_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	preal_instantiation_statement, 
		PREAL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pstring_instantiation_statement, 
		PSTRING_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	process_instantiation_statement, 
		PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	channel_instantiation_statement, 
		CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	data_instantiation_statement, 
		DATA_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	process_template_type_completion, 
		PROCESS_TEMPLATE_TYPE_COMPLETION_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	bool_instance_attribute, 
		DBOOL_INSTANCE_ATTRIBUTE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	int_instance_attribute, 
		DINT_INSTANCE_ATTRIBUTE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	enum_instance_attribute, 
		ENUM_INSTANCE_ATTRIBUTE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	channel_instance_attribute, 
		CHANNEL_INSTANCE_ATTRIBUTE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	process_instance_attribute, 
		PROCESS_INSTANCE_ATTRIBUTE_TYPE_KEY, 0)

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	bool_direction_declaration, 
		DBOOL_DIRECTION_DECLARATION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	int_direction_declaration, 
		DINT_DIRECTION_DECLARATION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	enum_direction_declaration, 
		ENUM_DIRECTION_DECLARATION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	channel_direction_declaration, 
		CHANNEL_DIRECTION_DECLARATION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	process_direction_declaration, 
		PROCESS_DIRECTION_DECLARATION_TYPE_KEY, 0)

}	// end namespace util


//=============================================================================
namespace HAC {
namespace entity {
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
/**
	TODO: auto_indent.
 */
ostream&
instantiation_statement_base::dump(ostream& o,
		const expr_dump_context& dc) const {
//	STACKTRACE("instantation_statement::dump()");
	const count_ptr<const fundamental_type_reference>
		type_base(get_type_ref());
	NEVER_NULL(type_base);
	type_base->dump(o);
	// is this ok: reference to automatic object?
	o << " ";
	const never_ptr<const instance_placeholder_base>
		inst_base(get_inst_base());
	if(inst_base) {
		o << inst_base->get_name();
	} else {
		o << "<unknown>";
	}
	if (indices)
		indices->dump(o, dc);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_statement_base::get_name(void) const {
	const never_ptr<const instance_placeholder_base>
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
	const good_bool ret(indices->unroll_resolve_rvalues(r, c));
	if (!ret.good) {
		// ranges is passed and returned by reference
		// fail
		cerr << "ERROR: unable to resolve indices of " <<
			get_inst_base()->get_qualified_name() <<
			" for instantiation: ";
		indices->dump(cerr, expr_dump_context::default_value) << endl;
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
instantiation_statement_base::unroll(const unroll_context& c) const {
	ICE_NEVER_CALL(cerr);
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
// explicit template class instantiations

template class instantiation_statement<pbool_tag>;
template class instantiation_statement<pint_tag>;
template class instantiation_statement<preal_tag>;
template class instantiation_statement<pstring_tag>;
template class instantiation_statement<datatype_tag>;
template class instantiation_statement<channel_tag>;
template class instantiation_statement<process_tag>;
template class template_type_completion<process_tag>;
template class instance_attribute<bool_tag>;
template class instance_attribute<int_tag>;
template class instance_attribute<enum_tag>;
template class instance_attribute<channel_tag>;
template class instance_attribute<process_tag>;
template class direction_declaration<bool_tag>;
template class direction_declaration<int_tag>;
template class direction_declaration<enum_tag>;
template class direction_declaration<channel_tag>;
template class direction_declaration<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

// responsibly, anally, undefining macros local to this module
#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

