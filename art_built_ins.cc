// "art_built_ins.cc"

/**
	Definitions and instantiations for built-ins of the ART language.  
	Includes static globals.  
**/

#include "ptrs.h"
#include "art_built_ins.h"
#include "art_object_instance.h"
#include "art_object_expr.h"

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
built_in_namespace("BUILT-IN", never_const_ptr<name_space>(NULL));

//-----------------------------------------------------------------------------
/** built-in parameter pbool type definition initialization */
const built_in_param_def
pbool_def = built_in_param_def(
	never_const_ptr<name_space>(&built_in_namespace), "pbool", pbool_type);

/** built-in parameter pint type definition initialization */
const built_in_param_def
pint_def = built_in_param_def(
	never_const_ptr<name_space>(&built_in_namespace), "pint", pint_type);

/** built-in parameter pbool type reference */
const param_type_reference
pbool_type = param_type_reference(
	never_const_ptr<built_in_param_def>(&pbool_def));

/** built-in parameter pint type reference */
const param_type_reference
pint_type = param_type_reference(
	never_const_ptr<built_in_param_def>(&pint_def));

//-----------------------------------------------------------------------------
/** built-in data bool type definition initialization */
const built_in_datatype_def
bool_def = built_in_datatype_def(
	never_const_ptr<name_space>(&built_in_namespace), "bool");

/** built-in data int type definition initialization */
const built_in_datatype_def
int_def = built_in_datatype_def(
	never_const_ptr<name_space>(&built_in_namespace),
	"int", 
	excl_ptr<param_instantiation>(
		new pint_instantiation(
			built_in_namespace, // pint_type,
			"width", new pint_const(32))));

/** built-in data int type reference */
const data_type_reference
int_type = data_type_reference(
	never_const_ptr<built_in_datatype_def>(&int_def));

//=============================================================================
}	// end namespace entity
}	// end namespace ART

