/**
	\file "art_context.h"
	Context class for traversing syntax tree, type-checking, 
	and constructing persistent objects.  
	$Id: art_context.h,v 1.11 2005/01/06 17:44:52 fang Exp $
 */

#ifndef __ART_CONTEXT_H__
#define __ART_CONTEXT_H__

#include <stdlib.h>
#include <string>
#include <stack>

#include "STL/list.h"

#include "memory/pointer_classes.h"

#include "art_object_fwd.h"

namespace ART {

//=============================================================================
using namespace entity;

namespace parser {
//=============================================================================
using std::ostream;
using std::stack;
using std::string;
USING_LIST
using namespace util::memory;		// for pointer classes

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
	stack<never_ptr<const definition_base> >	definition_stack;
#define	current_definition_reference		definition_stack.top()

	/**
		Pointer to the concrete type to instantiate.  
		Use shared count_ptr until type-cache is implemented.  
	 */
	count_ptr<const fundamental_type_reference>
						current_fundamental_type;


	/**
		Since loop and conditional contexts may be nested, 
		we need to keep track of them in a stack.  
		These pointers are modifiable, but are not owned.  
		Remember to push NULL initially.  
	 */
	stack<never_ptr<sequential_scope> >	sequential_scope_stack;
#define	current_sequential_scope		sequential_scope_stack.top()

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

public:
	/// The number of semantic errors to accumulate before bailing out.  
	static const long	type_error_limit = 3;

	// optional include short-cut pointers to built-ins
	/**
		Read-only shortcut pointer to the global namespace.
		Can this be modified inadvertently?
		Need to be modifiable to access ordered lists...
	 */
	const never_ptr<name_space>		global_namespace;
//	const never_ptr<const name_space>	global_namespace;

protected:
	/**
		Consider a stack of contexts for instance_management_lists.
		Push/pop when entering/leaving definition or
		control scope.  
		sequential_scope_stack?
		Namespace change doesn't affect this stack!
		The globally ordered (master) instance management list
		should be at the bottom of the stack.  
		Maintain multiple stacks? or unified stack?

		This corresponds to the module class's 
		globally ordered instance management list.  
		This is where all globally ordered actions go.  
	 */
	// sequential_scope::instance_management_list_type&
	list<sticky_ptr<const instance_management_base> >&
						master_instance_list;

public:
	context(module& m);
	~context();

// TO DO: sort methods by where they are expected to be invoked
//	i.e. from the syntax tree check_build, or from the symbol_table objects

	void
	open_namespace(const token_identifier& id);

	void
	close_namespace(void);

	void
	using_namespace(const qualified_id& id);

	void
	alias_namespace(const qualified_id& id, const string& a);

	never_ptr<const name_space>
	top_namespace(void) const;

	never_ptr<definition_base>
	add_declaration(excl_ptr<definition_base> d);

// void	declare_process(const token_identifier& ps);
	void
	open_process_definition(const token_identifier& ps);

	void
	close_process_definition(void);

	void
	declare_datatype(const token_identifier& ds);
	void
	open_datatype(const token_identifier& ds);

	void
	close_datatype_definition(void);

// void	declare_enum(const token_identifier& en);
	void
	open_enum_definition(const token_identifier& en);

	bool
	add_enum_member(const token_identifier& em);

	void
	close_enum_definition(void);

	void
	declare_chantype(const token_identifier& ds);

	void
	open_chantype(const token_identifier& ds);

	void
	close_chantype_definition(void);

	bool
	alias_definition(never_ptr<const definition_base> d, 
		const token_identifier& id);

	void
	add_connection(excl_ptr<const instance_reference_connection> c);

	void
	add_assignment(excl_ptr<const param_expression_assignment> a);

/**
	Need to make distinctions:
	call this get_current_named_scope.
	Make another get_current_sequential_scope.
 */
	never_ptr<const scopespace>
	get_current_named_scope(void) const;

	never_ptr<scopespace>
	get_current_named_scope(void);

	never_ptr<const name_space>
	get_current_namespace(void) const { return current_namespace; }

// sets context's definition for instantiation, or for member lookup
	never_ptr<const definition_base>	
	get_current_definition_reference(void) const
		{ return current_definition_reference; }

// pointer instead of reference?
	never_ptr<const definition_base>
	push_current_definition_reference(const definition_base& d) {
		definition_stack.push(
			never_ptr<const definition_base>(&d));
		return current_definition_reference;
	}

// never_ptr<const fundamental_type_reference>
	count_ptr<const fundamental_type_reference>
	get_current_fundamental_type(void) const;

	never_ptr<definition_base>
	set_current_prototype(excl_ptr<definition_base> d);
// void	reset_current_prototype(void);

/** destructive transfer return */
	excl_ptr<definition_base>
	get_current_prototype(void);

	never_ptr<const definition_base>
	get_current_prototype(void) const;

	never_ptr<const datatype_definition_base>
	get_current_datatype_definition(void) const;

// should be called by parser after done using definitions
	void
	pop_current_definition_reference(void);

	void
	reset_current_fundamental_type(void);

	never_ptr<const built_in_param_def>
	get_current_param_definition(void) const;

	never_ptr<const channel_definition_base>
	get_current_channel_definition(void) const;

	never_ptr<const process_definition_base>
	get_current_process_definition(void) const;

	void
	set_current_fundamental_type(
		count_ptr<const fundamental_type_reference> tr);

	never_ptr<const object>
	lookup_object(const qualified_id& id) const;

	never_ptr<const definition_base>
	lookup_definition(const token_identifier& id) const;

	never_ptr<const definition_base>
	lookup_definition(const qualified_id& id) const;

	never_ptr<const instance_collection_base>
	lookup_instance(const token_identifier& id) const;

	never_ptr<const instance_collection_base>
	lookup_instance(const qualified_id& id) const;

	never_ptr<const instance_collection_base>
	add_instance(const token_identifier& id);

	never_ptr<const instance_collection_base>
	add_instance(const token_identifier& id, 
		index_collection_item_ptr_type dim);

	// should be param_instance_collection
	never_ptr<const instance_collection_base>
	add_template_formal(const token_identifier& id, 
		count_ptr<const param_expr> d);

	// should be param_instance_collection
	never_ptr<const instance_collection_base>
	add_template_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim, 
		count_ptr<const param_expr> d);

	never_ptr<const instance_collection_base>
	add_port_formal(const token_identifier& id);

	never_ptr<const instance_collection_base>
	add_port_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim);

	void
	push_object_stack(count_ptr<object> i);

	count_ptr<object>
	pop_top_object_stack(void);

// repeat for processes and channels...

	string
	auto_indent(void) const;

private:
	void
	close_current_definition(void);
};	// end class context

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_CONTEXT_H__

