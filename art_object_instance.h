// "art_object_instance.h"

#ifndef	__ART_OBJECT_INSTANCE_H__
#define	__ART_OBJECT_INSTANCE_H__

#include <iosfwd>
#include <string>
#include <list>
#include <deque>

#include "art_object_base.h"
#include "art_macros.h"

#if 0
#include "qmap.h"
#include "hash_qmap.h"
#include "ptrs.h"
#include "count_ptr.h"
#endif

/*********** note on use of data structures ***************
Lists are needed for sets that need to maintain sequence, such as
formal declarations in definitions.  Type-checking is done in order
of elements, comparing actuals against formals one-by-one.  
For some lists, however, we'd like constant time access to 
elements in the sequence by hashing indices.  Hashlist provides
this added functionality by associating a key to each element in the 
list.  

Maps...

********************** end note **************************/

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
// forward declarations
#if 0
	class scopespace;
	class name_space;
	class loop_scope;
	class conditional_scope;

	class definition_base;
	class channel_definition;
	class datatype_definition;
	class process_definition;
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

	class fundamental_type_reference;
	class collective_type_reference;
	class data_type_reference;
	class channel_type_reference;
	class process_type_reference;
	class param_type_reference;		// redundant

//	class instance_collection_stack_item;

	class instantiation_base;
	class channel_instantiation;
	class datatype_instantiation;
	class process_instantiation;
	class param_instantiation;
	class pint_instantiation;
	class pbool_instantiation;

	class instance_reference_base;
	class simple_instance_reference;
	class datatype_instance_reference;
	class channel_instance_reference;
	class process_instance_reference;
	class param_instance_reference;
//	class pint_instance_reference;		// relocated "art_object_expr"
//	class pbool_instance_reference;		// relocated "art_object_expr"

	class connection_assignment_base;
	class param_expression_assignment;
	class instance_reference_connection;

// declarations from "art_object_expr.h"
	class param_expr;
	class pint_expr;
	class pbool_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
	class index_list;			// not ART::parser::index_list

	typedef	count_const_ptr<range_expr_list>
					index_collection_item_ptr_type;
	/** we keep track of the state of instance collections at
		various program points with this container */
	typedef	deque<index_collection_item_ptr_type>
					index_collection_type;

	/** the state of an instance collection, kept track by each 
		instance reference */
	typedef	index_collection_type::const_iterator
					instantiation_state;

typedef	never_const_ptr<param_expr>			param_expr_ptr_type;

/**
	The container type for template parameters.  
	Temporarily allows any entity::object, however, 
	should definitely not contain subclasses
	of scopespace; intended for instantiations of constant
	parameters, (and when things get fancy) other types, 
	(even fancier) other template arguments.  
	These parameter expressions are not owned!  
	(because they are cached?)
 */
typedef	list<param_expr_ptr_type>			template_param_list;
#endif

//=============================================================================
#if 0
/**
	PHASING INTO art_object_expr as range_expr_list.  or not...

	temporary: derive from object to be usable on stack?

	We keep track of the precise state of collections
	(associater with an identifier in a definition scope) 
	by maintaining a stack of collection-additions each time
	more sparse instantiations are added.  
	Uses and references to an identifier-collection will 
	refer to a *position* in the instance's collection stack
	which precisely represents what is visible at the
	referencing program point.  
	This class is the abstract interface class for items
	on the instance-collection-stack.  
 */
class instance_collection_stack_item : public object {
protected:
	never_const_ptr<instantiation_base>	owner;
public:
	instance_collection_stack_item() { }
virtual	~instance_collection_stack_item() { }

// temporary
virtual ostream& what(ostream& o) const
	{ return o << "instance_collection_stack_item"; }
virtual ostream& dump(ostream& o) const
	{ return what(o); }

/** dimensionality of the indices */
virtual	size_t	dimensions(void) const = 0;
/**
	Query whether or not there is definite overlap with
	this item.  
	\return Conservatively returns false for dynamic instances.  
 */
virtual	bool static_overlap(const instance_collection_stack_item& ) const;

/**
	Resolve at unroll time, all parameters must be bound to 
	known constants.  
virtual	bool final_resolve(void) const = 0;
**/

};	// end class instance_collection_stack_item

