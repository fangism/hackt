// "art_symbol_table.h"
// class declarations and definitions for symbol table classes
// really need to rename this as "art_context"

#ifndef __ART_SYMBOL_TABLE_H__
#define __ART_SYMBOL_TABLE_H__

#include <stdlib.h>
#include <string>
#include <stack>
#include <list>

#include "ptrs.h"
#include "count_ptr.h"

namespace ART {

//=============================================================================
// forward declarations from another namespace, from "art_object.h"
// avoids having to include "art_object.h"
namespace entity {
	using std::list;			
	using namespace fang;			// for pointer classes

	// ... and more as they are needed
	class object;
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
	class range_expr_list;

	class instance_collection_stack_item;
	// try to convert this to excl_ptr...
	typedef count_const_ptr<range_expr_list>
				index_collection_item_ptr_type;

	class connection_assignment_base;
	class param_expression_assignment;
	class instance_reference_connection;

	// replicated typedefs from "art_object.h"
//	typedef class list<never_const_ptr<param_expr> >	array_dim_list;
	typedef class list<never_const_ptr<param_expr> >	template_param_list;
}
using namespace entity;

namespace parser {
//=============================================================================
using namespace std;
using namespace fang;			// for pointer classes

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
		Remember, these name_space pointers are not owned, but 
			are modifiable.  
	 */
	stack<never_ptr<name_space> >	namespace_stack;
#define current_namespace	namespace_stack.top()

	/**
		Pointer to the current definition that is open for 
		modification, intended for adding items to the body.  
		One pointer is sufficient for all definitions because
		only one definition can be open at a time.  
		Never delete's because the definition has already 
		been registered to some scopespace that owns it.  
		Q: is this made redundant by current_scope?
	 */
	never_ptr<definition_base>	current_open_definition;

	/**
		This pointer is the scratch space for constructing
		definitions' prototypes which will then be added and 
		possibly checked against previous definitions.  
		Intended for adding template formals and port formals.  
		Exclusive-pointer because is freshly constructed.  
	*/
	excl_ptr<definition_base>	current_prototype;

	/**
		Pointer to the current definition referenced, usually
		resolved by the last identifier.  
		May point to any definition for a channel, data-type, 
		process, or even the dummy built-in parameter and data-types.  
		The definition will be combined with optional 
		template parameters to form a type reference (below).  
	 */
	never_const_ptr<definition_base>	current_definition_reference;

	/**
		Pointer to the concrete type to instantiate.  
		Use shared count_ptr until type-cache is implemented.  
	 */
//	never_const_ptr<fundamental_type_reference>
	count_const_ptr<fundamental_type_reference>
						current_fundamental_type;

#if 0
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
	excl_ptr<template_param_list>		current_template_arguments;
#endif

	// need current_blah_inst for array/dimension/range additions
//	const array_dim_list*		current_array_dimensions;


	/**
		Since loop and conditional contexts may be nested, 
		we need to keep track of them in a stack.  
		These pointers are modifiable, but are not owned.  
		Remember to push NULL initially.  
	 */
	stack<never_ptr<scopespace> >	dynamic_scope_stack;
#define	current_dynamic_scope		dynamic_scope_stack.top()

#if 0
// NOT YET
	/**
		Expressions need to be reference counted.  
	 */
	stack<count_ptr<param_expr> >		expr_stack;
#endif

	/**
		UPDATE ME.
		A unified stack intended for instance references and
		parameter expressions.  
		Items need to be modifiable.  
		e.g. when we are initializing reference 
			expressions on the stack.
	 */
	stack<count_ptr<object> >		object_stack;
//	stack<excl_ptr<object> >		object_stack;

public:
	/// The number of semantic errors to accumulate before bailing out.  
	static const long	type_error_limit = 3;

