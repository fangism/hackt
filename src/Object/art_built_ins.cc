/**
	\file "art_built_ins.cc"
	Definitions and instantiations for built-ins of the ART language.  
	Includes static globals.  
 	$Id: art_built_ins.cc,v 1.17 2005/03/11 08:47:24 fang Exp $
 */

#ifndef	__ART_BUILT_INS_CC__
#define	__ART_BUILT_INS_CC__

#define	DEBUG_ART_BUILT_INS			0

#include "memory/pointer_classes.h"
#include "memory/list_vector_pool.h"
#include "art_built_ins.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance_param.h"
#include "art_object_expr_const.h"
#include "static_trace.h"
#include "art_object_value_collection.h"
#include "art_object_classification_details.h"

STATIC_TRACE_BEGIN("built-ins");

#if DEBUG_ART_BUILT_INS
	#define	ENABLE_STACKTRACE			1
	#include "stacktrace.h"

USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

// global static initializations...
namespace ART {
namespace entity {
//=============================================================================
// needed to ensure that pint_const pool is ready to dish out pints
// this holds onto a reference count that will be guaranteed to be
// discarded AFTER subsequent static objects are deallocated in this module
// becaused of reverse-order static destruction.
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(pint_const)
#if 0
// re-enable this when it switches back to pooled...
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(pint_scalar)
#endif
// this early because int_def contains a pint_scalar, 
// and built_in_namespace contains the int_def.

//=============================================================================
/**
	The built-in namespace, not global.
	We don't actually add anything to it.  
	In fact this is rather useless.  :P
	Constructing built in definitions doesn't add them to it...
 */
name_space
built_in_namespace("BUILT-IN", never_ptr<const name_space>(NULL));

//-----------------------------------------------------------------------------
/** built-in parameter pbool type definition initialization */
const built_in_param_def
pbool_def = built_in_param_def(
	never_ptr<const name_space>(&built_in_namespace), "pbool");

/** built-in parameter pint type definition initialization */
const built_in_param_def
pint_def = built_in_param_def(
	never_ptr<const name_space>(&built_in_namespace), "pint");

// will need to pool param_type_reference?

/** built-in parameter pbool type reference */
const count_ptr<const param_type_reference> pbool_type_ptr =
	count_ptr<const param_type_reference>(new param_type_reference(
		never_ptr<const built_in_param_def>(&pbool_def)));

/** built-in parameter pint type reference */
const count_ptr<const param_type_reference> pint_type_ptr =
	count_ptr<const param_type_reference>(new param_type_reference(
	never_ptr<const built_in_param_def>(&pint_def)));

//-----------------------------------------------------------------------------

/** built-in data bool type definition initialization */
const built_in_datatype_def
bool_def = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace), "bool");

/** built-in data int type definition initialization */
// not really necessary, but this may make things cleaner in future
// this eliminates need for special-case built-in definition constructor
const built_in_datatype_def
int_def = built_in_datatype_def(
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
static excl_ptr<pint_scalar>
int_def_width(
//	new pint_scalar(int_def, "width", int_def_width_default) // was this
	new pint_scalar(int_def, "width")
);

static const good_bool
__good_int_width(int_def_width->assign_default_value(int_def_width_default));

// INVARIANT(__good_int_width.good);

static excl_ptr<instance_collection_base>
int_def_width_base(int_def_width);

static const never_ptr<const instance_collection_base>
int_def_width_ref =
const_cast<built_in_datatype_def&>(int_def)
	.add_template_formal(int_def_width_base);

#if 0
// can't hurt to keep this initialization check...
static const int check = (assert(int_def_width_ref), assert(int_def_width), 0);
// ODDBALL ALERT: on gcc-3.3.3 SuSE linux, this statement generates a reference
//	to a __PRETTY_FUNCTION__ symbol, which is the name of a reserved
//	built-in compiler macro.  
// This problem doesn't occur on OSX/FreeBSD, AFAICT.
#endif

#if 0
static ostream&
__cerr__(int_def.dump(std::cerr) << std::endl);
#endif

#if 0
/** built-in data int type reference */
const data_type_reference
bool_type = data_type_reference(
	never_ptr<const built_in_datatype_def>(&bool_def));
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef	DEBUG_ART_BUILT_INS

STATIC_TRACE_END("built-ins");

#endif	// __ART_BUILT_INS_CC__

