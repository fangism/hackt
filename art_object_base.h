// "art_object_base.h"

#ifndef	__ART_OBJECT_BASE_H__
#define	__ART_OBJECT_BASE_H__

#include <iosfwd>
#include <string>
#include <list>
#include <deque>

#include "art_macros.h"

#include "qmap.h"
#include "hash_qmap.h"
#include "ptrs.h"
#include "count_ptr.h"

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
//=============================================================================
// temporary switches

//=============================================================================
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
	class channel_definition_base;
	class datatype_definition_base;
	class process_definition_base;
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
	class member_instance_reference_base;
	class datatype_instance_reference;
	class channel_instance_reference;
	class process_instance_reference;
	class param_instance_reference;
//	class pint_instance_reference;		// relocated "art_object_expr"
//	class pbool_instance_reference;		// relocated "art_object_expr"

	class connection_assignment_base;
	class param_expression_assignment;
	class instance_reference_connection;
	class aliases_connection;
	class port_connection;

// declarations from "art_object_expr.h"
	class param_expr;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class pint_expr;
	class pbool_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
	class index_list;			// not ART::parser::index_list
	class const_index_list;

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

#if 0
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
// general non-member functions

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

/**
	Consider an interface to hierarchical errors embedded in the objects.  
	Mmmm... fancy.
**/
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
	List of objects.
	Will potentially want to make the following sub-typed lists:
	(some cases will have special implicit conversions)
	list<pint_range> for array declarations/instantiations,
	list<index_expr> for array indexing and dimension collapsing, 
	list<param_expr> for param_expression_assignments, 
	list<instance_reference_base> for alias_connections.  
 */
class object_list : public object, public list<count_ptr<object> > {
protected:
	typedef	list<count_ptr<object> >		parent;
public:
	object_list();
	~object_list();
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
// using parent's list interface

	// could use excl_ptr, but then would need to release to count_ptr...
	count_ptr<range_expr_list>
		make_formal_dense_range_list(void) const;
	count_ptr<range_expr_list>
		make_sparse_range_list(void) const;
	excl_ptr<index_list>
		make_index_list(void) const;
	excl_ptr<dynamic_param_expr_list>
		make_param_expr_list(void) const;
	excl_ptr<param_expression_assignment>
		make_param_assignment(void);
	excl_const_ptr<aliases_connection>
		make_alias_connection(void) const;
#if 0
	// needs base process type
	excl_const_ptr<instance_reference_connection>
		make_port_connection(void) const;
#endif
};	// end class object_list

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
class scopespace : virtual public object {
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

	/**
		Ordered list of connections and assignments.  
	 */
	typedef	list<excl_const_ptr<connection_assignment_base> >
						connect_assign_list_type;

protected:	// members
	// should really only contain instantiations? no definitions?
	// what should a generic scopespace contain?
#if 0
	// these members need to be implemented in children classes
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
#endif

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

	/**
		This list is for maintaining items and actions
		whose order must be preserved.  
		Assignments and connection statements are 
		maintained in this list.  
		Conditional and loop scopes are kept in program order
		in this list.
	 */
	connect_assign_list_type	connect_assign_list;

public:
	scopespace();
//	scopespace(const string& n, never_const_ptr<scopespace> p);
virtual	~scopespace();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;

virtual	const string& get_key(void) const = 0;
virtual	string get_qualified_name(void) const = 0;
virtual never_const_ptr<scopespace> get_parent(void) const = 0;

virtual	never_const_ptr<object>	lookup_object_here(const string& id) const;
virtual	never_ptr<object>	lookup_object_here_with_modify(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const string& id) const;
virtual	never_const_ptr<object>	lookup_object(const qualified_id_slice& id) const;

virtual	never_const_ptr<scopespace>	lookup_namespace(const qualified_id_slice& id) const;

virtual	never_const_ptr<instantiation_base>
			add_instance(excl_ptr<instantiation_base> i);
	bool add_definition_alias(never_const_ptr<definition_base> d, 
		const string& a);

	void add_connection_to_scope(
		excl_const_ptr<connection_assignment_base> c);

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
	const string				key;
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

	// later introduce single symbol imports?
	// i.e. using A::my_type;

public:
explicit name_space(const string& n);
	name_space(const string& n, never_const_ptr<name_space>);
	~name_space();

