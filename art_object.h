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
	class token_string;
	class token_identifier;
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
	const name_space*	parent;

	/**
		The name of this namespace, also used as the map key for 
		the table that contains this namespace.  
	 */
	string			key;
public:
	name_space(const token_identifier& n, const name_space* p);
// explicit name_space();	// for GLOBAL?
	~name_space() { }

// update these return types later
void	add_open_namespace(const token_identifier& n);
void	add_using_directive(const id_expr& n);
void	add_using_alias(const id_expr& n, const token_identifier& a);
void	leave_namespace(void);	// or close_namespace
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
	typedef	list<const type_definition*>	type_members;
protected:
	string			key;		///< name of type
	// list of other type definitions
	type_members		template_params;
	type_members		members;
public:
	user_type_def(const token_identifier& name);
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