//=============================================================================
/**
	A collection addition whose indices are statically resolved
	to constants.  
	At this point no need to expand indices into a tree yet, 
	save that for unroll-time.  
 */
class static_collection_addition : public instance_collection_stack_item {
protected:
	typedef	instance_collection_stack_item		parent;
protected:
	excl_const_ptr<const_range_list>		indices;
public:
	static_collection_addition(excl_const_ptr<const_range_list>& i);
	// no copy-constructor
	~static_collection_addition() { }

	size_t	dimensions(void) const;
	bool static_overlap(const instance_collection_stack_item& ) const;

};	// end class static_collection_addition

//=============================================================================
/**
	A collection addition whose indices contain references to 
	template formal parameters, that cannot be determined at
	compile-time.  
	Some parameter expressions may be constants, of course.  
 */
class dynamic_collection_addition : public instance_collection_stack_item {
protected:
	typedef	instance_collection_stack_item		parent;
protected:
	excl_const_ptr<dynamic_range_list>		indices;
public:
	dynamic_collection_addition(excl_const_ptr<dynamic_range_list>& i);
	~dynamic_collection_addition() { }

	size_t	dimensions(void) const;
};	// end class dynamic_collection_addition

//=============================================================================
/**
	Reference to a loop-body that contains instances of
	a collection.  
	May be static or dynamic.  
 */
class loop_collection_addition : public instance_collection_stack_item {
protected:
	typedef	instance_collection_stack_item		parent;
protected:
//	never_const_ptr<loop_scope>			body;

	size_t	dimensions(void) const { assert(0); return 0; }	// BARF for now
};	// end class loop_collection_addition

//=============================================================================
/**
	Reference to a conditional body that contains instances of a 
	collection.  
	May be static or dynamic.
 */
class conditional_collection_addition : public instance_collection_stack_item {
protected:
	typedef	instance_collection_stack_item		parent;
protected:
//	never_const_ptr<conditional_scope>		body;

	size_t	dimensions(void) const { assert(0); return 0; }	// BARF for now

};	// end class conditional_collection_addition
#endif

//=============================================================================
#if 0
/**
	Base class for instantiation objects.  Recall that instantiations
	may appear both inside and outside definitions.  Instantiations
	inside definitions will build as part of the definitions, 			whereas those outside definitions will register as actual 
	instantiations in the object file.  
	Instantiations may be simple or complex-aggregate.  
	Rules for collective instantiations (two types):
	1) dense...
	2) sparse...
	Sub-classes must contain const pointers to fundamental_type_reference
	sub-classes.  
 */
class instantiation_base : public object {
protected:
	/**
		Back-pointer to the namespace to which this instantiation
		belongs.  
		Can be a namespace of definition's scopespace.  
		Is NEVER null, should be reference?
		Should never be a loop or conditional namespace.  
	 */
	const never_const_ptr<scopespace>	owner;

	/**
		Name of instance.
	 */
	string				key;

	/**
		Optional array dimension sizes, which can be ranges.  

		REPLACE this with a hierarchical dimension tree!
		of expressions, mostly constants, some unresolved.  
		Multidimensional and sparse arrays.  

		Needs to be a grown stack of instances, because
		of changing collection.  
		Needs to be a deque so we can use iterators.  
	 */
	index_collection_type			index_collection;

	/**
		Dimensions, >= 0, limit is 4.  
		Once set, is fixed.  
	 */
	size_t	depth;

public:
	// o should be reference, not pointer
	instantiation_base(const scopespace& o, const string& n, 
		index_collection_item_ptr_type d);
virtual	~instantiation_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary
	string get_name(void) const { return key; }
virtual	string get_qualified_name(void) const;
virtual	string hash_string(void) const { return key; }

virtual	never_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;

	never_const_ptr<scopespace> get_owner(void) const { return owner; }
	size_t dimensions(void) const { return depth; }
	instantiation_state collection_state_end(void) const;
	instantiation_state current_collection_state(void) const;
	const_range_list detect_static_overlap(
		index_collection_item_ptr_type r) const;
	const_range_list add_index_range(index_collection_item_ptr_type r);
	const_range_list merge_index_ranges(never_const_ptr<instantiation_base> i);

/** currently always returns NULL, useless */
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const = 0;
};	// end class instantiation_base
#endif

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

