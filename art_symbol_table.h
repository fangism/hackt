// "art_symbol_table.h"
// class declarations and definitions for symbol table classes

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>

#include <list>
#include <stack>
#include <map>

// #include "art_object.h"

namespace ART {

//=============================================================================
// forward declarations from another namespace
namespace entity {
	class name_space;
};
using namespace entity;

namespace parser {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
class namespace_symtab;
class definition_symtab;
class template_symtab;
class formals_symtab;

//=============================================================================
/// Class for a registered type, used by namespaces...
class type_entry {

};

//-----------------------------------------------------------------------------
class definition_entry {

};
// or typedef pair<> definition_entry?
//-----------------------------------------------------------------------------
class instance_entry {

};

//=============================================================================
/**
	The mother symbol table class.  
	Subclasses will be context-specific.  
 */
class symbol_table {
protected:
	const symbol_table*		parent;
public:
	symbol_table(symbol_table* p = NULL) : parent(p) { }
virtual	~symbol_table() { }

// pure virtual functions
	
};

//-----------------------------------------------------------------------------
/**
	Namespace's symbol table.  
	The root symbol table will be for the global namespace.  
 */
class namespace_symtab : public symbol_table {
public:
	// types
	// definitions
	// instantiations

};

//-----------------------------------------------------------------------------
/**
	The symbol table for general definitions.  
 */
class definition_symtab : public symbol_table {

};

//-----------------------------------------------------------------------------
/**
	The symbol table within a type's or definition's template context.  
 */
class template_symtab : public symbol_table {

};

//-----------------------------------------------------------------------------
/**
	The symbol table within a definition's formals context.  
 */
class formals_symtab : public symbol_table {

};

//=============================================================================
// what is a context object?
// a reference to:
//	1) the current stack of visible scopes (goes up to global)
//	2) the list of available outside scopes
//	3) exception / error stack
//	4) for namespace-qualified identifiers, the chain of scopes
//	5) for index/member identifiers, the current search scope
//	6) a certain type that is to be expected

/// the contect object is what is passed down the type-checker routines
// the context is updated in the order of the syntax tree traversal
class context {
protected:
	stack<name_space*>	namespace_scope;
// are we in some expression? what depth?
public:
	context() :
		namespace_scope()	// empty stack
		{ }


// object*	lookup_namespace(const token_identifier& n);

};

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