	const string& get_key(void) const;
	never_const_ptr<scopespace> get_parent(void) const;

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
// to be used ONLY by the global namespace (???)
never_ptr<definition_base>	add_definition(excl_ptr<definition_base> db);

// convert me to pointer-class:
#if 0
datatype_definition*	add_type_alias(const qualified_id& t, const string& a);
#endif

// for generic concrete types, built-in and user-defined
never_const_ptr<fundamental_type_reference>
		add_type_reference(excl_ptr<fundamental_type_reference> tb);

// returns type if unique match found, else NULL
never_const_ptr<scopespace>	lookup_namespace(const qualified_id_slice& id) const;

// type-specific counterparts, obsolete

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
	Q: should not really be a scope, doesn't contain
		it's own used_id_map, should use parent's.  
		All lookups and registrations go to enclosing
		definition or namespace scope.  
	S: may contain instantiations and connections... references?
	Q: However interface should be like scopespace?
	Q: Should be able to make similar queries?
	S: May have to further refine classification of scopes...
		named_scope, true_scope, psuedo_scope...
 */
class loop_scope {
protected:
	// should have modifiable pointer to parent scope?
	// induction variable
	// range expression
public:
	/** what about name of scope? none. */
	loop_scope(const string& n, never_const_ptr<scopespace>);
		// more args...
	~loop_scope();

};	// end class loop_scope

//=============================================================================
/**
	Scope of a conditional body.  
	Should this be some list?
 */
class conditional_scope {
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
class definition_base : virtual public object {
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
		May need hashqlist, for const-queryable hash structure!!!
	**/
	typedef	never_const_ptr<param_instantiation>
					template_formals_value_type;
	// double-maintenance...
	typedef	hash_qmap<string, template_formals_value_type>
					template_formals_map_type;
	typedef	list<template_formals_value_type>
					template_formals_list_type;
	/** map from param_instantiation to actual value passed */
	typedef	hash_qmap<string, count_const_ptr<param_expr> >
					template_actuals_map_type;

protected:
//	const string			key;
//	const never_const_ptr<name_space>	parent;

protected:
	/** subset of used_id_map, must be coherent with list */
	template_formals_map_type	template_formals_map;
	/** subset of used_id_map, must be coherent with map */
	template_formals_list_type	template_formals_list;

	/**
		Whether or not this definition is complete or only declared.  
		As soon as a definition is opened, mark it as defined
		to allow self-recursive template definitions.  
	 */
	bool				defined;
public:
//	definition_base(const string& n, never_const_ptr<name_space> p);
//	definition_base(const string& n);
	definition_base();
virtual	~definition_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;	// temporary
	ostream& dump_template_formals(ostream& o) const;

virtual	const string& get_key(void) const = 0;
virtual	never_const_ptr<scopespace> get_parent(void) const = 0;

	bool is_defined(void) const { return defined; }
	void mark_defined(void) { assert(!defined); defined = true; }

	void fill_template_actuals_map(template_actuals_map_type& am, 
		const param_expr_list& al) const;

	never_const_ptr<param_instantiation>
		lookup_template_formal(const string& id) const;
/** should be pure virtual, but let's default to NULL */
virtual	never_const_ptr<instantiation_base>
		lookup_port_formal(const string& id) const;
virtual	never_const_ptr<object>	lookup_object_here(const string& id) const;

virtual	bool check_null_template_argument(void) const;


protected:
	// Q: what if expressions are involved, can't statically resolve?
	// e.g. with arrays of parameters... and referenced indices.
	// well, they must be at least initialized (usable).  
	// need notion of formal equivalence
	// MAY be equivalent
	bool equivalent_template_formals(
		never_const_ptr<definition_base> d) const;

#if 0
OBSOLETE
public:
	/**
		sub-classes shouldn't have to re-implement this,
		no longer virtual
	 */
	never_const_ptr<definition_base>
		set_context_definition(context& c) const;
#endif

protected:
	bool certify_template_arguments(
		never_ptr<dynamic_param_expr_list> ta) const;
public:
	excl_ptr<dynamic_param_expr_list>
		make_default_template_arguments(void) const;

public:
// proposing to replace set_context_fundamental_type with the following:
virtual count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(
			excl_ptr<dynamic_param_expr_list> ta) const = 0;
	// overloaded for no template argument, for convenience, 
	// but must check that everything has default arguments!
	count_const_ptr<fundamental_type_reference>
		make_fundamental_type_reference(void) const {
			return make_fundamental_type_reference(
				make_default_template_arguments());
		}
// why virtual? special cases for built-in types?

/**
	Create an empty, unresolved typedef, based on the 
	invoker's type.  
 */
virtual	excl_ptr<definition_base>
		make_typedef(never_const_ptr<scopespace> s, 
			const token_identifier& id) const = 0;

// need not be virtual?
virtual	string get_name(void) const;
// need not be virtual?
virtual	string get_qualified_name(void) const;

/** definition signature comparison, true if equal */
virtual	bool require_signature_match(
		never_const_ptr<definition_base> d) const
		{ return false; }	// temporary, should be pure

/**
	f should be const and owned -- pointer type conflict...  
	virtual so that types without templates can assert NULL.  
 */
virtual	never_const_ptr<instantiation_base>
		add_template_formal(excl_ptr<instantiation_base> f);

/**
	Really, only some definitions should have ports...
 */
virtual	never_const_ptr<instantiation_base>
		add_port_formal(excl_ptr<instantiation_base> f);
};	// end class definition_base

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
public:
	/** map from param_instantiation to actual value passed */
	typedef	definition_base::template_actuals_map_type
						template_actuals_map_type;
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
	excl_const_ptr<param_expr_list>		template_params;

public:
	fundamental_type_reference();
explicit fundamental_type_reference(excl_const_ptr<param_expr_list> pl);
virtual	~fundamental_type_reference();

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;
virtual never_const_ptr<definition_base> get_base_def(void) const = 0;
	string template_param_string(void) const;
	string get_qualified_name(void) const;
	string hash_string(void) const;

