// "art_object.h"

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

#include <string>

#include "map_of_ptr.h"
#include "hash_map_of_ptr.h"
#include "hashlist.h"		// includes "list_of_ptr.h" and <hash_map>
	// for now don't need hashlist...
#include "art_macros.h"

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
//	class token_string;
//	class token_identifier;
	class id_expr;
};
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

//=============================================================================
// forward declarations
class scopespace;
class name_space;
class definition_base;
class channel_definition;
class datatype_definition;
class process_definition;
class built_in_datatype_def;
class built_in_param_def;

class type_reference_base;
class data_type_reference;
class channel_type_reference;
class process_type_reference;

class instantiation_base;
class channel_instantiation;
class datatype_instantiation;
class process_instantiation;
class param_instantiation;

class instance_reference_base;
class datatype_instance_reference;
class channel_instance_reference;
class process_instance_reference;
// class param_instance_reference;

// from "art_object_expr.h"
class param_expr;

//=============================================================================
/// the root object type
class object {
public:
// virtual ~object() { }
virtual	ostream& what(ostream& o) const = 0;
};

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
		Container owned (unordered) sub-namespaces.  
		Mapped entries are non-const because parents are allowed
		to modify (add to) sub-namespaces, e.g. through 
		namespace traversal by type-check/build routines.  
	 */
	typedef	map_of_ptr<string, name_space>		subns_map_type;
	/**
		Aliased namespaces, which are not owned, 
		cannot be modified.  
	 */
	typedef	map_of_const_ptr<string, name_space>	alias_map_type;

	/**
		Resolves identifier to actual data type.  
		Remember that public members of defined types, 
		must be accessible both in sequential order
		(for actuals type-checking) and in random order
		by member name mapping, as in x.y.z.  
		This structure owns the pointers to these definitions,
		and thus, is responsible for deleteing them.  
	 */
	typedef	map_of_ptr<string, datatype_definition>	type_def_set;
	/// list useful for query returns, const pointer?
	typedef	list_of_const_ptr<datatype_definition>	type_def_list;

	/// resolves identifier to actual data type, we own these pointers
	typedef	map_of_ptr<string, datatype_instantiation>	type_inst_set;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_definition>	proc_def_set;
	typedef	list_of_const_ptr<process_definition>	proc_def_list;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_instantiation>	proc_inst_set;

	typedef	map_of_ptr<string, built_in_param_def>	param_def_set;
	typedef	list_of_const_ptr<built_in_param_def>	param_def_list;
	typedef	map_of_ptr<string, param_instantiation>	param_inst_set;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are read-only, and thus not
		owned by this namespace.  
	 */
	typedef list_of_const_ptr<name_space>		namespace_list;

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
	typedef	hash_map_of_const_ptr<string, object>	used_id_map_type;

	/**
		A cache of type references for template-able types.  
		This set owns pointers to the type_references.  
	 */
	typedef	hashlist_of_ptr<string,type_reference_base>
							param_type_set;

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
	const scopespace*		parent;

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
	 */
	used_id_map_type	used_id_map;

	/**
		Keeps around a cache of types specified with template
		arguments.  
		Punting this, for now, make deep copy of template arguments
		for each type instance.  
	param_type_set		type_template_cache;
	**/

public:
	scopespace(const string& n, const scopespace* p);
virtual	~scopespace();

virtual	ostream& what(ostream& o) const = 0;
// const object*	what_is(const string& id);	// obsolete
// const object*&	assign_id(const string& id);	// obsolete

};	// end class scopespace

