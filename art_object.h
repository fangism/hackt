// "art_object.h"

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

#include <string>

#include "map_of_ptr.h"
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
/// class for namespace
class name_space : public object {
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

private:
	/**
		Container owned sub-namespaces.  Needs no ordering.  
	 */
	typedef	map_of_ptr<string, name_space>		subns_map_type;

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
	typedef	list<type_definition*>			type_def_list;

	/// resolves identifier to actual data type, we own these pointers
	typedef	map_of_ptr<string, type_instantiation>	type_inst_set;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_definition>	proc_def_set;
	typedef	list<process_definition*>		proc_def_list;

	/// resolves identifier to actual process type, we own these pointers
	typedef	map_of_ptr<string, process_instantiation>	proc_inst_set;

	/**
		Container for open namespaces with optional aliases.  
		Doesn't have to be a map because never any need to search
		by key.  List implementation is sufficient, because
		whole list will always be searched, if it is searched at all.  
		These pointers are not owned by this namespace.  
	 */
	typedef list<name_space*>			namespace_list;

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
	 */
	typedef	hash_map<string,object*>		used_id_map_type;

protected:
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
	name_space*		parent;

	/**
		The name of this namespace, also used as the map key for 
		the table that contains this namespace.  
	 */
	string			key;

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
	subns_map_type		open_aliases;

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

	/**
		Before mapping a new symbol to a symbolic object, 
		it must not already be mapped to an existing object.  
		All additions to the current namespace must register
		through this map.  
	 */
	used_id_map_type	used_id_map;

public:
	name_space(const string& n, name_space* p);
// explicit name_space();	// for GLOBAL?
	~name_space();

string	get_qualified_name(void) const;

virtual	ostream& what(ostream& o) const;

// update these return types later
name_space*	add_open_namespace(const string& n);
name_space*	leave_namespace(void);	// or close_namespace
name_space*	add_using_directive(const id_expr& n);
name_space*	add_using_alias(const id_expr& n, const string& a);

// to be used ONLY by the global namespace
built_in_type_def*	add_built_in_type_definition(built_in_type_def* d);
type_definition*	add_type_alias(const id_expr& t, const string& a);

// returns type if unique match found, else NULL
type_definition*	instance_type(const string& id);


type_definition*	add_type_definition();
type_instantiation*	add_type_instantiation();
process_definition*	add_proc_definition();
process_instantiation*	add_proc_instantiation();

// some private utility functions (may become public later)
// add versions for querying for types, instantiations, etc...
private:
name_space*	query_namespace_match(const id_expr& id);
name_space*	query_subnamespace_match(const id_expr& id);
void	query_import_namespace_match(namespace_list& m, const id_expr& id);

// these will not be recursive, but iteratively invoked by
// add_blah_inst/def();
void	query_type_def_match(type_def_list& m, const string& tid);
void	query_type_def_match(type_def_list& m, const type_id& tid);
void	query_type_inst_match(type_def_list& m, const id_expr& tid);
void	query_proc_def_match(proc_def_list& m, const type_id& pid);
void	query_proc_inst_match(proc_def_list& m, const id_expr& pid);

object*	what_is(const string& id);

// the following are not used... yet
void	find_namespace_ending_with(namespace_list& m, const id_expr& id);
void	find_namespace_starting_with(namespace_list& m, const id_expr& id);

void	inherit_built_in_types(void);

// will we need generalized versions of queries that return object*
// if we don't know a priori what an identifier's class is?
// single symbol table or separate?

};

//=============================================================================
/// base class for definition objects
class definition : public object {
protected:
	/**
		Back-pointer to the namespace to which this definition 
		belongs.  Should be const?  Do not delete.  
	 */
	const name_space*		owner;
public:
	definition(const name_space* o) : object(), owner(o) { }
virtual	~definition() { }

virtual	ostream& what(ostream& o) const = 0;
};

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
public:
	instantiation(const name_space* o) : object(), owner(o) { }
virtual	~instantiation() { }

virtual	ostream& what(ostream& o) const = 0;
};

//=============================================================================
/**
	Process definition.
 */
class process_definition : public definition {
	// table of template formals
	// table of port formals
	// table of sub-instantiations
	// table of language bodies, or should these just be merged?
private:
	// remember: template formals are accessible to the rest of the body
	// and to the port formals as well
	
protected:
	
public:
	process_definition(const name_space* o) : definition(o) { }
virtual	~process_definition();
};

//=============================================================================
/**
	Process instantiation.  
 */
class process_instantiation : public instantiation {
private:

protected:
	// list of template actuals
	string			key;		///< name of instance

public:
	process_instantiation(const name_space* o) : instantiation(o) { }
virtual	~process_instantiation();

virtual	ostream& what(ostream& o) const;
};

//=============================================================================
/// abstract base class for types and their representations
class type_definition : public definition {
protected:
	string			key;		///< name of type
public:
	type_definition(const name_space* o, const string& n);
virtual	~type_definition();

string	get_qualified_name(void) const;
virtual	string get_name(void) const;
virtual	ostream& what(ostream& o) const = 0;
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
};

//-----------------------------------------------------------------------------
// may need a template type that refers to an abstract template
// with template actuals/parameters
// or just use user_def_type below...

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_type_def : public type_definition {
private:
	/**
		Members will be kept as a map, 
		because their order doesn't matter, or do they?
	 */
	typedef	hash_map<string, const type_definition*>	type_members;
	/**
		Template parameter will be kept in a list because their
		order matters in type-checking.
	 */
	typedef	list<const type_definition*>	temp_param_list;
protected:
	// list of other type definitions
	temp_param_list		template_params;
	type_members		members;
public:
	user_type_def(const name_space* o, const string& name);
virtual	~user_type_def() { }

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
	// never delete canonical

const type_definition*	resolve_canonical(void) const;

virtual	ostream& what(ostream& o) const;
};

//=============================================================================
/// Instantiation of a data type, either inside or outside definition.  
class type_instantiation : public instantiation {
protected:
	const type_definition*	type;		/// the actual type
	string			key;		///< name of instance
	// more to come...
public:
	type_instantiation(const name_space* o, const type_definition* t,
		const string& n);
virtual	~type_instantiation();

virtual	ostream& what(ostream& o) const;
};

//=============================================================================
};	// end namespace entity
};	// end namespace ART

#endif	// __ART_OBJECT_H__

