// "art_object_instance.h"

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_base.h"
#include "count_ptr.h"
#include "art_macros.h"

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
	class context;
}
using namespace parser;

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
	using namespace std;
	using namespace fang;		// for experimental pointer classes

//=============================================================================
// class instantiation_base declared in "art_object_base.h"

//=============================================================================
/**
	Process instantiation.  
 */
class process_instantiation : public instantiation_base {
protected:
	/**
		The type of process being instantiated.  
		Need to chance to excl_const_ptr... everywhere.  
		Or some_ptr, beause built in type_references are static...  
		Can only use never_ptr if SOMEONE else owns it, e.g. cache.  
	 */
	count_const_ptr<process_type_reference>		type;

	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

private:
	process_instantiation();
public:
	process_instantiation(const scopespace& o, 
		count_const_ptr<process_type_reference> pt,
		const string& n, 
		index_collection_item_ptr_type d);
	~process_instantiation();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	// why is this a never_ptr?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class process_instantiation

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instantiation : public instantiation_base {
protected:
	count_const_ptr<data_type_reference>	type;	///< the actual type
private:
	datatype_instantiation();
public:
	datatype_instantiation(const scopespace& o, 
		count_const_ptr<data_type_reference> t,
		const string& n, 
		index_collection_item_ptr_type d);
	~datatype_instantiation();

	ostream& what(ostream& o) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
	// why is this never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class datatype_instantiation

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instantiation : public instantiation_base {
protected:
	count_const_ptr<channel_type_reference>	type;
private:
	channel_instantiation();
public:
	channel_instantiation(const scopespace& o, 
		count_const_ptr<channel_type_reference> ct,
		const string& n, 
		index_collection_item_ptr_type d);
	~channel_instantiation();

	ostream& what(ostream& o) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class channel_instantiation

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
 */
class param_instantiation : public instantiation_base {
protected:
	/**
		OBSOLETE after sub-typing, and hard-wiring.  
		Type refers directly to a definition in this case, 
		because parameters are never templatable.  
	never_const_ptr<param_type_reference>	type;
	**/

protected:
	param_instantiation();
public:
	param_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d);
virtual	~param_instantiation();

virtual	ostream& what(ostream& o) const = 0;
	ostream& dump(ostream& o) const;

virtual	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
	// why is this never?
virtual	count_ptr<instance_reference_base>
		make_instance_reference(void) const = 0;
	/** should just assert fail, forbid reference to param members */
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const;

	/** appropriate for the context of a template parameter formal */
virtual	count_const_ptr<param_expr> default_value(void) const = 0;

	bool is_template_formal(void) const;

/**
	A parameter is considered "usable" if it is either initialized
	to another expression, or it is a template formal.  

	Think we may need two flavors of initialization query, 
	because of conservativeness and imprecision:
	"may_be_initialized" and "definitely_initialized"

	TECHNICALITY: what about conditional assignments to a variable?
	Will need assignment expression stack to resolve statically...
	\return true if initialized to an expression.  
	\sa initialize
 */
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;

virtual	bool assign_default_value(count_const_ptr<param_expr> p) = 0;

// used by definition_base::certify_template_arguments
virtual	bool type_check_actual_param_expr(const param_expr& pe) const = 0;

/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool is_static_constant(void) const;

#if 0
NOTE: these functions should only be applicable to param_instance_references.  
/**
	Whether or not this parameter is itself a loop index,
	or if indexed, its indices depend on some loop index.
	This may be applicable to instantiation_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
};	// end class param_instantiation

//-----------------------------------------------------------------------------
/**
	Hard-wired to pbool_type, defined in "art_built_ins.h".  
 */
class pbool_instantiation : public param_instantiation {
protected:
	/**
		Expression or value with which parameter is initialized. 
		Recall that parameters are static -- written once only.  
		Not to be used by the hash_string.  
		In the formals context of a template signature, 
		ival is to be interpreted as a default value, in the 
		case where one is not supplied.  
		Or should this be never deleted? cache-owned expressions?
		Screw the cache.  
		Only applicable for simple instances.  
		Collectives won't be checked until unroll time.  
	 */
	count_const_ptr<pbool_expr>		ival;
private:
	pbool_instantiation();
public:
	pbool_instantiation(const scopespace& o, const string& n);
	pbool_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d);
	pbool_instantiation(const scopespace& o, const string& n, 
		count_const_ptr<pbool_expr> i);
	pbool_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d, 
		count_const_ptr<pbool_expr> i);

	ostream& what(ostream& o) const;

	// PROBLEM: built-in? needs to be consistent
	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const;
	// why never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;

	bool initialize(count_const_ptr<pbool_expr> e);
	bool assign_default_value(count_const_ptr<param_expr> p);
	count_const_ptr<param_expr> default_value(void) const;
	count_const_ptr<pbool_expr> initial_value(void) const;

	bool type_check_actual_param_expr(const param_expr& pe) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class pbool_instantiation

//-----------------------------------------------------------------------------
/**
	Hard-wired to pint_type, defined in "art_built_ins.h".  
 */
class pint_instantiation : public param_instantiation {
protected:
	/**
		Expression or value with which parameter is initialized. 
		Recall that parameters are static -- written once only.  
		Not to be used by the hash_string.  
		In the formals context of a template signature, 
		ival is to be interpreted as a default value, in the 
		case where one is not supplied.  
		Or should this be never deleted? cache-owned expressions?
		Screw the cache.  
		Only applicable for simple instances.  
		Collectives won't be checked until unroll time.  
	 */
	count_const_ptr<pint_expr>		ival;
private:
	pint_instantiation();
public:
	pint_instantiation(const scopespace& o, const string& n);
	pint_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d);
	pint_instantiation(const scopespace& o, const string& n, 
		count_const_ptr<pint_expr> i);
	pint_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d, 
		count_const_ptr<pint_expr> i);

	ostream& what(ostream& o) const;

	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const;
	// why never?
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;

	bool initialize(count_const_ptr<pint_expr> e);
	bool assign_default_value(count_const_ptr<param_expr> p);
	count_const_ptr<param_expr> default_value(void) const;
	count_const_ptr<pint_expr> initial_value(void) const;

	bool type_check_actual_param_expr(const param_expr& pe) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class pint_instantiation

//=============================================================================
//=============================================================================

/**
	This node retains the information for an instantiation statement.  
	This is what will be unrolled.  
	No parent, is a globally sequential item.  
 */
class instantiation_statement : public object, public instance_management_base {
protected:
	/**
		A reference to an instantiation_base, where the collection
		will be unrolled.  
		Needs to be modifiable for unrolling.  
		Should this point to an unrolled instance?
		No, it will be looked up.  
	 */
	never_ptr<instantiation_base>		inst_base;

	index_collection_item_ptr_type		indices;
private:
	instantiation_statement();
public:
	instantiation_statement(never_ptr<instantiation_base> b, 
		const index_collection_item_ptr_type& i);
	~instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
/***
	case: A top-level instantiation is called.
	case: won't happen?  instantiation inside definition called
***/
	// unroll (context?)
public:
	ART_OBJECT_IO_METHODS
};	// end class instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

