/**
	\file "AST/parse_context.h"
	Context class for traversing syntax tree, type-checking, 
	and constructing persistent objects.  
	This file came from "Object/art_context.h" in a previous life.  
	$Id: parse_context.h,v 1.10 2006/10/18 01:19:00 fang Exp $
 */

#ifndef __AST_PARSE_CONTEXT_H__
#define __AST_PARSE_CONTEXT_H__

#include "util/string_fwd.h"
#include <stack>
#include <list>

#include "util/STL/list_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "Object/common/util_types.h"
#include "Object/devel_switches.h"
#include "util/boolean_types.h"
#include "util/attributes.h"

namespace HAC {
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
#if USE_INSTANCE_PLACEHOLDERS
	class instance_placeholder_base;
	template <class> class value_placeholder;
#else
	class instance_collection_base;
#endif
	class instance_management_base;
	class meta_instance_reference_connection;
	class param_expr;
#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
	class dynamic_param_expr_list;
#else
	class param_expr_list;
#endif
	class param_expression_assignment;
	class loop_scope;
	class conditional_scope;
	struct pint_tag;
#if !USE_INSTANCE_PLACEHOLDERS
	template <class, size_t> class value_array;
#endif
namespace PRS {
	class rule;
	class rule_set;
}
}	// end namespace entity

namespace parser {
//=============================================================================
// forward declarations outside this namespace
using std::ostream;
using std::stack;
using std::string;
using std::list;
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
#if USE_INSTANCE_PLACEHOLDERS
using entity::instance_placeholder_base;
using entity::value_placeholder;
#else
using entity::instance_collection_base;
using entity::value_array;
#endif
using entity::instance_management_base;
using entity::meta_instance_reference_connection;
using entity::param_expr;
#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
using entity::dynamic_param_expr_list;
#else
using entity::param_expr_list;
#endif
using entity::param_expression_assignment;
using entity::index_collection_item_ptr_type;
using entity::pint_tag;
using entity::loop_scope;
using entity::conditional_scope;

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
#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
	typedef	count_ptr<const dynamic_param_expr_list>
							relaxed_args_ptr_type;
#else
	typedef	count_ptr<const param_expr_list>	relaxed_args_ptr_type;
#endif
	typedef list<string>			file_name_stack_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	never_ptr<const instance_placeholder_base>
#else
	typedef	never_ptr<const instance_collection_base>
#endif
						placeholder_ptr_type;
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

	/**
		Pointer to the current definition that is open for 
		modification, intended for adding items to the body.  
		One pointer is sufficient for all definitions because
		only one definition can be open at a time.  
		Until now! (2006-10-07)
		Never delete's because the definition has already 
		been registered to some scopespace that owns it.  
		Q: is this made redundant by current_scope?
	 */
#if SUPPORT_NESTED_DEFINITIONS
	stack<never_ptr<definition_base> >	open_definition_stack;
#else
	never_ptr<definition_base>	current_open_definition;
#endif

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

	/**
		TODO: should just contain reference to module...
	 */
	entity::PRS::rule_set&			top_prs;

#if USE_INSTANCE_PLACEHOLDERS
	typedef	value_placeholder<pint_tag>	loop_var_placeholder_type;
	typedef	list<count_ptr<const loop_var_placeholder_type> >
#else
	typedef	value_array<pint_tag, 0>	pint_scalar;
	typedef	pint_scalar			loop_var_placeholder_type;
	typedef	list<count_ptr<const pint_scalar> >
#endif
						loop_var_stack_type;
	loop_var_stack_type			loop_var_stack;

	/**
		File name stack for diagnostics.  
	 */
	file_name_stack_type			file_name_stack;
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
		Stupid implementation of switching between
		strict and relaxed template parameters. 
		This flag is manipulated by the methods:
			strict_template_parameters(), 
			relaxed_template_parameters().
		This flag is read by add_template_formals().  
	 */
	bool					strict_template_mode;

	/**
		Whether or not current scope is in some conditional body.
	 */
	bool					in_conditional_scope;

	/**
		This turns off accessibility checks in all contexts, 
		which is useful for other tools that need to read 
		the pre-compiled objects.  
		Default: false, enforcing port visibility only.  
	 */
	bool					view_all_publicly;

public:
	explicit
	context(module& m);

	explicit
	context(const module& m, const bool _public);

private:
	// private undefined copy-constructor
	context(const context&);

public:
	~context();

// TO DO: sort methods by where they are expected to be invoked
//	i.e. from the syntax tree check_build, or from the symbol_table objects

	/**
		Class object for automatically balancing namespace stack.  
	 */
	class namespace_frame {
		context& 		_context;
	public:
		namespace_frame(context&, const token_identifier&);
		~namespace_frame();
	} __ATTRIBUTE_UNUSED__;

private:
	void
	open_namespace(const token_identifier& id);

	void
	close_namespace(void);

public:
	void
	using_namespace(const qualified_id& id);

	void
	alias_namespace(const qualified_id& id, const string& a);

	never_ptr<const name_space>
	top_namespace(void) const;

	never_ptr<definition_base>
	add_declaration(excl_ptr<definition_base>& d);

	template <class DefType>
	class definition_frame {
		context&			_context;
	public:
		definition_frame(context&, const token_identifier&);
		~definition_frame();
	} __ATTRIBUTE_UNUSED__;

	class enum_definition_frame {
		context&			_context;
	public:
		enum_definition_frame(context&, const token_identifier&);
		~enum_definition_frame();
	} __ATTRIBUTE_UNUSED__;

private:
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

public:
	void
	declare_datatype(const token_identifier& ds);

