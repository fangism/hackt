// "art_object.h"

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

#include <iosfwd>
#include <string>
#include <deque>

#include "art_macros.h"

#include "qmap.h"
#include "hash_qmap.h"
#include "hashlist.h"
#include "ptrs.h"
#include "multidimensional_sparse_set.h"

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

class scopespace;
class name_space;
class loop_scope;
class conditional_scope;

class definition_base;
class channel_definition;
class datatype_definition;
class process_definition;
class built_in_datatype_def;
class built_in_param_def;

class fundamental_type_reference;
class collective_type_reference;
class data_type_reference;
class channel_type_reference;
class process_type_reference;
class param_type_reference;		// redundant

class instantiation_base;
class channel_instantiation;
class datatype_instantiation;
class process_instantiation;
class param_instantiation;

class instance_reference_base;
class single_instance_reference;
class datatype_instance_reference;
class channel_instance_reference;
class process_instance_reference;
class param_instance_reference;

// from "art_object_expr.h"
class param_expr;

typedef	never_const_ptr<param_expr>			param_expr_ptr_type;
typedef	pair<param_expr_ptr_type, param_expr_ptr_type>	param_range_type;

typedef	pair<int, int>					static_range_type;

/**
	Array dimensions are specified as a list of parameter expressions.
	Range expressions also allowed.
	Expressions may contain other literals referring to other parameters,
	and need not necessarily be constants.
	Non-constants will be checked at instantiation time.
	Elements should be param_expr (owned pointers).
 */
typedef	list<param_range_type>				array_index_list;

/**
	Array (range) dimensions resolved to static constants.  
	\sa make_static_array_index_list
 */
typedef	list<static_range_type>				static_array_index_list;


/**
	The container type for template parameters.  
	Temporarily allows any entity::object, however, 
	should definitely not contain subclasses
	of scopespace; intended for instantiations of constant
	parameters, (and when things get fancy) other types, 
	(even fancier) other template arguments.  
	These parameter expressions are not owned!  
 */
typedef	list<param_expr_ptr_type>			template_param_list;


//=============================================================================
// general non-member functions

/**
	Attempts to resolve a list of range expressions into constants.  
	\param a the unresolved list of range expressions.
	\return newly constructed list of constant index ranges, 
		if possible, else NULL.  
 */
excl_const_ptr<static_array_index_list>
	make_static_array_index_list(const array_index_list& a);

//=============================================================================
/// the root object type
class object {
public:
virtual ~object() { }

/**
	What eeeeez it man?
 */
virtual	ostream& what(ostream& o) const = 0;

/**
	Detailed hierarchical dump of contents.  
	Later: add detail-level argument.  
 */
virtual	ostream& dump(ostream& o) const = 0;

/**
	Use this method to automatically dereference object handles.  
 */
virtual const object& self(void) const { return *this; }
};	// end class object

//=============================================================================
/**
	Need handle class of object to add one level of indirection.  
	When handle is destroyed, the dereferenced member is not de-allocated. 
	This is particularly useful for aggregate types whose elements
	are non-const and owned by the container, but you want to 
	add a const (non-owned) member.  
	Using this will ensure that when the handle is deleted the 
	referenced object is left intact.  
	Used in class name_space to manage open namespaces.  
 */
class object_handle : public object {
protected:
	/**
		The actual object referenced.  
		Never delete.  
		Is it ever NULL?
		Should be reference instead of pointer?
	 */
	const object&			obj;
public:
	/**
		Constructor to wrap around an object reference.  
		\param o may not be a handle.  
			Someone else should have responsibility for deleting.  
	 */
	object_handle(never_const_ptr<object> o) : object(), obj(*o)
		{ assert(!o.is_a<object_handle>()); }

	/**
		No intention to de-allocate reference object.  
	 */
	~object_handle() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	const object& self(void) const { return obj; }
};	// end class object_handle

// after note: if we need object_handle (non-const) implement later...

