// "art_object.h"

#ifndef	__ART_OBJECT_H__
#define	__ART_OBJECT_H__

/****
	Should the classes in this file maintain the symbol tables?
	Should symbol tables be generic or class-specific and separate?

****/

#include <list>
#include <map>

#include "art_macros.h"
#include "art_parser.h"

namespace ART {
//=============================================================================
// forward declarations
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
//	class token_string;
//	class token_identifier;
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
class instantiation;

//=============================================================================
/// the root object type
class object {

};

//=============================================================================
/// class for namespace
class name_space : public object {
	// list of pointers to other opened namespaces (and their aliases)
	// table of type definitions (user-defined data types)
	// table of process definitions
	// table of real instantiations (outside of definitions)
private:
	/// may become hash_map if need be, for now map suffices (r/b-tree)
	typedef	map<string, name_space*>	ns_map_type;

	/// container for open namespaces with optional aliases
	typedef map<string, name_space*>	using_map_type;
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
	ns_map_type		subns;

	/**
		The set of namespaces which are open to search within
		this namespace's scope.  The imported namespaces may be 
		re-opened and renamed.  When this namespace closes, however, 
		the list of open spaces and aliases will be purged.  
		You'll have to add them each time you re-open this
		namespace if you want them, otherwise, it is a convenient
		way to reset the namespace alias list.  

		Q: the key for these aliases: should they be the full
		id_expr, or just the tail end?
		The whole id_expr, use aliases to reduce the names....?
	 */
	using_map_type		open_aliases;

public:
	name_space(const string& n, name_space* p);
// explicit name_space();	// for GLOBAL?
	~name_space();

// update these return types later
name_space*	add_open_namespace(const string& n);
name_space*	add_using_directive(const id_expr& n);
name_space*	add_using_alias(const id_expr& n, const string& a);
name_space*	leave_namespace(void);	// or close_namespace

// some private utility functions (may become public later)
private:
name_space*	query_namespace_match(const id_expr& id);
name_space*	query_subnamespace_match(const id_expr& id);
name_space*	query_import_namespace_match(const id_expr& id);
name_space*	find_namespace_ending_with(const id_expr& id);
name_space*	find_namespace_starting_with(const id_expr& id);
};

//=============================================================================
class definition : public object {

};

//=============================================================================
class instantiation : public object {

};

//=============================================================================
class process_definition : public definition {
	// table of template formals
	// table of port formals
	// table of sub-instantiations
	// table of language bodies

};

//=============================================================================
class process_instantiation : public instantiation {

};

//=============================================================================
/// abstract base class for types and their representations
class type_definition : public definition {

};

//-----------------------------------------------------------------------------
class built_int_type_def : public type_definition {

};

//-----------------------------------------------------------------------------
// no formal name, just type name
class user_type_def : public type_definition {
private:
	/// the members will be kept as a list or map
	typedef	list<const type_definition*>	type_members;
protected:
	string			key;		///< name of type
	// list of other type definitions
	type_members		template_params;
	type_members		members;
public:
	user_type_def(const string& name);
virtual	~user_type_def() { }

};

//=============================================================================
class type_instantiation : public instantiation {
protected:
	const type_definition*	type;
	string			key;		///< name of instance
public:


};

//=============================================================================
// built-in types will be added to the global scope


//=============================================================================
// built-in types need to be statically initialized

// static type_definition("int")

//=============================================================================
};
};

#endif	// __ART_OBJECT_H__

