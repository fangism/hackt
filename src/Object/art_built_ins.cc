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
	never_const_ptr<name_space>(&built_in_namespace), "pbool");

/** built-in parameter pint type definition initialization */
const built_in_param_def
pint_def = built_in_param_def(
	never_const_ptr<name_space>(&built_in_namespace), "pint");

#if 0
/** built-in parameter pbool type reference */
const param_type_reference
pbool_type = param_type_reference(
	never_const_ptr<built_in_param_def>(&pbool_def));

/** built-in parameter pint type reference */
const param_type_reference
pint_type = param_type_reference(
	never_const_ptr<built_in_param_def>(&pint_def));
#endif

/** built-in parameter pbool type reference */
const count_const_ptr<param_type_reference> pbool_type_ptr =
	count_const_ptr<param_type_reference>(new param_type_reference(
		never_const_ptr<built_in_param_def>(&pbool_def)));

/** built-in parameter pint type reference */
const count_const_ptr<param_type_reference> pint_type_ptr =
	count_const_ptr<param_type_reference>(new param_type_reference(
	never_const_ptr<built_in_param_def>(&pint_def)));

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
/***
			// Really the formal should "belong" to the definition, 
			// (&int_def) however, the built-in definitions are not
			// scopespaces, so we will make the parent
			// back-reference just NULL.  
			// This is safe, assuming that the parent
			// back-reference for such a built-in type is never 
			// used.  
			never_const_ptr<scopespace>(&int_def),
***/
			int_def, 		// need valid reference.
			"width",
			count_const_ptr<pint_expr>(new pint_const(32)))));

#if 0
/** built-in data int type reference */
const data_type_reference
bool_type = data_type_reference(
	never_const_ptr<built_in_datatype_def>(&bool_def));
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