//=============================================================================
/**
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
class instance_collection_stack_item {
protected:
	never_const_ptr<instantiation_base>	owner;
public:
	instance_collection_stack_item() { }
virtual	~instance_collection_stack_item() { }

/**
	Query whether or not there is definite overlap with
	this item.  
	\return Conservatively returns false for dynamic instances.  
virtual	bool definite_overlap(void) const = 0;
**/

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
 */
class static_collection_addition : public instance_collection_stack_item {
protected:
	typedef	instance_collection_stack_item		parent;
protected:
	excl_const_ptr<static_array_index_list>		indices;
public:
	static_collection_addition(excl_const_ptr<static_array_index_list>& i)
		: parent(), indices(i) { }
	// no copy-constructor
	~static_collection_addition() { }

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
	list<param_range_type>				indices;
public:
	dynamic_collection_addition() : parent(), indices() { }
	~dynamic_collection_addition() { }

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
//	never_const_ptr<loop_scope>		body;

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
//	never_const_ptr<conditional_scope>	body;

};	// end class conditional_collection_addition

//=============================================================================
#if 0
/**
	THIS IS PREMATURE.
	Need to establish the idea of a collection stack that
	represents the state of a stack at various program points.  
	Instance reference will refer to positions in the collection stack.  

	Collection of indices for a sparse multidimensional array.
	Indices however may depend on parameters and other unroll-time 
	values, thus we need to keep around: 
	1) statically bound instance indices, such as constants and 
	resolved expressions, 2) dynamically determined indices
	such as those dependent on formal template parameters, 
	and loop and conditional indices dependent thereupon.  
	Do we need to keep around precise conditions, or can we just 
	FLAG it as dynamic for the purposes of static type-checking.  
	And don't bother tracking here.  
	This is not an object... yet.
 */
class sparse_index_collection {
public:
	typedef	base_multidimensional_sparse_set<int>
						static_instance_set_type;
#if 1
	typedef never_const_ptr<param_expr>	value_ptr_type;
	typedef	pair<value_ptr_type, value_ptr_type>	range_type;
	typedef	list<range_type>		range_list_type;
	typedef	list<range_list_type>		range_list_set_type;
#endif

protected:
	count_ptr<static_instance_set_type>	static_instances;
		// of excl_ptr?
	range_list_set_type			dynamic_instances;

public:
	sparse_index_collection(const int dim);
	~sparse_index_collection();


};	// end class sparse_index_collection
#endif

//=============================================================================
/**
	Generic base class for scoped entities such as namespaces, 
	process definitions, and other definition bodies.  
	To restrict the type of things that can be added to a specific
	scope-class, derive from this class privately.  
	e.g. process definitions cannot contain other namespaces or other 
	process definitions, and namespaces do not contain formals or
	naked language bodies.  
 */
class scopespace : public object {
protected:	// typedefs -- keep these here for re-use

	/**
		Aliased namespaces, which are not owned, 
		cannot be modified.  
	 */
	typedef	qmap<string, never_const_ptr<name_space> >	alias_map_type;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are read-only, and thus not
		owned by this namespace.  
	 */
	typedef list<never_const_ptr<name_space> >	namespace_list;

	/**
		This set contains the list of identifiers for this namespace
		that have been mapped to some class: either another namespace, 
		a process/data-type/channel definitions/instantiation.  
		The language currently forbids reuse of identifiers within
		this namespace, so one cannot say namespace x {}; followed
		by int x, even though the syntax tree is sufficient to 
		disambiguate between the uses of x, based on context.  
		We do, however, allow the overshadowing of names from
		other namespaces, say that of the parent, or of an 
		imported or sub-space.  
		This type is needed to rapid lookup of identifiers in a body
		that can refer to a symbol declared in the template formals, 
		port parameters, or local definitions and instantiations.  
		The stored value is a generic polymorphic object pointer 
		whose type is deduced in the grammar.  
		EVERY addition to this namespace must register
		through this hash_map.  
		Again, these pointers are not owned.  
		These (redundantly) stored copies of pointers are read-only.  
		To get the modifiable pointers, you'll need to look them up 
		in the corresponding type-specific map.  
	 */
	typedef	hash_qmap<string, some_ptr<object> >	used_id_map_type;

