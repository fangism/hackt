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
	class datatype_definition;
	class datatype_instantiation;
	class process_definition;
	class process_instantiation;
	class built_in_param_def;
};
using namespace entity;

namespace parser {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
class id_expr;
class token_identifier;
class token_datatype;
class token_paramtype;

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
//	name_space*		current_namespace;
#define current_namespace	ns_stack.top()

	/**
		Pointer to current channel type, 
		which is being declared or defined.  Do not delete.  
		No stack needed, as definitions may not be nested.  
	 */
	user_def_chan*		current_chan_def;
	/**
		Flag that indicates whether the current channel definition
		in this context is new (first-encounter) or is a repeat, 
		in which case, the current prototype's formals
		should be type-matched one-for-one against the previous
		declaration.  
	 */
	bool			check_against_prev_chan;

	/**
		Pointer to current data type, 
		which is being declared or defined.  Do not delete.  
		No stack needed, as definitions may not be nested.  
	 */
	user_def_type*		current_type_def;
	/**
		Flag that indicates whether the current data type definition
		in this context is new (first-encounter) or is a repeat, 
		in which case, the current prototype's formals
		should be type-matched one-for-one against the previous
		declaration.  
	 */
	bool			check_against_prev_type;

	/**
		Reference to the current process, which is being declared
		or defined.
		Do not delete.
	 */
	process_definition*	current_proc_def;
	/**
		Flag that indicates whether the current process definition
		in this context is new (first-encounter) or is a repeat, 
		in which case, the current prototype's formals
		should be type-matched one-for-one against the previous
		declaration.  
	 */
	bool			check_against_prev_process;

	/**
		Reference to the current process type being instantiated.  
	 */
	const process_definition*	inst_proc_def;

	/**
		Reference to the current data type being instantiated.  
	 */
	const datatype_definition*	inst_data_def;

	/**
		Reference to the current channel begin instantiated.  
	 */
	const channel_definition*	inst_chan_def;

	/**
		Reference to the current parameter being declared.
	 */
	const built_in_param_def*	inst_param_def;

	// need current_blah_inst for array/dimension/range additions

	// to be used in conjunction with inst_data_def
	// current_template_params;

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

void	declare_process(const token_identifier& ps);
void	open_process(const token_identifier& ps);
// used for process and definitions
void	close_process();

void	declare_datatype(const token_identifier& ds);
void	open_datatype(const token_identifier& ds);
void	close_datatype();

void	declare_chantype(const token_identifier& ds);
void	open_chantype(const token_identifier& ds);
void	close_chantype();

// sets context's definition for instantiation
const datatype_definition*	set_datatype_def(const id_expr& tid);
const datatype_definition*	set_datatype_def(const token_string& tid);
	// for keyword: int or bool
const datatype_definition*	set_datatype_def(const token_datatype& tid);
	// set template argument separately!
void	unset_datatype_def(void);

const built_in_param_def*	set_param_def(const token_paramtype& pt);
void	unset_paramtype_def(void);

void	set_template_params(void);
void	unset_template_params(void);

datatype_instantiation*	add_type_instance(const token_identifier& id);
	// make another version overloaded for arrays

const datatype_instantiation*	add_template_formal(const token_identifier& id);


// repeat for processes and channels...

string		auto_indent(void) const;

};	// end class context

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

