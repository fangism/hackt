/**
	\file "art_built_ins.cc"
	Definitions and instantiations for built-ins of the ART language.  
	Includes static globals.  
 	$Id: art_built_ins.cc,v 1.12 2005/01/12 03:19:36 fang Exp $
 */

#include <iostream>		// debug only

#include "memory/pointer_classes.h"
#include "art_built_ins.h"
#include "art_object_type_ref.h"
#include "art_object_instance_param.h"
#include "art_object_inst_stmt.h"
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
#if 0
const built_in_datatype_def
int_def = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace),
	"int", 
	excl_ptr<param_instance_collection>(
		new pint_scalar(
/***
			// Really the formal should "belong" to the definition, 
			// (&int_def) however, the built-in definitions are not
			// scopespaces, so we will make the parent
			// back-reference just NULL.  
			// This is safe, assuming that the parent
			// back-reference for such a built-in type is never 
			// used.  
			never_ptr<const scopespace>(&int_def),
***/
			int_def, 		// need valid reference.
			"width",
			count_ptr<const pint_const>(new pint_const(32)))));
#else

// not really necessary, but this may make things cleaner in future
// this eliminates need for special-case built-in definition constructor
const built_in_datatype_def
int_def = built_in_datatype_def(
	never_ptr<const name_space>(&built_in_namespace), "int");

#if 0
// ass context::add_template_formal for how/why this is done
// here, we lack a context, so we have to fake it...
// see also note for built_in_datatype_def::add_template_formal

static excl_ptr<instantiation_statement>
int_def_width_inst = 
	fundamental_type_reference::make_instantiation_statement(
		pint_type_ptr, index_collection_item_ptr_type(NULL));

// copy-share dynamic cast
static const never_ptr<param_instance_collection>
int_def_width_inst_coll(int_def_width_inst.is_a<param_instance_collection>());

const bool
int_def_width_status = 
	int_def_width_inst_coll->assign_default_value(
		count_ptr<const pint_const>(new pint_const(32)));

// assert(int_def_width_status

static excl_ptr<const instance_management_base>
int_def_width_inst_base =
	int_def_width_inst.as_a_xfer<const instance_management_base>();

static const int
__int_def_width__ =
	(const_cast<sequential_scope*>(
		IS_A(const sequential_scope*, &int_def))
			->append_instance_management(
				int_def_width_inst_base), 0);

#else
// this causes memory bug for reason described above
/***
	BUG: passing excl_ptr where never_ptr<instantiation_statement>
	is expected -- will result in reference to deallocated memory!
	BUG: passing temporary excl_ptr will result in premature deallocation!
***/

static const count_ptr<const pint_const>
int_def_width_default(new pint_const(32));

// will transfer ownership to definition
static excl_ptr<instance_collection_base>
int_def_width(new pint_scalar(int_def, "width", int_def_width_default));

#if 0
static ostream&
__cerr__(int_def_width->dump(std::cerr) << std::endl);
#endif

static const never_ptr<const instance_collection_base>
__int_def_width__ =
const_cast<built_in_datatype_def&>(int_def).add_template_formal(int_def_width);

static int check = (assert(__int_def_width__), assert(int_def_width), 0);

#if 0
static ostream&
__cerr__(int_def.dump(std::cerr) << std::endl);
#endif

#endif	// memory bug fix

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

