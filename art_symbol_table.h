// "art_symbol_table.h"
// class declarations and definitions for symbol table classes
// really need to rename this as "art_context"

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>
#include <string>

// add as they are needed
#include <stack>
#include "list_of_ptr.h"		// PHASE OUT
// #include <map>

namespace ART {

//=============================================================================
// forward declarations from another namespace, from "art_object.h"
// avoids having to include "art_object.h"
namespace entity {
	// ... and more as they are needed
	class scopespace;
	class name_space;
	class built_in_type_def;
	class fundamental_type_reference;
	class instantiation_base;
	class instance_reference_base;
	class definition_base;
	class user_def_chan;
	class user_def_type;
	class channel_definition;
	class channel_type_reference;
	class channel_instantiation;
	class channel_instance_reference;
	class datatype_definition;
	class data_type_reference;
	class datatype_instantiation;
	class datatype_instance_reference;
	class process_definition;
	class process_type_reference;
	class process_instantiation;
	class process_instance_reference;
	class built_in_param_def;
	class param_type_reference;
	class param_instantiation;
	class param_instance_reference;
	class param_expr;
	// replicated typedefs from "art_object.h"
	typedef list_of_ptr<param_expr>		array_dim_list;
	typedef list_of_const_ptr<param_expr>	template_param_list;
};
using namespace entity;

namespace parser {
//=============================================================================
using namespace std;

//=============================================================================
// forward declarations
class qualified_id;
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
	Some of these members should be collapsed into mutexs...
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
		Remember, these name_space pointers are not owned.  
	 */
	stack<name_space*>	namespace_stack;
#define current_namespace	namespace_stack.top()

	/**
		Pointer to the current definition that is open for 
		modification.  
		One pointer is sufficient for all definitions because
		only one definition can be open at a time.  
	 */
	definition_base*	current_open_definition;
	/**
		Flag that indicates whether or not we are declaring
		a new definition, or checking against a previously
		defined or declared definition.  
	 */
	bool			check_against_previous_definition_signature;

	/**
		Pointer to the current definition referenced, usually
		resolved by the last identifier.  
		May point to any definition for a channel, data-type, 
		process, or even the dummy built-in parameter and data-types.  
		The definition will be combined with optional 
		template parameters to form a type reference (below).  
	 */
	const definition_base*		current_definition_reference;

	/**
		Pointer to the concrete type to instantiate.  
	 */
	const fundamental_type_reference*	current_fundamental_type;

	/**
		Instance(s) referenced to connect.  
		Must type check.  
		Not used yet.
	 */
	const instance_reference_base*	current_instance_to_connect;

	/**
		List of parameter expressions to use as template
		arguments with the current_definition_reference, 
		to create a concrete fundamental_type_reference.  
		Should not be const? will be owned by type_reference?
		TO DO:
		Now that these are declared before the definition, 
		we need to hold on to these until the definition is open, 
		and then add them to the definition's used_id_map.  
		Remember to search this list for parameters that depend on
		other parameters, such as in the case of array dimensions.  
	 */
	template_param_list*		current_template_arguments;

	// need current_blah_inst for array/dimension/range additions
	const array_dim_list*		current_array_dimensions;


	/**
		Since loop and conditional contexts may be nested, 
		we need to keep track of them in a stack.  
		These pointers are modifiable, but are not owned.  
		Remember to push NULL initially.  
	 */
	stack<scopespace*>		dynamic_scope_stack;
#define	current_dynamic_scope		dynamic_scope_stack.top()

public:
	/// The number of semantic errors to accumulate before bailing out.  
	static const long	type_error_limit = 3;

	// optional include short-cut pointers to built-ins
	/**
		Read-only shortcut pointer to the global namespace.
		Can this be modified inadvertently?
	 */
	const name_space*	global_namespace;


public:
	context(name_space* g);
	~context();

// TO DO: sort methods by where they are expected to be invoked
//	i.e. from the syntax tree check_build, or from the symbol_table objects

void	open_namespace(const token_identifier& id);
void	close_namespace(void);
void	using_namespace(const qualified_id& id);
void	alias_namespace(const qualified_id& id, const string& a);
const name_space*	top_namespace(void) const;

void	declare_process(const token_identifier& ps);
void	open_process(const token_identifier& ps);
// used for process and definitions
void	close_process_definition();

void	declare_datatype(const token_identifier& ds);
void	open_datatype(const token_identifier& ds);
void	close_datatype_definition();

void	declare_chantype(const token_identifier& ds);
void	open_chantype(const token_identifier& ds);
void	close_chantype_definition();

const scopespace*	get_current_scope(void) const;
scopespace*		get_current_scope(void);
const name_space*	get_current_namespace(void) const {
				return current_namespace;
			}
template_param_list*	get_current_template_arguments(void) const {
				return current_template_arguments;
			}

// sets context's definition for instantiation
const definition_base*		get_current_definition_reference(void) const
				{ return current_definition_reference; }
const definition_base*		set_current_definition_reference(
					const definition_base& d) {
					current_definition_reference = &d;
					return &d;
				}
const fundamental_type_reference*	get_current_fundamental_type(void) const
				{ return current_fundamental_type; }

const datatype_definition*	get_current_datatype_definition(void) const;

	// for keyword: int or bool
const datatype_definition*	set_datatype_def(const token_datatype& tid);
	// set template argument separately!
const datatype_definition*	set_inst_data_def(const datatype_definition& );
	// need to assert(!inst_data_type_ref)?
const data_type_reference*	set_inst_data_type_ref(const data_type_reference& dr);

// should be called by parser after done using definitions
void	reset_current_definition_reference(void);
void	reset_current_fundamental_type(void);

const built_in_param_def*	get_current_param_definition(void) const;
const built_in_param_def*	set_inst_param_def(const built_in_param_def& );
const param_type_reference*	set_inst_param_type_ref(const param_type_reference& pr);
const built_in_param_def*	set_param_def(const token_paramtype& pt);
// const param_type_reference*	set_param_type_ref(...);
// void	unset_paramtype_def(void);

const channel_definition*	get_current_channel_definition(void) const;
const channel_definition*	set_inst_chan_def(const channel_definition& );
const channel_type_reference*	set_inst_chan_type_ref(const channel_type_reference& cr);

const process_definition*	get_current_process_definition(void) const;
const process_definition*	set_inst_proc_def(const process_definition& );
const process_type_reference*	set_inst_proc_type_ref(const process_type_reference& pr);

// to be called from parser's check_build
const fundamental_type_reference*
	set_current_fundamental_type(void);
// to be called from symbol_table type_reference_base classes 
const fundamental_type_reference*
	set_current_fundamental_type(const fundamental_type_reference& tr);

void	set_current_template_arguments(template_param_list& tl) {
			assert(!current_template_arguments);
			current_template_arguments = &tl;
		}
void	reset_current_template_arguments(void);

const object*		lookup_object(const qualified_id& id) const;
const definition_base*	lookup_definition(const token_identifier& id) const;
const definition_base*	lookup_definition(const qualified_id& id) const;
const instantiation_base* lookup_instance(const token_identifier& id) const;
const instantiation_base* lookup_instance(const qualified_id& id) const;
// need qualified_id versions...

const instantiation_base*	add_instance(const token_identifier& id);
// const?
datatype_instantiation*	add_datatype_instance(const token_identifier& id);
	// make another version overloaded for arrays
param_instantiation*	add_paramtype_instance(const token_identifier& id);

const datatype_instantiation*	add_template_formal(const token_identifier& id);


// repeat for processes and channels...

string		auto_indent(void) const;

private:
void close_current_definition(void);
};	// end class context

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