	// optional include short-cut pointers to built-ins
	/**
		Read-only shortcut pointer to the global namespace.
		Can this be modified inadvertently?
	 */
	const never_const_ptr<name_space>	global_namespace;


public:
	context(never_ptr<name_space> g);
	~context();

// TO DO: sort methods by where they are expected to be invoked
//	i.e. from the syntax tree check_build, or from the symbol_table objects

void	open_namespace(const token_identifier& id);
void	close_namespace(void);
void	using_namespace(const qualified_id& id);
void	alias_namespace(const qualified_id& id, const string& a);
never_const_ptr<name_space>	top_namespace(void) const;

never_ptr<definition_base>
	add_declaration(excl_ptr<definition_base> d);

// void	declare_process(const token_identifier& ps);
void	open_process_definition(const token_identifier& ps);
void	close_process_definition(void);

void	declare_datatype(const token_identifier& ds);
void	open_datatype(const token_identifier& ds);
void	close_datatype_definition(void);

// void	declare_enum(const token_identifier& en);
void	open_enum_definition(const token_identifier& en);
bool	add_enum_member(const token_identifier& em);
void	close_enum_definition(void);

void	declare_chantype(const token_identifier& ds);
void	open_chantype(const token_identifier& ds);
void	close_chantype_definition(void);

bool	alias_definition(never_const_ptr<definition_base> d, 
		const token_identifier& id);

void	add_connection(excl_const_ptr<connection_assignment_base> c);
void	add_assignment(excl_const_ptr<connection_assignment_base> a)
		{ add_connection(a); }

never_const_ptr<scopespace>	get_current_scope(void) const;
never_ptr<scopespace>		get_current_scope(void);

never_const_ptr<name_space>
		get_current_namespace(void) const {
			return current_namespace;
		}

#if 0
/**
	Note: non-const because of destructive transfer.
	Returning pointer by reference.  
	This needs work.  
 */
excl_ptr<template_param_list>&
		get_current_template_arguments(void) {
			return current_template_arguments;
		}
#endif

// sets context's definition for instantiation
never_const_ptr<definition_base>	
		get_current_definition_reference(void) const
			{ return current_definition_reference; }

/**
	To do: change prototype to use pointer class.  
 */
never_const_ptr<definition_base>
		set_current_definition_reference(const definition_base& d) {
			current_definition_reference = 
				never_const_ptr<definition_base>(&d);
			return current_definition_reference;
		}

// never_const_ptr<fundamental_type_reference>
count_const_ptr<fundamental_type_reference>
		get_current_fundamental_type(void) const;

never_ptr<definition_base>
		set_current_prototype(excl_ptr<definition_base> d);

/** destructive transfer return */
excl_ptr<definition_base>
	get_current_prototype(void) { return current_prototype; }
never_const_ptr<definition_base>
	get_current_prototype(void) const { return current_prototype; }

never_const_ptr<datatype_definition>
		get_current_datatype_definition(void) const;

// should be called by parser after done using definitions
void	reset_current_definition_reference(void);
void	reset_current_fundamental_type(void);

never_const_ptr<built_in_param_def>
	get_current_param_definition(void) const;

never_const_ptr<channel_definition>
	get_current_channel_definition(void) const;

never_const_ptr<process_definition>
	get_current_process_definition(void) const;

#if 0
// to be called from parser's check_build
never_const_ptr<fundamental_type_reference>
	set_current_fundamental_type(void);
// to be called from symbol_table type_reference_base classes 
never_const_ptr<fundamental_type_reference>
	set_current_fundamental_type(const fundamental_type_reference& tr);
#else
void	set_current_fundamental_type(
		count_const_ptr<fundamental_type_reference> tr);
#endif

#if 0
void	set_current_template_arguments(excl_ptr<template_param_list>& tl);
void	reset_current_template_arguments(void);
#endif

never_const_ptr<object>	lookup_object(const qualified_id& id) const;
never_const_ptr<definition_base>
			lookup_definition(const token_identifier& id) const;
never_const_ptr<definition_base>
			lookup_definition(const qualified_id& id) const;
never_const_ptr<instantiation_base>
			lookup_instance(const token_identifier& id) const;
never_const_ptr<instantiation_base>
			lookup_instance(const qualified_id& id) const;

never_const_ptr<instantiation_base>
			add_instance(const token_identifier& id);
never_const_ptr<instantiation_base>
			add_instance(const token_identifier& id, 
				index_collection_item_ptr_type dim);

never_const_ptr<instantiation_base>	// should be param_instantiation
			add_template_formal(const token_identifier& id, 
				count_const_ptr<param_expr> d);
never_const_ptr<instantiation_base>	// should be param_instantiation
			add_template_formal(const token_identifier& id, 
				index_collection_item_ptr_type dim, 
				count_const_ptr<param_expr> d);

never_const_ptr<instantiation_base>
			add_port_formal(const token_identifier& id);
never_const_ptr<instantiation_base>
			add_port_formal(const token_identifier& id, 
				index_collection_item_ptr_type dim);

#if 1
void	push_object_stack(count_ptr<object> i);
count_ptr<object> pop_top_object_stack(void);
#else
void	push_object_stack(excl_ptr<object> i);
excl_ptr<object> pop_top_object_stack(void);
#endif

// repeat for processes and channels...

string		auto_indent(void) const;

private:
void close_current_definition(void);
};	// end class context

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_SYMBOL_TABLE_H__

