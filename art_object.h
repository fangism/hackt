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
class definition;
class type_definition;
class process_definition;
class instantiation;
class type_instantiation;
class process_instantiation;
class built_in_type_def;

//=============================================================================
/// the root object type
class object {
public:
virtual ~object() { }
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
	typedef	map_of_ptr<string, type_definition>	type_def_set;
	/// list useful for query returns, const pointer?
	typedef	list_of_const_ptr<type_definition>	type_def_list;

	/// resolves identifier to actual data type, we own these pointers
	typedef	map_of_ptr<string, type_instantiation>	type_inst_set;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_definition>	proc_def_set;
	typedef	list_of_const_ptr<process_definition>	proc_def_list;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_instantiation>	proc_inst_set;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are not owned by this namespace.  
		Furthermore, they are read-only.  
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
		These (redundandlty) stored copies of pointers are read-only.  
		To get the modifiable pointers, you'll need to look them up 
		in the corresponding type-specific map.  
	 */
	typedef	hash_map_of_const_ptr<string, object>	used_id_map_type;

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
built_in_type_def*	add_built_in_type_definition(built_in_type_def* d);
type_definition*	add_type_alias(const id_expr& t, const string& a);

// returns type if unique match found, else NULL
const type_definition*	lookup_unqualified_type(const string& id) const;
const type_definition*	lookup_qualified_type(const id_expr& id) const;

type_definition*	add_type_definition();
type_instantiation*	add_type_instantiation(const type_definition& t, 
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
void	query_type_def_match(type_def_list& m, const string& tid) const;
void	query_type_def_match(type_def_list& m, const type_id& tid) const;
void	query_type_inst_match(type_def_list& m, const string& tid) const;
void	query_type_inst_match(type_def_list& m, const id_expr& tid) const;
void	query_proc_def_match(proc_def_list& m, const type_id& pid) const;
void	query_proc_inst_match(proc_def_list& m, const id_expr& pid) const;

// the following are not used... yet
void	find_namespace_ending_with(namespace_list& m, 
		const id_expr& id) const;
void	find_namespace_starting_with(namespace_list& m, 
		const id_expr& id) const;

void	inherit_built_in_types(void);

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
// class definition : public object
class definition : public scopespace {
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
	definition(const string& n, const name_space* p);
virtual	~definition();

virtual	ostream& what(ostream& o) const = 0;
};	// end class definition

//=============================================================================
/**
	Base class for instantiation objects.  Recall that instantiations
	may appear both inside and outside definitions.  Instantiations
	inside definitions will build as part of the definitions, 			whereas those outside definitions will register as actual 
	instantiations in the object file.  
 */
class instantiation : public object {
protected:
	/**
		Back-pointer to the namespace to which this definition 
		belongs.  Should be const?  Do not delete.  
	 */
	const name_space*		owner;
	// TO DO: This would be a good place to include template instantiation
	// and array dimensions, assuming all types are parameterizable
	// and array-able.  (process, data-types, channels)

public:
	instantiation(const name_space* o);
virtual	~instantiation();

virtual	ostream& what(ostream& o) const = 0;
virtual	bool equals_port_formal(const port_formal_decl& tf) const = 0;
};	// end class instantiation

//=============================================================================
/**
	Process definition.  Contains optional set of template formals, 
	set of port formals, and body of instantiations and language bodies.  
	Is there a way to re-use name resolution code
	from class name_space without copying?  
	Consider deriving definition from name_space privately.  
 */
class process_definition : public definition {
public:
	/**
		Table of template formals.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Remember: template formals are accessible to the rest 
		of the body and to the port formals as well.  
		For now, the contained type is type_instantiation
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
	typedef hashlist_of_const_ptr<string, type_instantiation>
								temp_formal_set;
	
	/**
		Table of port formals.
		The types can be data-types or channel-types, 
		either base-types or user-defined types.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
	 */
	typedef hashlist_of_const_ptr<string, instantiation>	port_formal_set;

	/**
		Table of local instantiations (orderless) of 
		data-types and channels.  
		These instantiations are owned by this process.  
	 */
	typedef map_of_ptr<string, instantiation>	inst_set;

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

	bool equals_signature(const process_signature& ps) const;
	bool is_defined(void) const { return def; }

void	add_template_formal(const type_definition* d, 
		const template_formal_id& t);
void	add_port_formal(const type_definition* d, 
		const port_formal_id& p);

/*** to add (non-virtual):
void	add_process_instantiation(...);
void	add_type_instantiation(...);
void	add_channel_instantiation(...);
void	add_language_body(...);
***/

};	// end class process_definition

//=============================================================================
/**
	Process instantiation.  
 */
class process_instantiation : public instantiation {
private:

protected:
	string			key;		///< name of instance
	// list of template actuals
	// list of port actuals

public:
	process_instantiation(const name_space* o);
virtual	~process_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	bool equals_port_formal(const port_formal_decl& tf) const;
};

//=============================================================================
/**
	Abstract base class for types and their representations.
 */
class type_definition : public definition {
protected:
// inherited:
//	string			key;		///< name of type
public:
	type_definition(const name_space* o, const string& n);
virtual	~type_definition();

string	get_qualified_name(void) const;
virtual	string get_name(void) const;
virtual	ostream& what(ostream& o) const = 0;
virtual	const type_definition* resolve_canonical(void) const;
virtual	bool type_equivalent(const type_definition& t) const = 0;
};

//-----------------------------------------------------------------------------
/**
	Reserved for special built-in fundamental base types.  
	May potentially build off of this as a base class for 
	specialization.  
 */
class built_in_type_def : public type_definition {
public:
	built_in_type_def(const name_space* o, const string& n);
virtual	~built_in_type_def();

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const type_definition& t) const;
};

//-----------------------------------------------------------------------------
// need specialized type definitions for templates? (int<N>)
// class built_in_type_template_def : public built_in_type_def {
// perhaps contain a type definition and a template signature?
// }

//-----------------------------------------------------------------------------
// may need a template type that refers to an abstract template
// with template actuals/parameters
// or just use user_def_type below...

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_def_type : public type_definition {
private:
	/**
		Members will be kept as a hashlist
		because their order matters, or don't they?
	 */
	typedef	hashlist_of_const_ptr<string, type_definition>	type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist_of_const_ptr<string, type_definition>	temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_def_type(const name_space* o, const string& name);
virtual	~user_def_type() { }

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const type_definition& t) const;
};