	// new idea: use used_id_map as cache for type references and 
	// parameters expressions.  

protected:	// members
	// should really only contain instantiations? no definitions?
	// what should a generic scopespace contain?
	/**
		Reference to the parent namespace, if applicable.  
		The only symbol table that can be a parent is another 
		namespace.  Only the GLOBAL symbol table should have a 
		NULL parent.  
		The parent is a const pointer because no child namespace
		should be able to modify its parent namespaces.  
		However children (and their members) are allowed to 
		make other const references to outside members through the
		parent chain.  
	 */
	never_const_ptr<scopespace>		parent;

	/**
		The name of this namespace, also used as the map key for 
		the table that contains this namespace.  
	 */
	string			key;

	/**
		Before mapping a new symbol to a symbolic object, 
		it must not already be mapped to an existing object.  
		All additions to the current scope must register
		through this map.  
		Would be *nice* if code was structured to do this 
		automatically, expecially in sub-classes of scopespace.  
		Making this private forces access through the [] operators.  
	 */
	used_id_map_type	used_id_map;

public:
	scopespace(const string& n, never_const_ptr<scopespace> p);
virtual	~scopespace();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual	string get_qualified_name(void) const = 0;

virtual	never_const_ptr<object>	lookup_object_here(const string& id) const;
virtual	never_ptr<object>	lookup_object_here_with_modify(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const qualified_id_slice& id) const;

virtual	never_const_ptr<scopespace>	lookup_namespace(const qualified_id_slice& id) const;

virtual	never_const_ptr<instantiation_base>
			add_instance(excl_ptr<instantiation_base> i);

};	// end class scopespace

//=============================================================================
/**
	Namespace container class.  
 */
class name_space : public scopespace {
	// list of pointers to other opened namespaces (and their aliases)
	// table of type definitions (user-defined data types)
	// table of process definitions
	// table of real instantiations (outside of definitions)

	// determine for which elements the order matters:

	// does order of type definitions matter?
	// for now yes, because there is no support for prototype declarations
	// yet, which enforces an ordering on the declarations.  
	// or this may just be a temporary restriction on the ordering of defs

	// order of instantiations? shouldn't matter.

protected:
	// ummm... should this have been removed? scopespace already has one
	const never_const_ptr<name_space>	parent;	// override parent

	/**
		The set of namespaces which are open to search within
		this namespace's scope.  The imported namespaces may be 
		re-opened and renamed.  When this namespace closes, however, 
		the list of open spaces and aliases will be purged.  
		You'll have to add them each time you re-open this
		namespace if you want them, otherwise, it is a convenient
		way to reset the namespace alias list.  
		Remember: not owned.  
		CONSIDER: possible cycles in searching?
			hash pointers of searched, to prevent re-visiting?
	 */
	namespace_list		open_spaces;

	/**
		This is the set of open namespaces that are aliased
		locally under a different sub-namespaces, 
		a pseudo subspace.  These, too, are cleared upon leaving
		this namespace.  These will always be searched just like
		any other sub-namespace.  The difference is that 
		these namespace pointers are not owned, and thus are
		not deleted at destruction time.  
	 */
	alias_map_type		open_aliases;