	good_bool
	add_enum_member(const token_identifier& em);

	void
	declare_chantype(const token_identifier& ds);

	good_bool
	alias_definition(const never_ptr<const definition_base> d, 
		const token_identifier& id);

	void
	add_connection(
#if REF_COUNT_INSTANCE_MANAGEMENT
		const count_ptr<const meta_instance_reference_connection>&
#else
		excl_ptr<const meta_instance_reference_connection>& c
#endif
		);

	void
	add_assignment(
#if REF_COUNT_INSTANCE_MANAGEMENT
		const count_ptr<const param_expression_assignment>&
#else
		excl_ptr<const param_expression_assignment>& a
#endif
		);

	never_ptr<sequential_scope>
	get_current_sequential_scope(void) const {
		return sequential_scope_stack.top();
	}

	never_ptr<const scopespace>
	get_current_named_scope(void) const;

	never_ptr<scopespace>
	get_current_named_scope(void);

	never_ptr<const name_space>
	get_current_namespace(void) const
		{ return namespace_stack.top(); }

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
#if SUPPORT_NESTED_DEFINITIONS
		return open_definition_stack.top();
#else
		return current_open_definition;
#endif
	}

	/**
		Manages current type of instantiation on stack.  
	 */
	class fundamental_type_frame {
		context&		_context;
	public:
		fundamental_type_frame(context&,
			const count_ptr<const fundamental_type_reference>&);
		~fundamental_type_frame();
	} __ATTRIBUTE_UNUSED__;

private:
	void
	set_current_fundamental_type(
		const count_ptr<const fundamental_type_reference>&);

	void
	reset_current_fundamental_type(void);

public:
// never_ptr<const fundamental_type_reference>
	count_ptr<const fundamental_type_reference>
	get_current_fundamental_type(void) const;

	never_ptr<const object>
	lookup_object(const qualified_id& id) const;

	never_ptr<const object>
	lookup_object(const token_identifier& id) const;

// TODO: template the lookup methods.  

	never_ptr<const definition_base>
	lookup_definition(const token_identifier& id) const;

	never_ptr<const definition_base>
	lookup_definition(const qualified_id& id) const;

	placeholder_ptr_type
	lookup_instance(const token_identifier& id) const;

	placeholder_ptr_type
	lookup_instance(const qualified_id& id) const;

	placeholder_ptr_type
	add_instance(const token_identifier& id, 
		const relaxed_args_ptr_type&);

	placeholder_ptr_type
	add_instance(const token_identifier& id, 
		const relaxed_args_ptr_type&, 
		index_collection_item_ptr_type dim);

	// should be param_value_placeholder
	placeholder_ptr_type
	add_template_formal(const token_identifier& id, 
		count_ptr<const param_expr> d);

	// should be param_value_placeholder
	placeholder_ptr_type
	add_template_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim, 
		const count_ptr<const param_expr>& d);

	// port formals are not allowed to have instance-relaxed actuals.  
	// should be physical_instance_placeholder
	placeholder_ptr_type
	add_port_formal(const token_identifier& id);

	// port formals are not allowed to have instance-relaxed actuals.  
	placeholder_ptr_type
	add_port_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim);

	void
	commit_definition_arity(void);

	void
	add_top_level_production_rule(excl_ptr<entity::PRS::rule>&);

private:
	// TODO:
	// use nested struct for automatic construction/destruction matching...
	count_ptr<loop_var_placeholder_type>
	push_loop_var(const token_identifier&);

	void
	pop_loop_var(void);

public:
	/**
		Automatic loop-variable stack manager.  
		Pushes onto stack during construction.  
		Pops off stack at destruction time.  
		Used for PRS rule-loops and expr-loops.  
	 */
	struct loop_var_frame {
		context&			_context;
		count_ptr<loop_var_placeholder_type>	var;
		loop_var_frame(context&, const token_identifier&);
		~loop_var_frame();
	} __ATTRIBUTE_UNUSED__;	// end struct loop_var_frame


	/**
		Loop scope frame for unroll-related instance management.
		Used for top-level scopes and definition scopes.  
	 */
	struct loop_scope_frame {
		context&			_context;
#if REF_COUNT_INSTANCE_MANAGEMENT
		loop_scope_frame(context&, const count_ptr<loop_scope>&);
#else
		loop_scope_frame(context&, excl_ptr<loop_scope>&);
#endif
		~loop_scope_frame();
	} __ATTRIBUTE_UNUSED__;

	/**
		Conditional scope frame for unroll-related instance management.
	 */
	struct conditional_scope_frame {
		context&			_context;
		const bool			parent_cond;
#if REF_COUNT_INSTANCE_MANAGEMENT
		conditional_scope_frame(context&, 
			const count_ptr<conditional_scope>&);
#else
		conditional_scope_frame(context&, excl_ptr<conditional_scope>&);
#endif
		~conditional_scope_frame();
	} __ATTRIBUTE_UNUSED__;

	struct file_stack_frame {
		context&			_context;
		file_stack_frame(context&, const string&);
		~file_stack_frame();
	} __ATTRIBUTE_UNUSED__;

	ostream&
	dump_file_stack(ostream&) const;

	string
	auto_indent(void) const;

	void
	strict_template_parameters(void) { strict_template_mode = true; }

	void
	relaxed_template_parameters(void) { strict_template_mode = false; }

	bool
	is_publicly_viewable(void) const { return view_all_publicly; }

private:
	void
	close_current_definition(void);
};	// end class context

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_PARSE_CONTEXT_H__

