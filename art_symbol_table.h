// "art_symbol_table.h"
// class declarations and definitions for symbol table classes

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>
#include <string>

// add as they are needed
// #include <list>
// #include <stack>
// #include <map>

namespace ART {

//=============================================================================
// forward declarations from another namespace, from "art_object.h"
// avoids having to include "art_object.h"
namespace entity {
	// and more as they are needed
	class name_space;
	class built_in_type_def;
	class user_def_type;
	class type_definition;
	class type_instantiation;
	class process_definition;
	class process_instantiation;
};
using namespace entity;

namespace parser {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
class id_expr;
class token_identifier;

//=============================================================================
// what is a context object?
// a reference to:
//	1) the current stack of visible scopes (goes up to global)
//		no, the stack will be implicit
//	2) the list of available outside scopes
//	3) exception / error stack
//	4) for namespace-qualified identifiers, the chain of scopes
//	5) for index/member identifiers, the current search scope
//	6) a certain type that is to be expected

/// the contect object is what is passed down the type-checker routines
// the context is updated in the order of the syntax tree traversal
class context {
protected:
//	stack<name_space*>	namespace_scope;
// are we in some expression? what depth?
// what language context are we in? global? prs, chp, hse?

	long			indent;		///< for formatted output
	long			type_error_count;	///< error count
	/**
		A pointer to the current namespace scope.
		Do not delete.  
	 */
	name_space*		current_ns;

	/**
		Pointer to current data type, useful in list declarations.  
	 */
	type_definition*	current_dt;

	/// Reference to the current open process definition.  
	process_definition*	current_proc_def;
	/// Reference to the current open data type definition.  
	user_def_type*		current_type_def;
	// Reference to the current open channel definition.  
	// user_chan_def*	current_chan_def;

public:
	/// the number of semantic errors to accumulate before bailing out
	static const long	type_error_limit = 3;

public:
	/**
		Constructing a context should only be done once (per source
		file).  It initializes a global namespace with the 
		appropriate built-in types defined.  
	 */
	context();
	~context();

name_space*	open_namespace(const token_identifier& id);
void		close_namespace(void);
name_space*	using_namespace(const id_expr& id);
name_space*	alias_namespace(const id_expr& id, const string& a);

type_definition*	set_type_def(const id_expr& tid);
type_definition*	set_type_def(const token_string& tid);
	// for keyword: int or bool
type_definition*	set_type_def(const token_string& tid, const expr& w);
	// for int<width>, for now only accept token_int... later expressions
void	unset_type_def(void);

string		auto_indent(void) const;

};

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