//=============================================================================
/// class for namespace
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
	const name_space*		parent;		// override parent

	/**
		A set of sub-namespaces, mapped by name.  
		Not const-pointers, because they are "owned" by 
		this namespace.
	 */
	subns_map_type		subns;

	/**
		The set of namespaces which are open to search within
		this namespace's scope.  The imported namespaces may be 
		re-opened and renamed.  When this namespace closes, however, 
		the list of open spaces and aliases will be purged.  
		You'll have to add them each time you re-open this
		namespace if you want them, otherwise, it is a convenient
		way to reset the namespace alias list.  
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
//	subns_map_type		open_aliases;

	/**
		Container of data type definitions in this scope.
		Also contains local type aliases.  
		These definitions are owned by this scope, and should
		be deleted in the destructor.  
	 */
	type_def_set		type_defs;
	/**
		Container of data type instantiations in this scope.
		These definitions are owned by this scope, and should
		be deleted in the destructor.  
	 */
	type_inst_set		type_insts;

	/**
		Parameter types, only existing in the global namespace.
	 */
	param_def_set		param_defs;

	/**
		Local parameter instantiations.  
		Should be in scopespace?
	 */
	param_inst_set		param_insts;

	/**
		Container of process definitions in this scope.
		These definitions are owned by this scope, and should
		be deleted in the destructor.  
	 */
	proc_def_set		proc_defs;
	/**
		Container of process instantiations in this scope.
		These definitions are owned by this scope, and should
		be deleted in the destructor.  
	 */
	proc_inst_set		proc_insts;

	// later instroduce single symbol imports?
	// i.e. using A::my_type;

public:
	name_space(const string& n, const name_space* p);
// explicit name_space();	// for GLOBAL?
	~name_space();

virtual	ostream& what(ostream& o) const;

string	get_qualified_name(void) const;
const name_space*	get_global_namespace(void) const;

// update these return types later
name_space*		add_open_namespace(const string& n);
const name_space*	leave_namespace(void);	// or close_namespace
const name_space*	add_using_directive(const id_expr& n);
const name_space*	add_using_alias(const id_expr& n, const string& a);

// to be used ONLY by the global namespace
built_in_datatype_def*	add_built_in_datatype_definition(
				built_in_datatype_def* d);
built_in_param_def*	add_built_in_param_definition(built_in_param_def* d);
datatype_definition*	add_type_alias(const id_expr& t, const string& a);

// returns type if unique match found, else NULL
const datatype_definition*	lookup_unqualified_datatype(
					const string& id) const;
const datatype_definition*	lookup_qualified_datatype(
					const id_expr& id) const;
const datatype_definition*	lookup_built_in_datatype(
					const token_datatype& id) const;
const built_in_param_def*	lookup_built_in_paramtype(
					const token_paramtype& id) const;

datatype_definition*	add_datatype_definition();
datatype_instantiation*	add_datatype_instantiation(
//				const data_type_reference& t,
				data_type_reference& t,	// temporary
				const string& i);
const process_definition*	probe_process(const string& s) const;
process_definition*	add_proc_declaration(const token_identifier& pname);
process_definition*	add_proc_definition(const token_identifier& pname);
process_instantiation*	add_proc_instantiation();

// some private utility functions (may become public later)
// add versions for querying for types, instantiations, etc...
private:
const name_space*	query_namespace_match(const id_expr& id) const;
const name_space*	query_subnamespace_match(const id_expr& id) const;
void	query_import_namespace_match(namespace_list& m, const id_expr& id) const;

// these will not be recursive, but iteratively invoked by
// add_blah_inst/def();
void	query_datatype_def_match(type_def_list& m, const string& tid) const;
void	query_datatype_def_match(type_def_list& m, const type_id& tid) const;
void	query_datatype_inst_match(type_def_list& m, const string& tid) const;
void	query_datatype_inst_match(type_def_list& m, const id_expr& tid) const;
void	query_proc_def_match(proc_def_list& m, const type_id& pid) const;
void	query_proc_inst_match(proc_def_list& m, const id_expr& pid) const;

// the following are not used... yet
void	find_namespace_ending_with(namespace_list& m, 
		const id_expr& id) const;
void	find_namespace_starting_with(namespace_list& m, 
		const id_expr& id) const;

// void	inherit_built_in_types(void);		// OBSOLETE

// will we need generalized versions of queries that return object*
// if we don't know a priori what an identifier's class is?
// single symbol table or separate?

};	// end class name_space