	// later instroduce single symbol imports?
	// i.e. using A::my_type;

public:
explicit name_space(const string& n);
	name_space(const string& n, never_const_ptr<name_space>);
	~name_space();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;

string	get_qualified_name(void) const;
never_const_ptr<name_space>	get_global_namespace(void) const;

// update these return types later
never_ptr<name_space>	add_open_namespace(const string& n);
never_const_ptr<name_space>	leave_namespace(void);	// or close_namespace
never_const_ptr<name_space>	add_using_directive(const qualified_id& n);
never_const_ptr<name_space>	add_using_alias(const qualified_id& n, const string& a);

// do we really need to specialize adding definitions by class?
// to be used ONLY by the global namespace
never_ptr<definition_base>	add_definition(excl_ptr<definition_base> db);

// convert me to pointer-class:
datatype_definition*	add_type_alias(const qualified_id& t, const string& a);

// for generic concrete types, built-in and user-defined
never_const_ptr<fundamental_type_reference>
		add_type_reference(excl_ptr<fundamental_type_reference> tb);

// returns type if unique match found, else NULL
never_const_ptr<scopespace>	lookup_namespace(const qualified_id_slice& id) const;

// type-specific counterparts

never_const_ptr<process_definition>
			probe_process_definition(const string& s) const;
never_ptr<process_definition>
			add_proc_declaration(const token_identifier& pname);
never_ptr<process_definition>	add_proc_definition(const token_identifier& pname);

// some private utility functions (may become public later)
// add versions for querying for types, instantiations, etc...
private:
never_const_ptr<name_space>
		query_namespace_match(const qualified_id_slice& id) const;
never_const_ptr<name_space>
		query_subnamespace_match(const qualified_id_slice& id) const;

void	query_import_namespace_match(namespace_list& m, const qualified_id& id) const;

// these will not be recursive, but iteratively invoked by
// add_blah_inst/def();

// the following are not used... yet
void	find_namespace_ending_with(namespace_list& m, 
		const qualified_id& id) const;
void	find_namespace_starting_with(namespace_list& m, 
		const qualified_id& id) const;

// void	inherit_built_in_types(void);		// OBSOLETE

// will we need generalized versions of queries that return object*
// if we don't know a priori what an identifier's class is?
// single symbol table or separate?

};	// end class name_space

//=============================================================================
/**
	Scope of a loop body.  
	Notes: Instances in loop bodies will register sparse collections
	in the parent definition scope, but track indices in this scope.  
	Q: how should this be kept in the enclosing scope?
		in hash_map or in some ordered list?
	Q: should contents (instantiations) be kept ordered?
	Q: derive from dynamic_scope?
 */
class loop_scope : public scopespace {
protected:
	// should have modifiable pointer to parent scope?
	// induction variable
	// range expression
public:
	/** what about name of scope? */
	loop_scope(const string& n, never_const_ptr<scopespace>);
		// more args...
	~loop_scope();

};	// end class loop_scope

//=============================================================================
/**
	Scope of a conditional body.  
	Should this be some list?
 */
class conditional_scope : public scopespace {
protected:
	// condition expression
public:
	conditional_scope(const string& n, never_const_ptr<scopespace>);
		// more args...
	~conditional_scope();

};	// end class conditional_scope

//=============================================================================
/**
	Base class for definition objects.  
	Consider deriving from name_space to re-use the 
	name-resolving functionality.  
	All definitions are potentially templatable.  
 */
class definition_base : public scopespace {
public:
	/**
		Table of template formals.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Remember: template formals are accessible to the rest 
		of the body and to the port formals as well.  
		For now, the contained type is datatype_instantiation
			which is generalized to include the paramater types
			pbool and pint, not to be confused with the data 
			types bool and int.  
		In the far future, prepare to extend template formals to 
			include abstract types of processes, channels and 
			data types in template argument list.  
			*shudder*
			It'd be nice to be able to swap instance arguments
			that preserve specified interfaces...
	 */
	typedef	hashlist<string, never_const_ptr<param_instantiation> >
					template_formals_set;
protected:
	// inherited:
	// const scopespace*		parent;
	// string			key;
	// used_id_map_type		used_id_map;
	/**
		List and set of template formals.  
		Pointer or object?
		Depends on method of adding...
		Going to add later? can't be const then...
		Ownership?
		convert later...
	 */
	template_formals_set*		template_formals;
public:
	definition_base(const string& n,
		never_const_ptr<name_space> p, 
		template_formals_set* tf = NULL);
virtual	~definition_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary

virtual	bool check_null_template_argument(void) const;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const = 0;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const = 0;
virtual	string get_name(void) const;
virtual	string get_qualified_name(void) const;

never_const_ptr<definition_base> resolve_canonical(void) const;

/**
	f should be const and owned -- pointer type conflict...  
 */
virtual	const instantiation_base* add_template_formal(instantiation_base* f);
};	// end class definition_base

