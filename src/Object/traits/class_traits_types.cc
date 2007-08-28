/**
	\file "Object/traits/class_traits_types.cc"
	Definitions and instantiations for built-ins of the HAC language.  
	Includes static globals.  
	This file used to be "Object/art_built_ins.cc".
 	$Id: class_traits_types.cc,v 1.20 2007/08/28 04:54:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_CLASS_TRAITS_TYPES_CC__
#define	__HAC_OBJECT_TRAITS_CLASS_TRAITS_TYPES_CC__

#define	DEBUG_TRAITS_CLASS_TRAITS_TYPES			0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.tcc"
#include "util/attributes.h"
#include "util/memory/chunk_map_pool.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/def/param_definition.h"
#include "Object/common/namespace.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/param_type_reference.h"
#include "Object/inst/value_placeholder.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/param_instantiation_statement.h"
#include "Object/expr/meta_range_list.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_scalar.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/string_traits.h"

#if DEBUG_TRAITS_CLASS_TRAITS_TYPES
#define	ENABLE_STACKTRACE		DEBUG_TRAITS_CLASS_TRAITS_TYPES
#include "util/stacktrace.h"
REQUIRES_STACKTRACE_STATIC_INIT
#endif

// global static initializations...
namespace HAC {
namespace entity {
//=============================================================================
// needed to ensure that pint_const pool is ready to dish out pints
// this holds onto a reference count that will be guaranteed to be
// discarded AFTER subsequent static objects are deallocated in this module
// becaused of reverse-order static destruction.
// REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(pint_const)
REQUIRES_CHUNK_MAP_POOL_STATIC_INIT(pint_const)
// this early because int_def contains a pint_scalar, 
// and built_in_namespace contains the int_def.

//=============================================================================
/**
	TODO: Does this actually need external linkage?
	The built-in namespace, not global.
	We don't actually add anything to it.  
	In fact this is rather useless.  :P
	Constructing built in definitions doesn't add them to it...
 */
static
name_space
built_in_namespace("BUILT-IN", never_ptr<const name_space>(NULL));

//-----------------------------------------------------------------------------
/** built-in parameter pbool type definition initialization */
const built_in_param_def
pbool_traits::built_in_definition = built_in_param_def(
	never_ptr<const name_space>(&built_in_namespace), "pbool");

/** built-in parameter pint type definition initialization */
const built_in_param_def
pint_traits::built_in_definition = built_in_param_def(
	never_ptr<const name_space>(&built_in_namespace), "pint");

/** built-in parameter preal type definition initialization */
const built_in_param_def
preal_traits::built_in_definition = built_in_param_def(
	never_ptr<const name_space>(&built_in_namespace), "preal");

// will need to pool param_type_reference?

/** built-in parameter pbool type reference */
const pbool_traits::type_ref_ptr_type
pbool_traits::built_in_type_ptr =
	count_ptr<const param_type_reference>(new param_type_reference(
		never_ptr<const built_in_param_def>(&built_in_definition)));

/** built-in parameter pint type reference */
const pint_traits::type_ref_ptr_type
pint_traits::built_in_type_ptr =
	count_ptr<const param_type_reference>(new param_type_reference(
		never_ptr<const built_in_param_def>(&built_in_definition)));

/** built-in parameter preal type reference */
const preal_traits::type_ref_ptr_type
preal_traits::built_in_type_ptr =
	count_ptr<const param_type_reference>(new param_type_reference(
		never_ptr<const built_in_param_def>(&built_in_definition)));

//-----------------------------------------------------------------------------

/** built-in data bool type definition initialization */
const built_in_datatype_def
bool_traits::built_in_definition = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace), "bool");

/** built-in data int type definition initialization */
// not really necessary, but this may make things cleaner in future
// this eliminates need for special-case built-in definition constructor
const built_in_datatype_def
int_traits::built_in_definition = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace), "int");


static const count_ptr<const pint_const>
int_def_width_default(new pint_const(32));

#if 0
// EXAMPLE:
// this is the correct way to dynamically allocate during static initialization
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(pbool_const);
static const excl_ptr<const pbool_const>
dummy_bool(new pbool_const(true));
#endif

/***
	Really the formal should "belong" to the definition, 
	(&int_def) however, the built-in definitions are not
	scopespaces, so we will make the parent back-reference just NULL.  
	This is safe, assuming that the parent back-reference for 
	such a built-in type is never used.  
***/

// will transfer ownership to definition
static excl_ptr<pint_value_placeholder>
int_def_width(new pint_value_placeholder(
	int_traits::built_in_definition, "width", 0));

static const good_bool
__good_int_width
__ATTRIBUTE_UNUSED_CTOR__((int_def_width->assign_default_value(int_def_width_default)));

// INVARIANT(__good_int_width.good);

/**
	Since we unroll all template formals now, we need an initial
	instantiation statement per formal parameter.  
 */
static const count_ptr<pint_instantiation_statement>
width_inst = fundamental_type_reference::make_instantiation_statement(
		int_def_width->get_unresolved_type_ref(),
		index_collection_item_ptr_type(NULL)
	).is_a<pint_instantiation_statement>();

static const size_t int_inst_base_receipt =
	(width_inst->attach_collection(int_def_width), 0);

static const size_t width_receipt = 
	(int_def_width->attach_initial_instantiation_statement(width_inst), 0);

// need to fake adding the template formal and instantiating it
// or creating a footprint?
static excl_ptr<param_value_placeholder>
int_def_width_base(int_def_width);

static const never_ptr<const param_value_placeholder>
int_def_width_ref
__ATTRIBUTE_UNUSED_CTOR__((const_cast<built_in_datatype_def&>(
	int_traits::built_in_definition).add_template_formal(int_def_width_base)));

#if 0
// can't hurt to keep this initialization check...
static const int check = (assert(int_def_width_ref), assert(int_def_width), 0);
// ODDBALL ALERT: on gcc-3.3.3 SuSE linux, this statement generates a reference
//	to a __PRETTY_FUNCTION__ symbol, which is the name of a reserved
//	built-in compiler macro.  
// This problem doesn't occur on OSX/FreeBSD, AFAICT.
#endif

const bool_traits::type_ref_ptr_type
bool_traits::built_in_type_ptr(new data_type_reference(
	never_ptr<const built_in_datatype_def>(&built_in_definition)));

const bool_traits::type_ref_ptr_type&
bool_traits::nonmeta_data_type_ptr(built_in_type_ptr);

// be careful once type references are memory-pooled!
// the following function call calls a bunch of allocators (new)
const count_ptr<const data_type_reference>
int_traits::int32_type_ptr(data_type_reference::make_quick_int_type_ref(32));

const count_ptr<const data_type_reference>
int_traits::magic_int_type_ptr(data_type_reference::make_quick_int_type_ref(0));

const count_ptr<const data_type_reference>&
int_traits::nonmeta_data_type_ptr(magic_int_type_ptr);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const built_in_datatype_def
string_traits::built_in_definition = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace), "string");

const string_traits::type_ref_ptr_type
string_traits::built_in_type_ptr(new data_type_reference(
	never_ptr<const built_in_datatype_def>(&built_in_definition)));

const string_traits::type_ref_ptr_type&
string_traits::nonmeta_data_type_ptr(built_in_type_ptr);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	DEBUG_TRAITS_CLASS_TRAITS_TYPES

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_TRAITS_CLASS_TRAITS_TYPES_CC__