//=============================================================================
/**
	Base class for definition objects.  
	Consider deriving from name_space to re-use the 
	name-resolving functionality.  
 */
class definition_base : public scopespace {
protected:
	/**
		Back-pointer to the namespace to which this definition 
		belongs.  Should be const?  Do not delete.  
	 */
	const name_space*		parent;	// read-only override
	// inherited:
	// string			key;
	// used_id_map_type		used_id_map;
public:
	definition_base(const string& n, const name_space* p);
virtual	~definition_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	string get_name(void) const;
};	// end class definition_base

//=============================================================================
/**
	Base class for instantiation objects.  Recall that instantiations
	may appear both inside and outside definitions.  Instantiations
	inside definitions will build as part of the definitions, 			whereas those outside definitions will register as actual 
	instantiations in the object file.  
	Sub-classes must contain const pointers to type_reference_base
	sub-classes.  
 */
class instantiation_base : public object {
protected:
	/**
		The container type for array dimensions, allowing
		range expressions.  Expressions may contain
		other literals referring to other parameters, 
		and need not necessarily be constants.  
		Non-constants will be checked at instantiation time.  
		Elements should be param_expr (owned pointers).
	 */
	typedef	list_of_ptr<param_expr>		array_dim_list;
protected:
	/**
		Back-pointer to the namespace to which this definition 
		belongs.  Should be const?  Do not delete.  
	 */
	const name_space*		owner;

	/**
		Name of instance.
	 */

	string				key;
	/**
		Optional array dimension sizes, which can be ranges.  
	 */
	array_dim_list*			array_dimensions;

public:
	instantiation_base(const name_space* o, const string& n, 
		array_dim_list* d = NULL);
virtual	~instantiation_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	bool equals_port_formal(const port_formal_decl& tf) const = 0;
	void set_array_dimensions(array_dim_list* d);

	bool array_dimension_match(const instantiation_base& i) const;

};	// end class instantiation_base

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
class type_reference_base : public object {
protected:
	/**
		The container type for template parameters.  
		Temporarily allows any entity::object, however, 
		should definitely not contain subclasses
		of scopespace; intended for instantiations of constant
		parameters, (and when things get fancy) other types, 
		(even fancier) other template arguments.  
		Consider list of paramter_expressions.  
	 */
	typedef	list_of_ptr<object>		template_param_list;

// members
protected:
	/**
		Optional set of template parameters with which a
		type is instantiated.  
		Types must match that of template signature.  
		Hmmm... maybe need a concrete_type class...
		distinguish type_reference from type_instance.  
		This is owned, and thus must be deleted.  
	 */
	template_param_list*		template_params;

public:
	type_reference_base(template_param_list* pl);
virtual	~type_reference_base();

virtual const definition_base* get_base_def(void) const = 0;
// type equivalence relationship

};

//-----------------------------------------------------------------------------
class data_type_reference : public type_reference_base {
protected:
//	template_param_list*		template_params;	// inherited
	const datatype_definition*	base_type_def;
public:
	data_type_reference(const datatype_definition* td, 
		template_param_list* pl = NULL);
virtual	~data_type_reference();

	ostream& what(ostream& o) const;
	const definition_base* get_base_def(void) const;
};

//-----------------------------------------------------------------------------
class channel_type_reference : public type_reference_base {
protected:
//	template_param_list*		template_params;	// inherited
	const channel_definition*	base_chan_def;
public:
	channel_type_reference(const channel_definition* td, 
		template_param_list* pl = NULL);
virtual	~channel_type_reference();

	ostream& what(ostream& o) const;
	const definition_base* get_base_def(void) const;
};

//-----------------------------------------------------------------------------
class process_type_reference : public type_reference_base {
protected:
//	template_param_list*		template_params;	// inherited
	const process_definition*	base_proc_def;
public:
	process_type_reference(const process_definition* td, 
		template_param_list* pl = NULL);
virtual	~process_type_reference();

	ostream& what(ostream& o) const;
	const definition_base* get_base_def(void) const;
};

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
	subclass of type_reference_base.  
 */