//=============================================================================
/**
	Base class for instantiation objects.  Recall that instantiations
	may appear both inside and outside definitions.  Instantiations
	inside definitions will build as part of the definitions, 			whereas those outside definitions will register as actual 
	instantiations in the object file.  
	Instantiations may be single or collective.  
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
//	excl_ptr<sparse_index_collection>	index_collection;
	deque<count_const_ptr<instance_collection_stack_item> >
					index_collection;
//	excl_ptr<array_index_list>	array_dimensions;

	/**
		Dimensions, >= 0, limit is 4.  
		Once set, is fixed.  
	 */
	size_t	depth;

public:
	// o should be reference, not pointer
	instantiation_base(const scopespace& o, const string& n, 
		const array_index_list* d = NULL);
virtual	~instantiation_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary
	string get_name(void) const { return key; }
virtual	string get_qualified_name(void) const;
virtual	string hash_string(void) const { return key; }

virtual	never_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const = 0;

// new concept
//	void set_array_dimensions(excl_ptr<array_index_list> d);

	bool array_dimension_match(const instantiation_base& i) const;

};	// end class instantiation_base

//=============================================================================
class type_reference_base : public object {
protected:
public:
	type_reference_base() : object() { }
virtual	~type_reference_base() { }

};	// end class type_reference_base

//=============================================================================
/**
	This class is a reference to a type (datatype, process, channel), 
	and contains optional template argments.  
	Instantiations should contain wrapper-references of this type, 
	and not direct references to type definitions.  
	This level of indirection...
	Sub-classes thereof should contain const pointers to 
	definitions of the specific classes.  
 */
class fundamental_type_reference : public type_reference_base {
// members
protected:
	/**
		Optional set of template parameters with which a
		type is instantiated.  
		Types must match that of template signature.  
		Hmmm... maybe need a concrete_type class...
		distinguish type_reference from type_instance.  
		This is owned, and thus must be deleted.  
		Const?
	 */
	excl_ptr<template_param_list>		template_params;

public:
	fundamental_type_reference(void);
explicit fundamental_type_reference(excl_ptr<template_param_list> pl);
virtual	~fundamental_type_reference();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual never_const_ptr<definition_base> get_base_def(void) const = 0;
	string template_param_string(void) const;
	string get_qualified_name(void) const;
	string hash_string(void) const;
	never_const_ptr<fundamental_type_reference>
		set_context_type_reference(context& c) const;

	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool is_dynamically_parameter_dependent(void) const;

virtual never_const_ptr<instantiation_base>
		add_instance_to_scope(scopespace& s, 
			const token_identifier& id) const = 0;

// TO DO: type equivalence relationship

};	// end class fundamental_type_reference

//-----------------------------------------------------------------------------
/**
	Class for reference to a collection or array of fundamental types.
	Or should we allow collective_types to contain collective_types?
	Depends on whether or not gramma allows both styles of arrays:
	x[i][j] vs. x[i,j], and whether or not they are equivalent.  
 */
class collective_type_reference : public type_reference_base {
protected:
	// don't own these members
	never_const_ptr<type_reference_base>	base;
	never_const_ptr<array_index_list>		dim;
public:
	collective_type_reference(const type_reference_base& b, 
		never_const_ptr<array_index_list> d);
	~collective_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
};	// end class collective_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a data-type definition.  
	Includes optional template parameters.  
 */
class data_type_reference : public fundamental_type_reference {
protected:
//	template_param_list*		template_params;	// inherited
	never_const_ptr<datatype_definition>	base_type_def;
public:
	data_type_reference(
		never_const_ptr<datatype_definition> td);
	data_type_reference(
		never_const_ptr<datatype_definition> td, 
		excl_ptr<template_param_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~data_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	never_const_ptr<instantiation_base>
		add_instance_to_scope(scopespace& s, 
			const token_identifier& id) const;
};	// end class data_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a channel-type definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public fundamental_type_reference {
protected:
//	excl_ptr<template_param_list>	template_params;	// inherited
	never_const_ptr<channel_definition>	base_chan_def;
public:
	channel_type_reference(
		never_const_ptr<channel_definition> td);
	channel_type_reference(
		never_const_ptr<channel_definition> td, 
		excl_ptr<template_param_list> pl);
virtual	~channel_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	never_const_ptr<instantiation_base>
		add_instance_to_scope(scopespace& s, 
			const token_identifier& id) const;
};	// end class channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
protected:
//	template_param_list*		template_params;	// inherited
	never_const_ptr<process_definition>	base_proc_def;
