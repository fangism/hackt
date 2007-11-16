/**
	\file "AST/parse_context.cc"
	Class methods for context object passed around during 
	type-checking, and object construction.  
	This file was "Object/art_context.cc" in a previous life.  
 	$Id: parse_context.cc,v 1.22.2.2 2007/11/16 04:21:53 fang Exp $
 */

#ifndef	__AST_PARSE_CONTEXT_CC__
#define	__AST_PARSE_CONTEXT_CC__

#define ENABLE_STACKTRACE		0

#include <cassert>
#include <exception>
#include <iostream>

#include "AST/parse_context.h"
#include "AST/parse_context.tcc"
#include "AST/token_string.h"
#include "AST/identifier.h"
#include "Object/expr/meta_range_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/process_definition.h"
#include "Object/type/param_type_reference.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/expression_assignment.h"
#include "Object/unroll/alias_connection.h"
#include "Object/unroll/loop_scope.h"
#include "Object/unroll/conditional_scope.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/collection_fwd.h"
#include "Object/inst/dummy_placeholder.h"
#include "Object/traits/node_traits.h"
#include "Object/module.h"

#include "common/ICE.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"
using entity::object_handle;
using entity::enum_datatype_def;
using entity::instantiation_statement_base;
using entity::physical_instance_placeholder;
using entity::param_value_placeholder;
using entity::param_type_reference;
using entity::process_definition;
using entity::user_def_chan;
using entity::user_def_datatype;
using entity::pint_scalar;
using entity::expr_dump_context;

//=============================================================================
// class context method definition

/**
	Default context constructor.  Should really only be invoked
	once (per source file or module).  
	This constructor is used when constructing a module during compilation.
	Creates the global namespace and initializes it with
	built-in types.  
	\param g pointer to global namespace.
 */
context::context(module& m, const parse_options& o) :
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		namespace_stack(), 
		open_definition_stack(), 
		current_prototype(NULL), 
		current_fundamental_type(NULL), 
		sequential_scope_stack(), 
		loop_var_stack(), 
		global_namespace(m.get_global_namespace()), 
		current_prs_body(&m.get_prs()),
		strict_template_mode(true), 
		in_conditional_scope(false), 
		view_all_publicly(false), 
		parse_opts(o)
		{

	// perhaps verify that g is indeed global?  can't be any namespace
	namespace_stack.push(global_namespace);
	// now using top-level as a process definition!
	open_definition_stack.push(never_ptr<definition_base>(&m));
	// remember that the creator of the global namespace is responsible
	// for deleting it.  
	sequential_scope_stack.push(never_ptr<sequential_scope>(&m));

	// "current_namespace" is macro-defined to namespace_stack.top()
	NEVER_NULL(get_current_namespace());	// make sure allocated properly
	NEVER_NULL(global_namespace);	// same pointer

	// Q: should built-ins be in a super namespace about the globals?
	// some static global set?  shared among all objects?
	// thinking of merging object hierarchies in the future for linking...
	// want to avoid multiple definitions in the global namespace.  
	// A: built-ins have been re-implemented in "art_built_ins.*"

}	// end of context constructor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A read-only-module constructor for checking without modifying.
	This is used for tools that only need to read the module.  
	\param m the precompiled module to attach.  
		NOTE: really, only the namespace is required from the module.  
	\param _pub pass true to be able to view all members publicly, 
		lifting port-visibility restriction.  
 */