class instance_reference_base : public object {
protected:
	/**
		Optional array indices (not ranges).  
		Indices may be symbolic.  
		Should be list of parameter instances, possibly constants.  
	 */
	typedef	list_of_ptr<object>		array_index_list;

protected:
	array_index_list*			array_indices;
// for subclasses:
//	const instantiation_base*		inst_ref;

public:
	instance_reference_base(array_index_list* i = NULL);
virtual	~instance_reference_base();

};

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of datatype.  
 */
class datatype_instance_reference : public instance_reference_base {
protected:
//	array_index_list*			array_indices;	// inherited
	const datatype_instantiation*		data_inst_ref;

public:
	datatype_instance_reference(const datatype_instantiation* dt, 
		array_index_list* i = NULL);
virtual	~datatype_instance_reference();

};

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of channel.  
 */
class channel_instance_reference : public instance_reference_base {
protected:
//	array_index_list*			array_indices;	// inherited
	const channel_instantiation*		channel_inst_ref;

public:
	channel_instance_reference(const channel_instantiation* dt, 
		array_index_list* i = NULL);
virtual	~channel_instance_reference();

};

//-----------------------------------------------------------------------------
/**
	A reference to a single instance of process.  
 */
class process_instance_reference : public instance_reference_base {
protected:
//	array_index_list*			array_indices;	// inherited
	const process_instantiation*		process_inst_ref;

public:
	process_instance_reference(const process_instantiation* dt, 
		array_index_list* i = NULL);
virtual	~process_instance_reference();

};

//=============================================================================
/**
	Process definition.  Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	Consider deriving definition from name_space privately.  
 */
class process_definition : public definition_base {
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
	typedef hashlist_of_const_ptr<string, datatype_instantiation>
							temp_formal_set;
	
	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
	 */
	typedef hashlist_of_const_ptr<string, instantiation_base>
							port_formal_set;

	/**
		Table of local instantiations (orderless) of 
		data-types and channels.  
		These instantiations are owned by this process.  
	 */
	typedef map_of_ptr<string, instantiation_base>	inst_set;

	// List of language bodies, separate or merged?

// inherited:
//	typedef	hash_map<string, object*>	used_id_map_type;

protected:
//	string			key;		// inherited
//	used_id_map_type	used_id_map;	// inherited
	bool			def;		///< flag: declared or defined
	temp_formal_set		temp_formals;
	port_formal_set		port_formals;
	// list language bodies
	
public:
	process_definition(const name_space* o, const string& s, const bool d);
virtual	~process_definition();

virtual	ostream& what(ostream& o) const;

// OBSOLETE:
//	bool equals_signature(const process_signature& ps) const;

	bool is_defined(void) const { return def; }

void	add_template_formal(const datatype_definition* d, 
		const template_formal_id& t);
void	add_port_formal(const datatype_definition* d, 
		const port_formal_id& p);

/*** to add (non-virtual):
void	add_process_instantiation(...);
void	add_datatype_instantiation(...);
void	add_channel_instantiation(...);
void	add_language_body(...);
***/

};	// end class process_definition

//=============================================================================
/**
	Process instantiation.  
 */
class process_instantiation : public instantiation_base {
protected:
	/**
		The type of process being instantiated.  
		Temporary: non-const and owned.  
	 */
	process_type_reference*		type;
//	const process_type_reference*		type;

	// reserve these for connections between instance_references
	// list of template actuals
	// list of port actuals

public:
	process_instantiation(const name_space* o, 
//		const process_type_reference* pt,
		process_type_reference* pt,
		const string& n);
virtual	~process_instantiation();

virtual	ostream& what(ostream& o) const;
// virtual	bool equals_port_formal(const port_formal_decl& tf) const;
};

//=============================================================================
/**
	Abstract base class for types and their representations.
 */