public:
	process_type_reference(
		never_const_ptr<process_definition> td);
	process_type_reference(
		never_const_ptr<process_definition> td, 
		excl_ptr<template_param_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~process_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	never_const_ptr<instantiation_base>
		add_instance_to_scope(scopespace& s, 
			const token_identifier& id) const;
};	// end class process_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a param definition.  
	This class really does nothing, but exists merely for consistency's
	sake.  Parameters are non-templatable, thus it suffices for 
	parameter instances to refer directly to the built-in definitions, 
	but we rather than make a special-case exception, we use this class.  
	The template_params member is inherited but not used.  
 */
class param_type_reference : public fundamental_type_reference {
protected:
//	template_param_list*		template_params;	// inherited, unused
	never_const_ptr<built_in_param_def>	base_param_def;
public:
	param_type_reference(never_const_ptr<built_in_param_def> td);
virtual	~param_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	never_const_ptr<instantiation_base>
		add_instance_to_scope(scopespace& s, 
			const token_identifier& id) const;
};	// end class param_type_reference

//=============================================================================
/**
	Base class for anything that *refers* to an instance, 
	or collection thereof.  
	Instance reference should be cacheable?
 */
class instance_reference_base : public object {
public:
	instance_reference_base() : object() { }
virtual	~instance_reference_base() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
virtual	string hash_string(void) const = 0;
};	// end class instance_reference_base

//=============================================================================
/**
	Reference to an array (one-level) of instances.  
	Self-reference is acceptable and intended for multidimensional
	array element references.  
 */
class collective_instance_reference : public instance_reference_base {
protected:
	// owned? no belongs to cache, even if multidimensional
	// may also be collective
	never_const_ptr<instance_reference_base>	base_array;
	never_const_ptr<param_expr>			lower_index;
	never_const_ptr<param_expr>			upper_index;
public:
	collective_instance_reference(
		never_const_ptr<instance_reference_base> b, 
		const param_expr* l = NULL, const param_expr* r = NULL);
virtual	~collective_instance_reference();

virtual	ostream& what(ostream& o) const;
virtual	ostream& dump(ostream& o) const;
virtual	string hash_string(void) const;
};	// end class collective_instance_reference

//=============================================================================
/**
	Base class for a reference to a particular instance.  
	Where a particular instance, either array or single, is 
	connected or aliased, this object refers to a single instance
	of a datatype, channel, or process.  
	To check, that the instance references was actually in the 
	dimension range of the array declared.  
	Collection, bundle?
	Sub-classes must contain a const pointer to the appropriate
	subclass of fundamental_type_reference.  
	Should these be hashed into used_id_map?
		Will there be identifier conflicts?
 */
class single_instance_reference : public instance_reference_base {
protected:
	/**
		Optional array indices (not ranges).  
		Indices may be symbolic.  
		Should be list of parameter expressions, possibly constants.  
		Expressions are owned by a separate expression cache.  
	 */
	typedef	list<never_const_ptr<param_expr> >	array_index_list;

protected:
	// consider letting collective_instance_reference take care of it...
	excl_ptr<array_index_list>		array_indices;

// for subclasses:
//	const instantiation_base*		inst_ref;

public:
	single_instance_reference(array_index_list* i = NULL) : 
		array_indices(i) { }
virtual	~single_instance_reference();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
virtual	string hash_string(void) const;

};	// end class single_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of datatype.  
 */
