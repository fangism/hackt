/**
	\file "art_built_ins.cc"
	Definitions and instantiations for built-ins of the ART language.  
	Includes static globals.  
 	$Id: art_built_ins.cc,v 1.15.10.2 2005/01/22 22:34:47 fang Exp $
 */

#ifndef	__ART_BUILT_INS_CC__
#define	__ART_BUILT_INS_CC__

// #include <iostream>		// debug only

#include "memory/pointer_classes.h"
#include "memory/list_vector_pool.h"
#include "art_built_ins.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_instance_param.h"
#include "art_object_expr_const.h"

// global static initializations...
namespace ART {
namespace entity {
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

#if 0
OBSOLETE
// needed to ensure that pint_const pool is ready to dish out pints
REQUIRES_LIST_VECTOR_POOL_STATIC_INIT(pint_const);
// this holds onto a reference count that will be guaranteed to be
// discarded AFTER subsequent static objects are deallocated in this module
// becaused of reverse-order static destruction.
#endif

static const count_ptr<const pint_const>
int_def_width_default(new pint_const(32));

#if 0
// OBSERVATION:
// enabling this causes pbool_const pool to not be destroyed upon
// program termination.  
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
static excl_ptr<instance_collection_base>
int_def_width(new pint_scalar(int_def, "width", int_def_width_default));

static const never_ptr<const instance_collection_base>
int_def_width_ref =
const_cast<built_in_datatype_def&>(int_def).add_template_formal(int_def_width);

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

#endif	// __ART_BUILT_INS_CC__