context::context(const module& m, const parse_options& o, const bool _pub) :
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		namespace_stack(), 
		open_definition_stack(), 
		current_prototype(NULL), 
		current_fundamental_type(NULL), 
		sequential_scope_stack(), 
		loop_var_stack(), 
		global_namespace(m.get_global_namespace()), 
		current_prs_body(NULL),	// not adding any PRS
		strict_template_mode(true), 
		in_conditional_scope(false), 
		view_all_publicly(_pub), 
		parse_opts(o)
		{
	namespace_stack.push(global_namespace);
	// now using top-level as a process definition!
	// load NULL because this is a read-only use of the module
	open_definition_stack.push(never_ptr<definition_base>(NULL));
	// NOTE: we don't bother loading the module's sequential scope
	// because it should not be used in a read-only context.  
	NEVER_NULL(get_current_namespace());	// make sure allocated properly
	NEVER_NULL(global_namespace);	// same pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Context destructor.  Need not explicitly clear structure members, 
	as their default destructors are automatically invoked.  
	Reminder, that the global namespace in the namespace_stack is not owned
	by this context, and thus should not be deleted.  
 */
context::~context() {
//	namespace_stack.pop();			// redundant
// possible sanity check: the remaining namespace is the global
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attempts to open up namespace, modifying the context, 
	and pushes the new namespace onto namespace_stack.
	Error occurs if name conflicts, as defined by the implementation
	within add_open_namespace.  
	\param id is the name of the namespace to enter.  
	\return pointer to opened namespace.
	\sa close_namespace
 */
void
context::open_namespace(const token_identifier& id) {
#if 0
	cerr << "In context::open_namespace(\"" << id << "\"):" << endl;
	cerr << "Before add_open_namespace(), " << endl;
	current_namespace->dump(cerr) << endl;
#endif
	const never_ptr<name_space>
		insub(namespace_stack.top()->add_open_namespace(id));

#if 0
	cerr << "After add_open_namespace(), " << endl;
	get_current_namespace()->dump(cerr) << endl;
#endif

	// caution: assigning to NULL may ruin the context!
	// if this returns NULL, we signal to the caller to skip
	// over this malformed namespace body, don't even enter it...
	// which means no need to leave it.  

	if (insub) {
		namespace_stack.push(insub);
		indent++;
	} else {
		// leave current_namespace as it is
		type_error_count++;
		cerr << where(id) << endl;
		THROW_EXIT;			// temporary
		// return NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes the current namespace, leaving the scope.  
	Also pops the namespace off of the stack.  
	\sa open_namespace
 */
void
context::close_namespace(void) {
	never_ptr<const name_space>
		new_top(namespace_stack.top()->leave_namespace());
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	namespace_stack.pop();
	INVARIANT(get_current_namespace() == new_top);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	current_open_definition can be NULL when module is opened in
	read-only mode, in which case we say we are at the top-level.  
 */
bool
context::at_top_level(void) const {
	const never_ptr<const definition_base> d(get_current_open_definition());
	return !d || d.is_a<const module>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if in a definition scope or not in global namespace.
	NOTE: this is not affected by parse_opts.namespace_instances
 */
bool
context::in_nonglobal_namespace(void) const {
	const never_ptr<const definition_base> d(get_current_open_definition());
	NEVER_NULL(d);	// since top-level module is a process_definition
	return d.is_a<const module>() &&	// is a non-module definition
		(get_current_namespace() != global_namespace);
		// else in non-global namespace
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if not in a definition scope and not in global namespace.
	NOTE: this is not affected by parse_opts.namespace_instances
 */
bool
context::reject_namespace_lang_body(void) const {
	const never_ptr<const definition_base> d(get_current_open_definition());
	NEVER_NULL(d);	// since top-level module is a process_definition
	return d.is_a<const module>() &&	// is a non-module definition
		(get_current_namespace() != global_namespace);
		// else in non-global namespace
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if trying to add an instance to a non-global namespace.
	This check can be disabled by parse_opts.namespace_instances.
 */
bool
context::reject_nonglobal_instance_management(void) const {
	const bool ret =
		!parse_opts.namespace_instances && in_nonglobal_namespace();
	if (ret) {
cerr << "Error: instance management statements are forbidden "
	"in non-global namespaces outside definitions." << endl;
cerr << "To allow them, pass -f namespace-instances to the compiler." << endl;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a using namespace directive, adds namespace to search list.
	Currently exits on error.  
	\param id qualified identifier to add to namespace.  
 */
void
context::using_namespace(const qualified_id& id) {
	never_ptr<const name_space> ret =
		namespace_stack.top()->add_using_directive(id);
	if (!ret) {
		type_error_count++;
		cerr << where(id) << endl;
		THROW_EXIT;			// temporary
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a using namespace directive under a different local name.
	\param id the referenced namespace.
	\param a the local alias.  
 */
void
context::alias_namespace(const qualified_id& id, const string& a) {
	const never_ptr<const name_space>
		ret(namespace_stack.top()->add_using_alias(id, a));
	if (!ret) {
		type_error_count++;
		cerr << where(id) << endl;
		THROW_EXIT;			// temporary
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Peeks at the top of the namespace stack.  
	\return pointer at the top of the namespace stack.  
 */
never_ptr<const name_space>
context::top_namespace(void) const {
	return get_current_namespace();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a declaration and checks against any previous 
	prototype with the same name.  
	Caller should check the return value if there is an error to report.  
 */
never_ptr<definition_base>
context::add_declaration(excl_ptr<definition_base>& d) {
	// careful, passing by reference may breaks some invariant!
	const never_ptr<definition_base>
		ret(namespace_stack.top()->add_definition(d));
	if (!ret) {
		// something went wrong
		type_error_count++;
		// additional error message isn't really necessary...
//		cerr << endl << "ERROR in context::add_declaration().  ";
//		cerr << where(*d) << endl;	// caller will do
		// For now, we intentionally delete the bad definition, 
		// though later, it may be useful for precise error messages.
		INVARIANT(d);
		const excl_ptr<definition_base> delete_d(d);
		INVARIANT(!d);
	} else {
		// should transfer ownership, unless there is an error
		INVARIANT(!d);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Q: GENERIC open_definition?  in place of type-specific?
	Looks up enum definition, and returns modifable pointer, 
		sets current_open_definition.
	\param es the enum signature, which contains and identifier, 
		optional template signature, and port signature.  
	Details: checks to see if prototype was already declared.  
	If already declared, then this re-declaration MUST be indentical, 
	else report error of mismatched re-declaration.  
	If not already declared, create an entry...
 */
void
context::open_enum_definition(const token_identifier& ename) {
	const never_ptr<enum_datatype_def>
		ed(get_current_namespace()->lookup_member_with_modify(ename)
				.is_a<enum_datatype_def>());
	if (ed) {
		if (ed->is_defined()) {
			cerr << ename << " is already defined!  attempted "
				"redefinition at " << where(ename) << endl;
			THROW_EXIT;
		}
		open_definition_stack.push(ed);
		ed->mark_defined();
		indent++;
	} else {
		// no real reason why this should ever fail...
		type_error_count++;
		cerr << where(ename) << endl;
		THROW_EXIT;			// temporary
		// return NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Adds an enumeration named member to the current enumeration
	definition.  
	\param em name of the enumeration member.  
	\return true if successful, false if there was conflict.  
 */
good_bool
context::add_enum_member(const token_identifier& em) {
	const never_ptr<enum_datatype_def>
		ed(get_current_open_definition().is_a<enum_datatype_def>());
	if (!ed) {
		cerr << "expected current_open_definition to be "
			"enum_datatype_def!  FATAL ERROR." << endl;
		THROW_EXIT;
	} else if (ed->add_member(em)) {
		return good_bool(true);
	} else {
		cerr << "enum " << ed->get_name() << " already has a member "
			"named " << em << ".  ERROR! " << where(em) << endl;
		type_error_count++;
		THROW_EXIT;
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the current open (modifying) definition.  
 */
void
context::close_enum_definition(void) {
	open_definition_stack.pop();
	indent--;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just sets current_open_definition to NULL, 
	and resets the check_against_previous_definition_signature flag.  
	Doesn't check what pointer is before nullifying.  
 */
inline
void
context::close_current_definition(void) {
	open_definition_stack.pop();
	indent--;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return destructive transfer of active definition (modifiable).  
 */
excl_ptr<definition_base>&
context::get_current_prototype(void) {
	return current_prototype;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to current active definition.  
 */
never_ptr<const definition_base>
context::get_current_prototype(void) const { return current_prototype; }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the current type to instantiate to NULL.
	TODO: Also resets template arguments.  
 */
void
context::reset_current_fundamental_type(void) {
	if (current_fundamental_type) {
		indent--;
		current_fundamental_type =
			count_ptr<const fundamental_type_reference>(NULL);
	} else {
		cerr << "warning: current_fundamental_type was already NULL."
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the current definition being constructed (activate).  
	\param d owned pointer to the constructed definition.  
	\return non-owned pointer to the constructed definition.  
 */
never_ptr<definition_base>
context::set_current_prototype(excl_ptr<definition_base>& d) {
	INVARIANT(!current_prototype);
	current_prototype = d;
	return current_prototype;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given a valid complete type reference, set the context to use it.  
	\param tr the expanded type reference to be used for 
		subsequent instantiations.  
 */
void
context::set_current_fundamental_type(
		const count_ptr<const fundamental_type_reference>& tr) {
	INVARIANT(!current_fundamental_type);
	current_fundamental_type = tr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Query the fundamental type to instantiate.  
	\return the current type used for instantiations.  
 */
count_ptr<const fundamental_type_reference>
context::get_current_fundamental_type(void) const {
	return current_fundamental_type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a qualified identifier.  
	Incidentally, never looks up loop variables this way, 
		because they cannot be referenced qualified-ly.  
	Is this comment stale?
	TO FIX: when checking a prototype, current scope should be 
	updated to that prototype's definition.  
 */
never_ptr<const object>
context::lookup_object(const qualified_id& id) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE_VERBOSE;
	// automatically resolve object handles.  
	return_type o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = return_type(&o->self());
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: 2006-09-01
	Need to rewrite loop-scope stuff, using placeholders.  

	Looks up an unqualified identifier.
	Also checks loop contexts with unualified reference.  
 */
never_ptr<const object>
context::lookup_object(const token_identifier& id) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE_VERBOSE;
	// aww shit, have to return a never_ptr when lookup count_ptr...
	// could spell trouble later...
	// fortunately, is only used locally by caller
	typedef	loop_var_stack_type::const_iterator const_iterator;
	const_iterator i(loop_var_stack.begin());
	const const_iterator e(loop_var_stack.end());
	for ( ; i!=e; i++) {
		if ((*i)->get_name() == id)
			return return_type(&**i);
	}
	// automatically resolve object handles.  
	return_type o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = return_type(&o->self());
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a definition alias to the current scope, be it definition or
	namespace.  
	\param d the definition, never owned because the actual 
		definition is already owned in some namespace.  
	\param a the local name to alias the definition.  
	\return true if successful (no collisions).  
 */
good_bool
context::alias_definition(const never_ptr<const definition_base> d,
		const token_identifier& a) {
	return get_current_named_scope()->add_definition_alias(d, a);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
context::add_instance_management(
		const count_ptr<const instance_management_base>& c) {
	NEVER_NULL(c);
	if (reject_nonglobal_instance_management()) {
		// already have error message
		c->dump(cerr << "got: ", expr_dump_context::default_value) << endl;
		THROW_EXIT;
	}
	get_current_sequential_scope()->append_instance_management(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a (partially) type-checked connection or assignment 
	to the current scope, which may be dynamic.  
	\param c the new connection or assignment list.
 */
void
context::add_connection(
		const count_ptr<const meta_instance_reference_connection>& c
		) {
	if (reject_nonglobal_instance_management()) {
		// already have error message
		c->dump(cerr << "got: ", expr_dump_context::default_value) << endl;
		THROW_EXIT;
	}
	get_current_sequential_scope()->append_instance_management(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a (partially) type-checked connection or assignment 
	to the current scope, which may be dynamic.  
	Note: may have to keep global ordering of all expression assignments, 
	by adding them all to the global namespace (break const-ness?).  
	\param c the new connection or assignment list.
 */
void
context::add_assignment(
		const count_ptr<const param_expression_assignment>& c
		) {
	if (reject_nonglobal_instance_management()) {
		// already have error message
		c->dump(cerr << "got: ", expr_dump_context::default_value) << endl;
		THROW_EXIT;
	}
	get_current_sequential_scope()->append_instance_management(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const definition_base>
context::__lookup_definition_return(const never_ptr<const object> o) const {
#if REQUIRE_DEFINITION_EXPORT
	const never_ptr<const definition_base>
		ret(o.is_a<const definition_base>());
	if (ret) {
		if (!parse_opts.export_all && 
				(get_current_namespace() != ret->get_parent())
				&& !ret->is_exported()) {
			cerr << "Error: definition `" <<
				ret->get_qualified_name() <<
				"\' is not exported from its home namespace."
				<< endl;
			return never_ptr<const definition_base>();
		}
	}
	return ret;
#else
	return o.is_a<const definition_base>();
#endif
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
never_ptr<const definition_base>
context::lookup_definition(const token_identifier& id) const {
	INVARIANT(get_current_namespace());
	const never_ptr<const object> o(lookup_object(id));
	return __lookup_definition_return(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
	\param id the qualified name of the definition sought.  
	\return const pointer to the identified definition, if found.  
 */
never_ptr<const definition_base>
context::lookup_definition(const qualified_id& id) const {
	INVARIANT(get_current_namespace());
	const never_ptr<const object> o(lookup_object(id));
	return __lookup_definition_return(o);
}

#undef	LOOKUP_DEFINITION_RETURN

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param id the name of the instance sought.  
	\return const pointer to the named instance sought, if found.  
 */
context::placeholder_ptr_type
context::lookup_instance(const token_identifier& id) const {
	INVARIANT(get_current_namespace());
	const never_ptr<const object> o(lookup_object(id));
	return o.is_a<const instance_placeholder_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param id the qualified name of the instance sought.  
	\return const pointer to the named instance sought, if found.  
 */
context::placeholder_ptr_type
context::lookup_instance(const qualified_id& id) const {
	INVARIANT(get_current_namespace());
	const never_ptr<const object> o(lookup_object(id));
	return o.is_a<const instance_placeholder_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup name of internal node.  
	This lookup does not go though the usual generic object lookup.  
	lookup_member
 */
never_ptr<const node_instance_placeholder>
context::lookup_internal_node(const token_identifier& id) const {
	return get_current_open_definition()
		.is_a<const process_definition>()->lookup_member(id)
		.is_a<const node_instance_placeholder>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the current scope, be it namespace, or definition.  
	Namespaces are kept on a stack, 
	Definitions may not be nested, but loops and conditionals
	may be nested.  
	Loops and conditionals may not contain any other types of scopes.  
	Constant semantics.  
 */
never_ptr<const scopespace>
context::get_current_named_scope(void) const {
	if (current_prototype)	// careful, is excl_ptr<>
		// must be valid
		return current_prototype.is_a<const scopespace>();
		// return never_ptr<const definition_base>(current_prototype);
			// .as_a<scopespace>();
	else if (get_current_open_definition()) {
		// no longer a static cast
		const never_ptr<const scopespace>
			ret(get_current_open_definition()
				.is_a<const scopespace>());
		INVARIANT(ret);
		if (ret.is_a<const module>()) {
			// top-level module does not count as a named scope
			return namespace_stack.top().as_a<scopespace>();
		} else {
			return ret;
		}
	} else {
		// This code can be reached only when calling parser
		// using a read-only parse-context.  
		// ICE(cerr, cerr << "Reached the unreachable code!" << endl;)
		return namespace_stack.top().as_a<scopespace>();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the current scope, be it namespace, or definition.  
	Namespaces (which may be nested) are kept on a stack, 
	definitions may not be nested, but loops and conditionals
	may be nested.  
	Loops and conditionals may not contain any other types of scopes.  
	Non-constant semantics.  
 */
never_ptr<scopespace>
context::get_current_named_scope(void) {
	// what about current_prototype?
	if (get_current_open_definition()) {
		// used to be static cast
		const never_ptr<scopespace>
			ret(get_current_open_definition().is_a<scopespace>());
		INVARIANT(ret);
		if (ret.is_a<module>()) {
			// top-level module does not count as a named scope
			return namespace_stack.top().as_a<scopespace>();
		} else {
			return ret;
		}
	} else {
		ICE(cerr, cerr << "Reached the unreachable code!" << endl;)
		return namespace_stack.top().as_a<scopespace>();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current_fundamental_type
	to the current_named_scope.  
	Note: what we really mean is the current STATIC scope, 
	since instances are not truly added to dynamic scopes.  
	Make overloaded version with dimensions.  
 */
context::placeholder_ptr_type
context::add_instance(const token_identifier& id 
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
		, const relaxed_args_ptr_type& a
#endif
		) {
	STACKTRACE_VERBOSE;
	// wrapper
	return add_instance(id, 
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
		a, 
#endif
		index_collection_item_ptr_type(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current_fundamental_type
	to the current_named_scope.  
	Note: what we really mean is the current STATIC scope, 
	since instances are not truly added to dynamic scopes.  

	THIS NEEDS REWORK.  New pseudo code:
	In this name scope (namespace, definition...), 
	see if instantiation entry already exists with same ID.  
	If not, create one and register it, 
	otherwise if entry exists, do a limited static type-check.  
	Make an instantiation_statement, using the fundamental_type
		with back-reference to the instance_collection_base.  
	Append the (excl) statement to the current sequence-scope's 
		(global, definition, loop, conditional) list.  
	Append the (never) statement to the instance_collection_base, 
		checking for index collisions.  

	\param id the name of the new instance.  
	\param dim the dimension specifier, may be null.  
	\return pointer to newly created instantiation.  
 */
context::placeholder_ptr_type
context::add_instance(const token_identifier& id, 
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
		const relaxed_args_ptr_type& a, 
#endif
		const index_collection_item_ptr_type dim) {
	typedef	placeholder_ptr_type		return_type;
	STACKTRACE_VERBOSE;
	if (reject_nonglobal_instance_management()) {
		// already have error message
		cerr << "got instance name: " << id << endl;
		return return_type(NULL);
	}
	NEVER_NULL(current_fundamental_type);
	const never_ptr<scopespace>
		current_named_scope(get_current_named_scope());
	NEVER_NULL(current_named_scope);
	// additional constraints:
	if (current_named_scope.is_a<const user_def_datatype>() &&
		!current_fundamental_type->is_accepted_in_datatype()) {
		// datatypes may contain only other datatypes
		cerr << "User-defined datatypes may only contain "
			"other data types and parameter values."
			<< endl;
		current_fundamental_type->dump(cerr << "\tgot: ") << endl;
		return return_type(NULL);
	}
	else if (current_named_scope.is_a<const user_def_chan>() &&
		// channels may contain channels and datatypes
		!current_fundamental_type->is_accepted_in_channel()) {
		cerr << "User-defined channel types may only contain "
			"other channels and data types." << endl;
		current_fundamental_type->dump(cerr << "\tgot: ") << endl;
		return return_type(NULL);
	}
	// processes may contain anything

	const count_ptr<instantiation_statement_base> inst_stmt(
		fundamental_type_reference::make_instantiation_statement(
			current_fundamental_type, dim
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
			, a
#endif
			));
	NEVER_NULL(inst_stmt);
	const return_type
		inst_base(current_named_scope->add_instance(inst_stmt, id, 
			in_conditional_scope));
	// adds non-const back-reference

	if (!inst_base) {
		cerr << where(id) << endl;
		type_error_count++;
		THROW_EXIT;
	}

	NEVER_NULL(get_current_sequential_scope());
	get_current_sequential_scope()->append_instance_management(inst_stmt);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers an internal node (declaration) implicitly to the
	current open definition's scope.  
	Performs a lookup to make sure name doesn't shadow another name.
	\return reference to instance placeholder in scope.  
 */
context::node_placeholder_ptr_type
context::add_internal_node(const token_identifier& id, const size_t dim) {
	// top-level is also considered a definition
	const never_ptr<process_definition>
		pd(get_current_open_definition().is_a<process_definition>());
	NEVER_NULL(pd);
	// forward call
	return pd->add_node_instance_idempotent(id, dim);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using the current_type_reference, adds a template formal parameter.  
	Is like add_instance, above.  
	If already exists, then checks against previous formal declaration.  
	For now, only allow parameters.  
	\param id the name of the formal instance.  
	\param dim the dimensions of the instance. 
	\param d optional default value of parameter.  
	\sa add_instance
 */
context::placeholder_ptr_type
context::add_template_formal(const token_identifier& id, 
		const index_collection_item_ptr_type dim, 
		const count_ptr<const param_expr>& d) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(current_prototype);	// valid definition_base
	NEVER_NULL(current_fundamental_type);
	count_ptr<const param_type_reference>
		ptype(current_fundamental_type.is_a<const param_type_reference>());
	NEVER_NULL(ptype);
		// valid parameter type to instantiate
	// Don't use fundamental_type_reference::add_instance_to_scope()
	// Use a variant of scopespace::add_instance.  
	const relaxed_args_ptr_type bogus(NULL);
	const count_ptr<instantiation_statement_base> inst_stmt(
		fundamental_type_reference::make_instantiation_statement(
			ptype, dim
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
			, bogus
#endif
			));
	// template formals cannot have relaxed types!
	NEVER_NULL(inst_stmt);
	// formal instance is constructed and added in add_instance
	// TODO: pass default-value to add_template_formal... (2006-10-04)
	placeholder_ptr_type
		inst_base(
			// depends on strict_template_mode
			(strict_template_mode) ?
			current_prototype->add_strict_template_formal(
				inst_stmt, id) :
			current_prototype->add_relaxed_template_formal(
				inst_stmt, id)
			);
		// same as current_named_scope? perhaps assert check?

	if (!inst_base) {
		cerr << where(id) << endl;
		type_error_count++;
		THROW_EXIT;
	}

	if (d) {
		// need modifiable pointer to param_value_collection
		const never_ptr<const instance_placeholder_base>
			ib(inst_stmt->get_inst_base());
		const never_ptr<const param_value_placeholder>
			pic(ib.is_a<const param_value_placeholder>());
		NEVER_NULL(pic);
		// TODO: when I have time, propagate the changes
		// to make this const-correct...
		if (!const_cast<param_value_placeholder&>(*pic).
				assign_default_value(d).good) {
			// error: type check failed
			cerr << "ERROR assigning default value to " << id <<
				", type/size mismatch!  " << where(id) << endl;
			type_error_count++;
			THROW_EXIT;
		}
	}
#if SEQUENTIAL_SCOPE_INCLUDES_FORMALS
	const never_ptr<sequential_scope>
		seq_scope(current_prototype.is_a<sequential_scope>());
	NEVER_NULL(seq_scope);
	seq_scope->append_instance_management(inst_stmt);
#endif	// SEQUENTIAL_SCOPE_INCLUDES_FORMALS
	return inst_base;
}	// end context::add_template_formal()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for adding a non-arrayed template formal instantiation.  
 */
context::placeholder_ptr_type
context::add_template_formal(const token_identifier& id, 
		count_ptr<const param_expr> d) {
	return add_template_formal(id, index_collection_item_ptr_type(NULL), d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: write it, finish it -- what about arrays?
	Using the current_type_reference, 
	adds a port formal parameter.  
	Is like add_instance, above.  
	If already exists, then checks against previous formal declaration.  
	For now, only allow parameters.  
	\param id the name of the formal instance.  
	\sa add_instance
 */
context::placeholder_ptr_type
context::add_port_formal(const token_identifier& id, 
		const index_collection_item_ptr_type dim) {
	STACKTRACE_VERBOSE;
	INVARIANT(current_prototype);	// valid definition_base
	INVARIANT(!current_fundamental_type.is_a<const param_type_reference>());
		// valid port type to instantiate
	const count_ptr<instantiation_statement_base> inst_stmt(
		fundamental_type_reference::make_instantiation_statement(
			current_fundamental_type, dim));
	NEVER_NULL(inst_stmt);
	// instance is constructed and added in add_instance
	const never_ptr<const physical_instance_placeholder>
		inst_base(current_prototype->add_port_formal(inst_stmt, id));
		// same as current_named_scope? perhaps assert check?

	if (!inst_base) {
		cerr << where(id) << endl;
		type_error_count++;
		THROW_EXIT;
	}
#if 1 || SEQUENTIAL_SCOPE_INCLUDES_FORMALS
	const never_ptr<sequential_scope>
		seq_scope(current_prototype.is_a<sequential_scope>());
	NEVER_NULL(seq_scope);
	seq_scope->append_instance_management(inst_stmt);
#endif	// SEQUENTIAL_SCOPE_INCLUDES_FORMALS
	return inst_base;
}	// end context::add_port_formal()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for adding a non-arrayed port formal instantiation.  
 */
context::placeholder_ptr_type
context::add_port_formal(const token_identifier& id) {
	return add_port_formal(id, index_collection_item_ptr_type(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
context::commit_definition_arity(void) {
	INVARIANT(current_prototype);
	current_prototype->commit_arity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: 2006-09-01
	rewrite using placeholders.
 */
count_ptr<context::loop_var_placeholder_type>
context::push_loop_var(const token_identifier& i) {
	typedef	count_ptr<loop_var_placeholder_type>	return_type;
	const never_ptr<const object> probe(lookup_object(i));
	if (probe) {
		cerr << "Warning (promoted to error): "
			"loop variable \"" << i << "\" shadows another "
			"object by the same name: ";
		probe->dump(cerr) << endl;
		return return_type(NULL);
	}
	// Technically, variable is not a true member of the scope, 
	// nevertheless, it needs to associate with some parent scope.  
	const return_type
		ret(new loop_var_placeholder_type(
			*get_current_named_scope(), i , 0));
	INVARIANT(ret);
	loop_var_stack.push_front(ret);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
context::pop_loop_var(void) {
	INVARIANT(!loop_var_stack.empty());
	loop_var_stack.pop_front();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
context::dump_file_stack(ostream& o) const {
	file_name_stack_type::const_iterator i(file_name_stack.begin());
	const file_name_stack_type::const_iterator e(file_name_stack.end());
	for ( ; i!=e; i++) {
		o << "At: " << *i << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Automatic indentation for nicer debug printing.
	TODO: phase this out in favor of util::indent.  
 */
string
context::auto_indent(void) const {
	long i = 0;
	string ret = "\n";
	for (i=0; i < indent; i++)
		ret += "| ";
	ret += "+-";
	return ret;
}

//=============================================================================
// class context::namespace_frame method definitions

context::namespace_frame::namespace_frame(context& c, 
		const token_identifier& i) : _context(c) {
	_context.open_namespace(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
context::namespace_frame::~namespace_frame() {
	_context.close_namespace();
}

//=============================================================================
// class context::fundamental_type_frame method definitions

context::fundamental_type_frame::fundamental_type_frame(context& c, 
		const count_ptr<const fundamental_type_reference>& t) :
		_context(c) {
	_context.set_current_fundamental_type(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
context::fundamental_type_frame::~fundamental_type_frame() {
	// if there was no error
	if (_context.get_current_fundamental_type())
		_context.reset_current_fundamental_type();
}

//=============================================================================
// class context::enum_definition_frame method definitions

context::enum_definition_frame::enum_definition_frame(context& c,
		const token_identifier& i) : _context(c) {
	_context.open_enum_definition(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
context::enum_definition_frame::~enum_definition_frame() {
	_context.close_enum_definition();
}

//=============================================================================
// struct context::loop_var_frame method definitions

context::loop_var_frame::loop_var_frame(context& c,
		const token_identifier& i) :
		_context(c), var(_context.push_loop_var(i)) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
context::loop_var_frame::~loop_var_frame() {
	if (var)
		_context.pop_loop_var();
}

//=============================================================================
// struct context::loop_scope_frame method definitions

/**
	Add the new loop scope to the current sequential scope, 
	then pushes it onto the sequential scope stack.  
 */
context::loop_scope_frame::loop_scope_frame(context& c, 
		const count_ptr<loop_scope>& l
		) : _context(c) {
	NEVER_NULL(l);
	_context.get_current_sequential_scope()->append_instance_management(l);
	_context.sequential_scope_stack.push(never_ptr<sequential_scope>(&*l));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequential scope stack balancing destructor.  
 */
context::loop_scope_frame::~loop_scope_frame() {
	_context.sequential_scope_stack.pop();
}

//=============================================================================
// struct context::conditional_scope_frame method definitions

/**
	Adde the new conditional scope to the current sequential scope, 
	then pushes it onto the sequential scope stack.  
	Also saves status of the in_conditional_scope flag.
 */
context::conditional_scope_frame::conditional_scope_frame(context& c, 
		const count_ptr<conditional_scope>& l) :
		_context(c), parent_cond(c.in_conditional_scope) {
	NEVER_NULL(l);
	_context.get_current_sequential_scope()->append_instance_management(l);
	_context.sequential_scope_stack.push(never_ptr<sequential_scope>(&*l));
	_context.in_conditional_scope = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequential scope stack balancing destructor.  
	Also restores status of the in_conditional_scope flag.
 */
context::conditional_scope_frame::~conditional_scope_frame() {
	_context.sequential_scope_stack.pop();
	_context.in_conditional_scope = parent_cond;
}

//=============================================================================
// struct context::file_stack_frame method definitions

context::file_stack_frame::file_stack_frame(
		context& c, const string& s) : _context(c) {
	_context.file_name_stack.push_front(s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
context::file_stack_frame::~file_stack_frame() {
	_context.file_name_stack.pop_front();
}

//=============================================================================
// explicit template method instantiations

template class context::definition_frame<process_definition>;
template class context::definition_frame<user_def_chan>;
template class context::definition_frame<user_def_datatype>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __AST_PARSE_CONTEXT_CC__