	excl_ptr<param_expr_list>
		get_copy_template_params(void) const;

	// limits the extend to which it can be statically type-checked
	// i.e. whether parameter is resolved to a scope's formal
	bool is_dynamically_parameter_dependent(void) const;

	// later add dimensions and indices?

excl_const_ptr<fundamental_type_reference>
	resolve_canonical_type(void) const;

/** wrapper for the next private function */
static	excl_ptr<instantiation_base>
		make_instantiation(
			count_const_ptr<fundamental_type_reference> t, 
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d);

private:
/**
	Since context's current_fundamental type is now a count_ptr, 
	we can't invoke it and copy the 'this' pointer.  
	't' is used to invoke.  
 */
virtual	excl_ptr<instantiation_base>
		make_instantiation_private(
			count_const_ptr<fundamental_type_reference> t, 
			never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d) const = 0;

public:
	bool may_be_equivalent(const fundamental_type_reference& t) const;
	bool must_be_equivalent(const fundamental_type_reference& t) const;

	// something for resolving typedefs
	// or return by value? statically would require copy constructor
	// wth, just allocate one...
	excl_const_ptr<fundamental_type_reference>
		make_canonical_type_reference(void) const;
};	// end class fundamental_type_reference

//=============================================================================
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

/**
	Why is this a count_ptr?  because type_references can be reused
	all over the place, so we reference count all type references.  
	Unfortunately this forces us to do the same with static 
	built-in types.  
 */
virtual	count_const_ptr<fundamental_type_reference>
		get_type_ref(void) const = 0;
	never_const_ptr<definition_base>
		get_base_def(void) const;

	never_const_ptr<scopespace> get_owner(void) const { return owner; }
	size_t dimensions(void) const { return depth; }
	instantiation_state collection_state_end(void) const;
	instantiation_state current_collection_state(void) const;
	const_range_list detect_static_overlap(
		index_collection_item_ptr_type r) const;
	const_range_list add_index_range(
		index_collection_item_ptr_type r);
	const_range_list merge_index_ranges(
		never_const_ptr<instantiation_base> i);

private:
	bool formal_size_equivalent(
		never_const_ptr<instantiation_base> b) const;
public:
	bool is_template_formal(void) const;
	bool is_port_formal(void) const;
	bool template_formal_equivalent(
		never_const_ptr<instantiation_base> b) const;
	bool port_formal_equivalent(
		never_const_ptr<instantiation_base> b) const;

protected:
	bool check_expression_dimensions(const param_expr& pr) const;

public:
/**
	always pushes onto context's object_stack, 
	currently always returns NULL, useless
 */
virtual	never_const_ptr<instance_reference_base>
		make_instance_reference(context& c) const = 0;
virtual	never_const_ptr<member_instance_reference_base>
		make_member_instance_reference(
			count_const_ptr<simple_instance_reference> b, 
			context& c) const = 0;
};	// end class instantiation_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_BASE_H__

