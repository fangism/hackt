// "art_object_instance.h"

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include "art_object_base.h"
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
	 */
	never_const_ptr<process_type_reference>		type;

	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

public:
	process_instantiation(const scopespace& o, 
		const process_type_reference& pt,
		const string& n, 
		index_collection_item_ptr_type d);
	~process_instantiation();

	ostream& what(ostream& o) const;
//	ostream& dump(ostream& o) const;
	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;
};	// end class process_instantiation

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instantiation : public instantiation_base {
protected:
	never_const_ptr<data_type_reference>	type;	///< the actual type
public:
	datatype_instantiation(const scopespace& o, 
		const data_type_reference& t,
		const string& n, 
		index_collection_item_ptr_type d);
virtual	~datatype_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;
};	// end class datatype_instantiation

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instantiation : public instantiation_base {
protected:
	never_const_ptr<channel_type_reference>	type;
public:
	channel_instantiation(const scopespace& o, 
		const channel_type_reference& ct,
		const string& n, 
		index_collection_item_ptr_type d);
virtual	~channel_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;
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

public:
	param_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d);
virtual	~param_instantiation();

virtual	ostream& what(ostream& o) const = 0;
// virtual	ostream& dump(ostream& o) const;

virtual	never_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const = 0;

#if 0
// replacing sub-classes with type-specific initializations
virtual	bool initialize(count_const_ptr<param_expr> e) = 0;
#endif

	/** appropriate for the context of a template parameter formal */
virtual	count_const_ptr<param_expr> default_value(void) const = 0;

	bool is_template_formal(void) const;

#if 0
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
	bool is_initialized(void) const;
#else
	bool may_be_initialized(void) const;
	bool must_be_initialized(void) const;
//	bool is_initialized(void) const { return must_be_initialized(); }
#endif

/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool is_static_constant(void) const;

#if 0
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
public:
	pbool_instantiation(const scopespace& o, const string& n, 
		const pbool_expr* i = NULL);
	pbool_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d, 
		const pbool_expr* i = NULL);

	ostream& what(ostream& o) const;

	never_const_ptr<fundamental_type_reference>
		get_type_ref(void) const;
	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;

	bool initialize(count_const_ptr<pbool_expr> e);
	count_const_ptr<param_expr> default_value(void) const;
	count_const_ptr<pbool_expr> initial_value(void) const;

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
public:
	pint_instantiation(const scopespace& o, const string& n, 
		const pint_expr* i = NULL);
	pint_instantiation(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d, 
		const pint_expr* i = NULL);

	ostream& what(ostream& o) const;

	never_const_ptr<fundamental_type_reference>
		get_type_ref(void) const;
	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;

	bool initialize(count_const_ptr<pint_expr> e);
	count_const_ptr<param_expr> default_value(void) const;
	count_const_ptr<pint_expr> initial_value(void) const;
};	// end class pint_instantiation

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

