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
// class instance_collection_base declared in "art_object_base.h"

//=============================================================================
/**
	Process instantiation.  
 */
class process_instance_collection : public instance_collection_base {
protected:
#if 0
	/**
		PHASE THIS OUT -- in favor of keeping type information
			in the instantiation_statement list.
		Eliminate later, doesn't hurt for now.

		The type of process being instantiated.  
		Need to chance to excl_const_ptr... everywhere.  
		Or some_ptr, beause built in type_references are static...  
		Can only use never_ptr if SOMEONE else owns it, e.g. cache.  
	 */
	count_const_ptr<process_type_reference>		type;
#endif

	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

private:
	process_instance_collection();
public:
	process_instance_collection(const scopespace& o, 
#if 0
		count_const_ptr<process_type_reference> pt,
#endif
		const string& n, 
		const size_t d);
	~process_instance_collection();

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

};	// end class process_instance_collection

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instance_collection : public instance_collection_base {
protected:
#if 0
	count_const_ptr<data_type_reference>	type;	///< the actual type
#endif
private:
	datatype_instance_collection();
public:
	datatype_instance_collection(const scopespace& o, 
#if 0
		count_const_ptr<data_type_reference> t,
#endif
		const string& n, 
		const size_t d);
	~datatype_instance_collection();

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
};	// end class datatype_instance_collection

//=============================================================================
/**
	Instantiation of a channel type.  
	Final class?
 */
class channel_instance_collection : public instance_collection_base {
protected:
#if 0
	count_const_ptr<channel_type_reference>	type;
#endif
private:
	channel_instance_collection();
public:
	channel_instance_collection(const scopespace& o, 
#if 0
		count_const_ptr<channel_type_reference> ct,
#endif
		const string& n, 
		const size_t d);
	~channel_instance_collection();

	ostream& what(ostream& o) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	count_ptr<instance_reference_base>
		make_instance_reference(void) const;
	count_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b) const;
public:
	ART_OBJECT_IO_METHODS
};	// end class channel_instance_collection

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
	TO DO: derive from a interface for template_argument.  
 */
class param_instance_collection : public instance_collection_base {
protected:
	/**
		OBSOLETE after sub-typing, and hard-wiring.  
		Type refers directly to a definition in this case, 
		because parameters are never templatable.  
	never_const_ptr<param_type_reference>	type;
	**/

protected:
	param_instance_collection();
public:
	param_instance_collection(const scopespace& o, const string& n, 
		const size_t d);
virtual	~param_instance_collection();

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
	This may be applicable to instance_collection_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
};	// end class param_instance_collection

//-----------------------------------------------------------------------------
/**
	Hard-wired to pbool_type, defined in "art_built_ins.h".  
 */
class pbool_instance_collection : public param_instance_collection {
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
	pbool_instance_collection();
public:
	pbool_instance_collection(const scopespace& o, const string& n);
	pbool_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

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
};	// end class pbool_instance_collection

//-----------------------------------------------------------------------------
/**
	Hard-wired to pint_type, defined in "art_built_ins.h".  
 */
class pint_instance_collection : public param_instance_collection {
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
	pint_instance_collection();
public:
	pint_instance_collection(const scopespace& o, const string& n);
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d);
	// keep these for built-in int datatype initialization
	pint_instance_collection(const scopespace& o, const string& n, 
		count_const_ptr<pint_expr> i);
	pint_instance_collection(const scopespace& o, const string& n, 
		const size_t d, count_const_ptr<pint_expr> i);

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
};	// end class pint_instance_collection

//=============================================================================
//=============================================================================

/**
	This node retains the information for an instantiation statement.  
	This is what will be unrolled.  
	No parent, is a globally sequential item.  
 */
class instantiation_statement : public instance_management_base {
protected:
	/**
		A back-reference to an instance_collection_base, 
		where the collection will be unrolled.  
		Needs to be modifiable for unrolling.  
		Should this point to an unrolled instance?
		No, it will be looked up.  
	 */
//	never_ptr<instance_collection_base>	inst_base;

//	count_const_ptr<fundamental_type_reference>	type_base;

	index_collection_item_ptr_type		indices;

protected:
	instantiation_statement();
public:
	instantiation_statement(
//		never_ptr<instance_collection_base> b, 
//		count_const_ptr<fundamental_type_reference> t, 
		const index_collection_item_ptr_type& i);
virtual	~instantiation_statement();

	ostream& dump(ostream& o) const;

virtual	void attach_collection(never_ptr<instance_collection_base> i) = 0;
virtual	never_ptr<instance_collection_base>
		get_inst_base(void) = 0;
virtual	never_const_ptr<instance_collection_base>
		get_inst_base(void) const = 0;

	string get_name(void) const;
	size_t dimensions(void) const;
	index_collection_item_ptr_type get_indices(void) const;

	// may eventually virtualize
virtual	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;

/***
	case: A top-level instantiation is called.
	case: won't happen?  instantiation inside definition called
***/
	// unroll (context?)
};	// end class instantiation_statement

//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement : public instantiation_statement {
protected:
	param_instantiation_statement();
public:
	param_instantiation_statement(const index_collection_item_ptr_type& i);
virtual	~param_instantiation_statement();

};	// end class param_instantiation_statement

//-----------------------------------------------------------------------------
class pbool_instantiation_statement : public object, 
		public param_instantiation_statement {
public:
	typedef	pbool_instance_collection	collection_type;
	typedef	count_const_ptr<param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pbool_instantiation_statement();
public:
	pbool_instantiation_statement(const index_collection_item_ptr_type& i);
	~pbool_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_const_ptr<instance_collection_base> get_inst_base(void) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class pbool_instantiation_statement

//-----------------------------------------------------------------------------
class pint_instantiation_statement : public object, 
		public param_instantiation_statement {
public:
	typedef	pint_instance_collection	collection_type;
	typedef	count_const_ptr<param_type_reference>	type_ptr_type;
protected:
	// doesn't have a type_ref, hard-coded to built-ins
	never_ptr<collection_type>		inst_base;
private:
	pint_instantiation_statement();
public:
	pint_instantiation_statement(const index_collection_item_ptr_type& i);
	~pint_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_const_ptr<instance_collection_base> get_inst_base(void) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class pint_instantiation_statement

//=============================================================================
class process_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	process_instance_collection	collection_type;
	typedef	count_const_ptr<process_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	process_instantiation_statement();
public:
	process_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~process_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_const_ptr<instance_collection_base> get_inst_base(void) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class process_instantiation_statement

//=============================================================================
class channel_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	channel_instance_collection	collection_type;
	typedef	count_const_ptr<channel_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	channel_instantiation_statement();
public:
	channel_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~channel_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_const_ptr<instance_collection_base> get_inst_base(void) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class channel_instantiation_statement

//=============================================================================
class data_instantiation_statement : public object, 
		public instantiation_statement {
public:
	typedef	datatype_instance_collection	collection_type;
	typedef	count_const_ptr<data_type_reference>	type_ptr_type;
protected:
	const type_ptr_type			type;
	never_ptr<collection_type>		inst_base;
private:
	data_instantiation_statement();
public:
	data_instantiation_statement(const type_ptr_type& t, 
		const index_collection_item_ptr_type& i);
	~data_instantiation_statement();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void attach_collection(never_ptr<instance_collection_base> i);
	never_ptr<instance_collection_base> get_inst_base(void);
	never_const_ptr<instance_collection_base> get_inst_base(void) const;
	count_const_ptr<fundamental_type_reference> get_type_ref(void) const;

public:
	ART_OBJECT_IO_METHODS

};	// end class data_instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_H__