class datatype_instance_reference : public single_instance_reference {
protected:
//	array_index_list*			array_indices;	// inherited
//	excl_ptr<array_index_list>		array_indices;	// inherited
	never_const_ptr<datatype_instantiation>	data_inst_ref;

public:
	datatype_instance_reference(const datatype_instantiation& di, 
		array_index_list* i = NULL) :
		single_instance_reference(i), data_inst_ref(&di) { }
	~datatype_instance_reference() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of channel.  
 */
class channel_instance_reference : public single_instance_reference {
protected:
//	array_index_list*			array_indices;	// inherited
//	excl_ptr<array_index_list>		array_indices;	// inherited
	never_const_ptr<channel_instantiation>	channel_inst_ref;

public:
	channel_instance_reference(const channel_instantiation& ci, 
		array_index_list* i = NULL) :
		single_instance_reference(i), channel_inst_ref(&ci) { }
	~channel_instance_reference() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class channel_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of process.  
 */
class process_instance_reference : public single_instance_reference {
protected:
//	array_index_list*			array_indices;	// inherited
//	excl_ptr<array_index_list>		array_indices;	// inherited
	never_const_ptr<process_instantiation>	process_inst_ref;

public:
	process_instance_reference(const process_instantiation& pi, 
		array_index_list* i = NULL) :
		single_instance_reference(i), process_inst_ref(&pi) { }
	~process_instance_reference() { }

	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class process_instance_reference

//=============================================================================
/**
	Process definition.  Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	No other class derives from this?
 */
class process_definition : public definition_base {
public:

	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
	 */
	typedef hashlist<string, never_const_ptr<instantiation_base> >
							port_formals_set;

	// List of language bodies, separate or merged?

protected:
//	string			key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	bool			def;		///< flag: declared or defined
	port_formals_set	port_formals;
	// list language bodies
	
public:
	process_definition(
		never_const_ptr<name_space> o, 
		const string& s, const bool d, 
		template_formals_set* tf = NULL);
virtual	~process_definition();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;

	bool is_defined(void) const { return def; }

// just a thought...
void	add_port_formal(const instantiation_base* d, const port_formal_id& p);

};	// end class process_definition

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
		const string& n);
	~process_instantiation();

	ostream& what(ostream& o) const;
	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;
};	// end class process_instantiation

//=============================================================================
/**
	Abstract base class for types and their representations.
 */
class datatype_definition : public definition_base {
protected:
// inherited:
//	string			key;		///< name of type
public:
	datatype_definition(
		never_const_ptr<name_space> o, 
		const string& n, 
		template_formals_set* tf = NULL);
virtual	~datatype_definition();

virtual	ostream& what(ostream& o) const = 0;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
virtual	bool type_equivalent(const datatype_definition& t) const = 0;
};	// end class datatype_definition

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	May potentially build off of this as a base class for 
	specialization.  
 */
class built_in_datatype_def : public datatype_definition {
public:
	built_in_datatype_def(never_const_ptr<name_space> o, const string& n);
virtual	~built_in_datatype_def();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
virtual	bool type_equivalent(const datatype_definition& t) const;
};	// end class_built_in_datatype_def

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in parameter types, pint and pbool.  
	Nothing can really be derived from them... yet.  
	Note that there is no intermediate param_definition class, 
	because parameter types can only be built in; there are no
	user-defined parameter types, for now...
 */
class built_in_param_def : public definition_base {
protected:
//	string			key;		// inherited
public:
	built_in_param_def(never_const_ptr<name_space> p, const string& n);
virtual	~built_in_param_def();

