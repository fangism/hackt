// "art_symbol_table.h"
// class declarations and definitions for symbol table classes

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>
#include <string>

// add as they are needed
#include <stack>
// #include <list>
// #include <map>

namespace ART {

//=============================================================================
// forward declarations from another namespace, from "art_object.h"
// avoids having to include "art_object.h"
namespace entity {
	// ... and more as they are needed
	class name_space;
	class built_in_type_def;
	class user_def_chan;
	class user_def_type;
	class channel_definition;
	class channel_instantiation;
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

/**
	The context object is what is passed down the type-checker routines, 
	and is updated in the order of the syntax tree traversal.  
	No virtual functions.  
 */
class context {
protected:
//	stack<name_space*>	namespace_scope;
// are we in some expression? what depth?
// what language context are we in? global? prs, chp, hse?

	long			indent;		///< for formatted output
	long			type_error_count;	///< error count

	/**
		Explicit namespace stack.  
		Maintained here instead of using pointers returned by
		leave_namespace() to keep const parent (namespace) pointer
		semantics.  
		Also may be useful in debugging to see state of namespace
		stack more easily.  
	 */
	stack<name_space*>	ns_stack;
	/*
		OBSOLETE: A pointer to the current namespace scope.
		Do not delete.  
	 */
//	name_space*		current_ns;
#define current_ns		ns_stack.top()

	/**
		Pointer to current data type, useful in list declarations.  
		Do not delete.  
		No stack needed, as definitions may not be nested.  
	 */
	const type_definition*	current_dt;

	/**
		Reference to the current open process definition.
		Do not delete.
	 */
	process_definition*	current_proc_def;

	/**
		Reference to the current open data type definition.  
		Do not delete.
	 */

	user_def_type*		current_type_def;

	/**
		Reference to the current open channel definition.
		Do not delete.
	 */
	user_def_chan*		current_chan_def;

public:
	/// The number of semantic errors to accumulate before bailing out.  
	static const long	type_error_limit = 3;

public:
	context(name_space* g);
	~context();

void	open_namespace(const token_identifier& id);
void	close_namespace(void);
void	using_namespace(const id_expr& id);
void	alias_namespace(const id_expr& id, const string& a);
const name_space*	top_namespace(void) const;

const type_definition*	set_type_def(const id_expr& tid);
const type_definition*	set_type_def(const token_string& tid);
	// for keyword: int or bool
type_definition*	set_type_def(const token_string& tid, const expr& w);
	// for int<width>, for now only accept token_int... later expressions
void	unset_type_def(void);

type_instantiation*	add_type_instance(const token_identifier& id);
	// make another version overloaded for arrays

string		auto_indent(void) const;

};	// end class context

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