class datatype_definition : public definition_base {
protected:
// inherited:
//	string			key;		///< name of type
public:
	datatype_definition(const name_space* o, const string& n);
virtual	~datatype_definition();

string	get_qualified_name(void) const;
virtual	ostream& what(ostream& o) const = 0;
virtual	const datatype_definition* resolve_canonical(void) const;
virtual	bool type_equivalent(const datatype_definition& t) const = 0;
};

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	May potentially build off of this as a base class for 
	specialization.  
 */
class built_in_datatype_def : public datatype_definition {
public:
	built_in_datatype_def(const name_space* o, const string& n);
virtual	~built_in_datatype_def();

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const datatype_definition& t) const;
};

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in parameter types, pint and pbool.  
	Nothing can really be derived from them... yet.  
	Note that there is no intermediate param_definition class, 
	because parameter types can only be built in; there are no
	user-defined parameter types, for now...
 */
class built_in_param_def : public definition_base {
public:
	built_in_param_def(const name_space* p, const string& n);
virtual	~built_in_param_def();

	ostream& what(ostream& o) const;
};

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
	typedef	hashlist_of_const_ptr<string, datatype_definition>	type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist_of_const_ptr<string, datatype_definition>	temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_def_datatype(const name_space* o, const string& name);
virtual	~user_def_datatype() { }

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const datatype_definition& t) const;
};

//-----------------------------------------------------------------------------
/// abstract base class for channels and their representations
class channel_definition : public definition_base {
protected:
	string			key;		///< name of type
public:
	channel_definition(const name_space* o, const string& n);
virtual	~channel_definition();

virtual	ostream& what(ostream& o) const = 0;
	string	get_qualified_name(void) const;
	string	get_name(void) const;
};

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
	typedef	hashlist_of_const_ptr<string, datatype_definition>	type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist_of_const_ptr<string, datatype_definition>	temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_def_chan(const name_space* o, const string& name);
virtual	~user_def_chan() { }

virtual	ostream& what(ostream& o) const;
};

//-----------------------------------------------------------------------------
/// Type aliases are analogous to typedefs in C (not yet implemented)
// for renaming convenience
// going to use this mechanism to inherit built-in types into each namespace
//	to accelerate their resolution, not having to search sequentially
//	up to the global namespace.  
// add an alias into each scope's used_id_map...
class type_alias : public datatype_definition {
protected:
	/// pointer to the type represented by this type-id
	const datatype_definition*		canonical;
public:
	type_alias(const name_space* o, const string& n, 
		const datatype_definition* t);
virtual	~type_alias();
	// never delete canonical (can't, it's const!)

// need not be virtual
const datatype_definition*	resolve_canonical(void) const;

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const datatype_definition& t) const;
};

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class datatype_instantiation : public instantiation_base {
protected:
//	const data_type_reference*	type;		///< the actual type
	data_type_reference*		type;		// temporarily owned
public:
	datatype_instantiation(const name_space* o, 
//		const data_type_reference* t,
		data_type_reference* t,
		const string& n);
virtual	~datatype_instantiation();

virtual	ostream& what(ostream& o) const;

	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	bool equals_port_formal(const port_formal_decl& tf) const;
};

//=============================================================================
/**
	Instantiation of a channel type.  
 */
class channel_instantiation : public instantiation_base {
protected:
//	const channel_type_reference*	type;
	channel_type_reference*		type;		// temporarily owned
public:
	channel_instantiation(const name_space* o, 
//		const channel_type_reference* ct,
		channel_type_reference* ct,
		const string& n);
virtual	~channel_instantiation();

virtual	ostream& what(ostream& o) const;
};	// end class channel_instantiation

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
 */
class param_instantiation : public instantiation_base {
protected:
	/**
		Type refers directly to a definition in this case, 
		because parameters are never templatable.  
	 */
	const built_in_param_def*	type;		// never owned
public:
	param_instantiation(const name_space* o, 
		const built_in_param_def* pt, const string& n);
virtual	~param_instantiation();

virtual	ostream& what(ostream& o) const;
};	// end class param_instantiation

//=============================================================================
};	// end namespace entity
};	// end namespace ART

#endif	// __ART_OBJECT_H__

