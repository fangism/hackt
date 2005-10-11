/**
	\file "AST/parse_context.h"
	Context class for traversing syntax tree, type-checking, 
	and constructing persistent objects.  
	This file came from "Object/art_context.h" in a previous life.  
	$Id: parse_context.h,v 1.3.8.2 2005/10/11 02:41:24 fang Exp $
 */

#ifndef __AST_PARSE_CONTEXT_H__
#define __AST_PARSE_CONTEXT_H__

#include "util/string_fwd.h"
#include <stack>

#include "util/STL/list.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "Object/common/util_types.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
	// forward declarations
	class object;
	class module;
	class scopespace;
	class name_space;
	class definition_base;
	class datatype_definition_base;
	class channel_definition_base;
	class process_definition_base;
	class fundamental_type_reference;
	class sequential_scope;
	class instance_collection_base;
	class instance_management_base;
	class meta_instance_reference_connection;
	class param_expr;
	class param_expr_list;
	class param_expression_assignment;
	struct pint_tag;
	template <class, size_t> class value_array;
}	// end namespace entity

namespace parser {
//=============================================================================
// forward declarations outside this namespace
using std::ostream;
using std::stack;
using std::string;
USING_LIST
using util::good_bool;
using util::bad_bool;
using util::memory::count_ptr;
using util::memory::never_ptr;
using util::memory::excl_ptr;
using util::memory::sticky_ptr;
using entity::object;
using entity::module;
using entity::scopespace;
using entity::name_space;
using entity::definition_base;
using entity::datatype_definition_base;
using entity::channel_definition_base;
using entity::process_definition_base;
using entity::fundamental_type_reference;
using entity::sequential_scope;
using entity::instance_collection_base;
using entity::instance_management_base;
using entity::meta_instance_reference_connection;
using entity::param_expr;
using entity::param_expr_list;
using entity::param_expression_assignment;
using entity::index_collection_item_ptr_type;
using entity::pint_tag;
using entity::value_array;

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

	IDEA: instead of push pop-like operations, 
	we can make nested classes to manipulate the stack automatically
	upon construction and destruction.  

	TODO: replace indent with util/indent implementation.  
	TODO: rename this parse_context.
	TODO: real error manager...
 */
class context {
public:
	typedef	count_ptr<const param_expr_list>	relaxed_args_ptr_type;
private:
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

	typedef	value_array<pint_tag, 0>	pint_scalar;
	typedef	list<count_ptr<const pint_scalar> >
						loop_var_stack_type;
	loop_var_stack_type			loop_var_stack;

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


private:
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

	/**
		Stupid implementation of switching between
		strict and relaxed template parameters. 
		This flag is manipulated by the methods:
			strict_template_parameters(), 
			relaxed_template_parameters().
		This flag is read by add_template_formals().  
	 */
	bool					strict_template_mode;

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
	add_declaration(excl_ptr<definition_base>& d);

// void	declare_process(const token_identifier& ps);
	template <class D>
	void
	open_definition(const token_identifier& ps);

	template <class D>
	void
	close_definition(void);

	// different: not sequential scopes
	void
	open_enum_definition(const token_identifier& en);

	void
	close_enum_definition(void);

	void
	declare_datatype(const token_identifier& ds);

// void	declare_enum(const token_identifier& en);
	good_bool
	add_enum_member(const token_identifier& em);

	void
	declare_chantype(const token_identifier& ds);

	good_bool
	alias_definition(const never_ptr<const definition_base> d, 
		const token_identifier& id);

	void
	add_connection(excl_ptr<const meta_instance_reference_connection>& c);

	void
	add_assignment(excl_ptr<const param_expression_assignment>& a);

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
	get_current_namespace(void) const
		{ return current_namespace; }

	never_ptr<definition_base>
	set_current_prototype(excl_ptr<definition_base>& d);
// void	reset_current_prototype(void);

/** destructive transfer return */
	excl_ptr<definition_base>&
	get_current_prototype(void);

	never_ptr<const definition_base>
	get_current_prototype(void) const;

	template <class D>
	never_ptr<const D>
	get_current_definition(void) const;

	never_ptr<definition_base>
	get_current_open_definition(void) const {
		return current_open_definition;
	}

	void
	set_current_fundamental_type(
		const count_ptr<const fundamental_type_reference>&);

// never_ptr<const fundamental_type_reference>
	count_ptr<const fundamental_type_reference>
	get_current_fundamental_type(void) const;

	void
	reset_current_fundamental_type(void);

	never_ptr<const object>
	lookup_object(const qualified_id& id) const;

	never_ptr<const object>
	lookup_object(const token_identifier& id) const;

// TODO: template the lookup methods.  

	never_ptr<const definition_base>
	lookup_definition(const token_identifier& id) const;

	never_ptr<const definition_base>
	lookup_definition(const qualified_id& id) const;

	never_ptr<const instance_collection_base>
	lookup_instance(const token_identifier& id) const;

	never_ptr<const instance_collection_base>
	lookup_instance(const qualified_id& id) const;

	never_ptr<const instance_collection_base>
	add_instance(const token_identifier& id, 
		const relaxed_args_ptr_type&);

	never_ptr<const instance_collection_base>
	add_instance(const token_identifier& id, 
		const relaxed_args_ptr_type&, 
		index_collection_item_ptr_type dim);

	// should be param_value_collection
	never_ptr<const instance_collection_base>
	add_template_formal(const token_identifier& id, 
		count_ptr<const param_expr> d);

	// should be param_value_collection
	never_ptr<const instance_collection_base>
	add_template_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim, 
		const count_ptr<const param_expr>& d);

	// port formals are not allowed to have instance-relaxed actuals.  
	never_ptr<const instance_collection_base>
	add_port_formal(const token_identifier& id);

	// port formals are not allowed to have instance-relaxed actuals.  
	never_ptr<const instance_collection_base>
	add_port_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim);

	void
	commit_definition_arity(void);

	// TODO:
	// use nested struct for automatic construction/destruction matching...
	count_ptr<pint_scalar>
	push_loop_var(const token_identifier&);

	void
	pop_loop_var(void);

// repeat for processes and channels...

	string
	auto_indent(void) const;

	void
	strict_template_parameters(void) { strict_template_mode = true; }

	void
	relaxed_template_parameters(void) { strict_template_mode = false; }

private:
	void
	close_current_definition(void);
};	// end class context

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_PARSE_CONTEXT_H__