	ostream& what(ostream& o) const;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
};	// end class built_in_param_def

//-----------------------------------------------------------------------------
// TO DO: need to distinguish parameter and data-types...
//-----------------------------------------------------------------------------
// need specialized type definitions for templates? (int<N>)
// class built_in_datatype_template_def : public built_in_datatype_def {
// perhaps contain a type definition and a template signature?
// }

//-----------------------------------------------------------------------------
// may need a template type that refers to an abstract template
// with template actuals/parameters
// or just use user_def_datatype below...

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_def_datatype : public datatype_definition {
private:
	/**
		Members will be kept as a hashlist
		because their order matters, or don't they?
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_def_datatype(never_const_ptr<name_space> o, const string& name);
	~user_def_datatype() { }

	ostream& what(ostream& o) const;
	bool type_equivalent(const datatype_definition& t) const;
};	// end class user_def_datatype

//-----------------------------------------------------------------------------
/// abstract base class for channels and their representations
class channel_definition : public definition_base {
protected:
//	string			key;		// inherited
public:
	channel_definition(never_const_ptr<name_space> o, const string& n, 
		template_formals_set* tf = NULL);
virtual	~channel_definition();

virtual	ostream& what(ostream& o) const = 0;
virtual	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
virtual	never_const_ptr<fundamental_type_reference>
		set_context_fundamental_type(context& c) const;
};	// end class channel_definition

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined channel type, which can (eventually) 
	build upon other user-defined channel types.  
 */
class user_def_chan : public channel_definition {
private:
	/**
		Members will be kept as a list for ordered checking.  
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
		Redundant? inherited?
	 */
	typedef	hashlist<string, never_const_ptr<datatype_definition> >
						temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_def_chan(const name_space* o, const string& name);
virtual	~user_def_chan() { }

virtual	ostream& what(ostream& o) const;
};	// end class user_def_chan

//-----------------------------------------------------------------------------
/// Type aliases are analogous to typedefs in C (not yet implemented)
// for renaming convenience
// going to use this mechanism to inherit built-in types into each namespace
//	to accelerate their resolution, not having to search sequentially
//	up to the global namespace.  
// add an alias into each scope's used_id_map...
// also templatable, for partial template spcifications?
class type_alias : public definition_base {
protected:
	/// pointer to the type represented by this type-id
	never_const_ptr<definition_base>	canonical;
public:
	type_alias(
		never_const_ptr<name_space> o,
		const string& n, 
		never_const_ptr<definition_base> t,
		template_formals_set* tf = NULL);
virtual	~type_alias();
	// never delete canonical (can't, it's const!)

// need not be virtual
// const definition_base*	resolve_canonical(void) const;
never_const_ptr<definition_base>	resolve_canonical(void) const;

virtual	ostream& what(ostream& o) const;
};	// end class type_alias

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instantiation : public instantiation_base {
protected:
	never_const_ptr<data_type_reference>	type;	///< the actual type
public:
	datatype_instantiation(const scopespace& o, 
		const data_type_reference& t,
		const string& n);
virtual	~datatype_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;

	bool equals_template_formal(const template_formal_decl& tf) const;
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
		const string& n);
virtual	~channel_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;
};	// end class channel_instantiation

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Need not be virtual.  
 */
class param_instantiation : public instantiation_base {
protected:
	/**
		Type refers directly to a definition in this case, 
		because parameters are never templatable.  
	 */
	never_const_ptr<param_type_reference>	type;

	/**
		Expression or value with which parameter is initialized. 
		Recall that parameters are static -- written once only.  
		Not to be used by the hash_string.  
		In the formals context of a template signature, 
		ival is to be interpreted as a default value, in the 
		case where one is not supplied.  
		Or should this be never delete? cache-owned expressions?
	 */
	excl_const_ptr<param_expr>	ival;

public:
	param_instantiation(const scopespace& o, 
		const param_type_reference& pt, const string& n, 
		const param_expr* i = NULL);
	~param_instantiation();

	ostream& what(ostream& o) const;

	never_const_ptr<fundamental_type_reference> get_type_ref(void) const;
	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const;

	void initialize(excl_const_ptr<param_expr> e);

	never_const_ptr<param_expr> default_value(void) const { return ival; }

/**
	A parameter is considered "usable" if it is either initialized
	to another expression, or it is a template formal.  
	\return true if initialized to an expression.  
	\sa initialize
 */
	bool is_initialized(void) const { return ival; }
};	// end class param_instantiation

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of parameter.  
 */
class param_instance_reference : public single_instance_reference {
protected:
//	array_index_list*			array_indices;	// inherited
	never_const_ptr<param_instantiation>	param_inst_ref;

public:
	param_instance_reference(const param_instantiation& pi, 
		array_index_list* i = NULL) : 
		single_instance_reference(i), param_inst_ref(&pi) { }
	~param_instance_reference() { }

	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class param_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_H__