//-----------------------------------------------------------------------------
/// abstract base class for channels and their representations
class channel_definition : public definition {
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
	typedef	hashlist_of_const_ptr<string, type_definition>	type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	hashlist_of_const_ptr<string, type_definition>	temp_param_list;
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
class type_alias : public type_definition {
protected:
	/// pointer to the type represented by this type-id
	const type_definition*		canonical;
public:
	type_alias(const name_space* o, const string& n, 
		const type_definition* t);
virtual	~type_alias();
	// never delete canonical (can't, it's const!)

// need not be virtual
const type_definition*	resolve_canonical(void) const;

virtual	ostream& what(ostream& o) const;
virtual	bool type_equivalent(const type_definition& t) const;
};

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class type_instantiation : public instantiation {
protected:
	const type_definition*	type;		///< the actual type
	string			key;		///< name of instance
	// need range of array
public:
	type_instantiation(const name_space* o, const type_definition* t,
		const string& n);
virtual	~type_instantiation();

virtual	ostream& what(ostream& o) const;
	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	bool equals_port_formal(const port_formal_decl& tf) const;
};

//=============================================================================
/***
class template_instance : public instantiation {
protected:
	list_of_ptr<...>		// some form of expression
public:

};
***/

//=============================================================================
};	// end namespace entity
};	// end namespace ART

#endif	// __ART_OBJECT_H__

