// "art_symbol_table.h"
// class declarations and definitions for symbol table classes

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>

#include <stack>
#include <map>

namespace ART {
namespace parser {
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
};
};

#endif	// __ART_SYMBOL_TABLE_H__

