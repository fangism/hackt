/**
	\file "art_context.cc"
	Class methods for context object passed around during 
	type-checking, and object construction.  
 	$Id: art_context.cc,v 1.15 2005/01/06 17:44:52 fang Exp $
 */

#include <assert.h>
#include <iostream>

#include "art_context.h"
#include "art_parser_base.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_inst_ref.h"
#include "art_object_inst_stmt_base.h"
#include "art_object_assign.h"
#include "art_object_connect.h"
#include "art_object_instance.h"	// for instantiation_statement
#include "art_object_instance_param.h"	// for param_instantiation_statement
#include "art_object_module.h"

//=============================================================================
namespace ART {
using namespace entity;

namespace parser {
#include "using_ostream.h"

//=============================================================================
// class context method definition

/**
	Default context constructor.  Should really only be invoked
	once (per source file or module).  
	Creates the global namespace and initializes it with
	built-in types.  
	\param g pointer to global namespace.
 */
context::context(module& m) :
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		namespace_stack(), 
		current_open_definition(NULL), 
		current_prototype(NULL), 
		definition_stack(), 
		current_fundamental_type(NULL), 
		sequential_scope_stack(), 
		object_stack(), 
		global_namespace(m.get_global_namespace()), 
		master_instance_list(m.instance_management_list)
		{

	// perhaps verify that g is indeed global?  can't be any namespace
	namespace_stack.push(global_namespace);
	// remember that the creator of the global namespace is responsible
	// for deleting it.  
	sequential_scope_stack.push(never_ptr<sequential_scope>(&m));
	object_stack.push(count_ptr<object>(NULL));
	definition_stack.push(never_ptr<const definition_base>(NULL));
	// initializing stacks else top() will seg-fault

	// "current_namespace" is macro-defined to namespace_stack.top()
	assert(current_namespace);	// make sure allocated properly
	assert(global_namespace);	// same pointer

	// Q: should built-ins be in a super namespace about the globals?
	// some static global set?  shared among all objects?
	// thinking of merging object hierarchies in the future for linking...
	// want to avoid multiple definitions in the global namespace.  
	// A: built-ins have been re-implemented in "art_built_ins.*"

}	// end of context constructor

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
	never_ptr<name_space> insub;
	insub = current_namespace->add_open_namespace(id);

#if 0
	cerr << "After add_open_namespace(), " << endl;
	current_namespace->dump(cerr) << endl;
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
		cerr << id.where() << endl;
		exit(1);			// temporary
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
		new_top(current_namespace->leave_namespace());
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	namespace_stack.pop();
	assert(current_namespace == new_top);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive, adds namespace to search list
// error possible
void
context::using_namespace(const qualified_id& id) {
	never_ptr<const name_space> ret =
		current_namespace->add_using_directive(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive under a different local name
void
context::alias_namespace(const qualified_id& id, const string& a) {
	never_ptr<const name_space> ret =
		current_namespace->add_using_alias(id, a);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Peeks at the top of the namespace stack.  
	\return pointer at the top of the namespace stack.  
 */
never_ptr<const name_space>
context::top_namespace(void) const {
	return current_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a declaration and checks against any previous 
	prototype with the same name.  
	Caller should check the return value if there is an error to report.  
 */
never_ptr<definition_base>
context::add_declaration(excl_ptr<definition_base> d) {
	never_ptr<definition_base> ret(current_namespace->add_definition(d));
	if (!ret) {
		// something went wrong
		type_error_count++;
		// additional error message isn't really necessary...
//		cerr << endl << "ERROR in context::add_declaration().  ";
//		cerr << d->where() << endl;	// caller will do
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
	never_ptr<enum_datatype_def>
		ed(current_namespace->lookup_object_here_with_modify(ename)
				.is_a<enum_datatype_def>());
	if (ed) {
		if (ed->is_defined()) {
			cerr << ename << " is already defined!  attempted "
				"redefinition at " << ename.where() << endl;
			exit(1);
		}
		assert(!current_open_definition);	// sanity check
		current_open_definition = ed;
		ed->mark_defined();
		indent++;
	} else {
		// no real reason why this should ever fail...
		type_error_count++;
		cerr << ename.where() << endl;
		exit(1);			// temporary
		// return NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	\return true if successful, false if there was conflict.  
 */
bool
context::add_enum_member(const token_identifier& em) {
	never_ptr<enum_datatype_def>
		ed(current_open_definition.is_a<enum_datatype_def>());
	if (!ed) {
		cerr << "expected current_open_definition to be "
			"enum_datatype_def!  FATAL ERROR." << endl;
		exit(1);
	} else if (ed->add_member(em)) {
		return true;
	} else {
		cerr << "enum " << ed->get_name() << " already has a member "
			"named " << em << ".  ERROR! " << em.where() << endl;
		type_error_count++;
		exit(1);
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
context::close_enum_definition(void) {
	current_open_definition = never_ptr<definition_base>(NULL);
	indent--;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	THIS NEEDS SERIOUS RE-WORK.  
	Registers a process definition's signature.  
	\param ps the process signature, which contains and identifier, 
		optional template signature, and port signature.  
	Details: checks to see if prototype was already declared.  
	If already declared, then this re-declaration MUST be indentical, 
	else report error of mismatched re-declaration.  
	If not already declared, create an entry...
 */
void
context::open_process_definition(const token_identifier& pname) {
	never_ptr<process_definition>
		pd(current_namespace->lookup_object_here_with_modify(pname)
				.is_a<process_definition>());
	if (pd) {
		if (pd->is_defined()) {
			cerr << pname << " is already defined!  attempted "
				"redefinition at " << pname.where() << endl;
			exit(1);
		}
		assert(!current_open_definition);	// sanity check
		current_open_definition = pd;
		sequential_scope_stack.push(pd.as_a<sequential_scope>());
		pd->mark_defined();
		indent++;
	} else {
		// no real reason why this should ever fail...
		type_error_count++;
		cerr << pname.where() << endl;
		exit(1);			// temporary
		// return NULL
	}
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
	current_open_definition = never_ptr<definition_base>(NULL);
	indent--;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a process definition in the context.  
	Just sets current_open_definition to NULL.  
 */
void
context::close_process_definition(void) {
	// sanity check
	current_open_definition.must_be_a<process_definition>();
	sequential_scope_stack.pop();
	close_current_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a user-defined data type definition in the context.  
	Just sets current_open_definition to NULL.  
 */
void
context::close_datatype_definition(void) {
	// sanity check
//	current_open_definition.must_be_a<datatype_definition>();
	current_open_definition.must_be_a<datatype_definition_base>();
	close_current_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a user-defined channel type definition in the context.  
	Just sets current_open_definition to NULL.  
 */
void
context::close_chantype_definition(void) {
	current_open_definition.must_be_a<channel_definition_base>();
	close_current_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return destructive transfer of active definition (modifiable).  
 */
excl_ptr<definition_base>
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

never_ptr<const built_in_param_def>
context::get_current_param_definition(void) const {
	return current_definition_reference.is_a<const built_in_param_def>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

never_ptr<const datatype_definition_base>
context::get_current_datatype_definition(void) const {
	return current_definition_reference.is_a<const datatype_definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

never_ptr<const channel_definition_base>
context::get_current_channel_definition(void) const {
	return current_definition_reference.is_a<const channel_definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

never_ptr<const process_definition_base>
context::get_current_process_definition(void) const {
	return current_definition_reference.is_a<const process_definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
context::pop_current_definition_reference(void) {
	assert(current_definition_reference);
	definition_stack.pop();
}

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
never_ptr<definition_base>
context::set_current_prototype(excl_ptr<definition_base> d) {
	assert(!current_prototype);
	current_prototype = d;
	return current_prototype;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Given a valid complete type reference, set the context to use it.  
 */
void
context::set_current_fundamental_type(
		count_ptr<const fundamental_type_reference> tr) {
	assert(!current_fundamental_type);
	current_fundamental_type = tr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Query the fundamental type to instantiate.  
 */
count_ptr<const fundamental_type_reference>
context::get_current_fundamental_type(void) const {
	return current_fundamental_type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO FIX: when checking a prototype, current scope should be 
	updated to that prototype's definition.  
	fixed: resolves object_handle automatically.
 */
never_ptr<const object>
context::lookup_object(const qualified_id& id) const {
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
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
bool
context::alias_definition(never_ptr<const definition_base> d,
		const token_identifier& a) {
	return get_current_named_scope()->add_definition_alias(d, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a (partially) type-checked connection or assignment 
	to the current scope, which may be dynamic.  
	\param c the new connection or assignment list.
 */
void
context::add_connection(excl_ptr<const instance_reference_connection> c) {
	typedef	excl_ptr<const instance_management_base> im_pointer_type;
	never_ptr<sequential_scope>
		seq_scope(get_current_named_scope().is_a<sequential_scope>());
	im_pointer_type imb(c);	// is not const, should be transferrable
	assert(imb);
	assert(!c);
	if (seq_scope) {
		seq_scope->append_instance_management(imb);
	} else {
#if 0
		// no excl_ptr copy constructor
		master_instance_list.push_back(imb);
#else
		master_instance_list.push_back(imb);
//		assert(imb);		// doesn't transfer!
//		master_instance_list.back() = imb;
#endif
	}
	assert(!imb);
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
context::add_assignment(excl_ptr<const param_expression_assignment> c) {
	typedef	excl_ptr<const instance_management_base> im_pointer_type;
	never_ptr<sequential_scope>
		seq_scope(get_current_named_scope().is_a<sequential_scope>());
	im_pointer_type imb(c);
	assert(imb);
	assert(!c);
	if (seq_scope) {
		seq_scope->append_instance_management(imb);
	} else {
		master_instance_list.push_back(imb);
	}
	assert(!imb);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
never_ptr<const definition_base>
context::lookup_definition(const token_identifier& id) const {
	assert(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
never_ptr<const definition_base>
context::lookup_definition(const qualified_id& id) const {
	assert(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const definition_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
context::lookup_instance(const token_identifier& id) const {
	assert(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
context::lookup_instance(const qualified_id& id) const {
	assert(current_namespace);
	never_ptr<const object> o(get_current_named_scope()->lookup_object(id));
	while (o.is_a<const object_handle>())
		o = never_ptr<const object>(&o->self());
	return o.is_a<const instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the current scope, be it namespace, or definition.  
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
	else if (current_open_definition) {
		// no longer a static cast
		never_ptr<const scopespace>
			ret(current_open_definition.is_a<const scopespace>());
		assert(ret);
		return ret;
	} else
		return current_namespace.as_a<scopespace>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the current scope, be it namespace, or definition.  
	Namespaces (which may be nested) are kept on a stack, 
	definitions may not be nested, but loops and conditionals
	may be nested.  
	Loops and conditionals may not contain any other types of scopes.  
	Non-constant semantics.  
 */
never_ptr<scopespace>
context::get_current_named_scope(void) {
	// what about current_prototype?
	if (current_open_definition) {
		// used to be static cast
		never_ptr<scopespace>
			ret(current_open_definition.is_a<scopespace>());
		assert(ret);
		return ret;
	} else
		return current_namespace.as_a<scopespace>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current_fundamental_type
	to the current_named_scope.  
	Note: what we really mean is the current STATIC scope, 
	since instances are not truly added to dynamic scopes.  
	Make overloaded version with dimensions.  
 */
never_ptr<const instance_collection_base>
context::add_instance(const token_identifier& id) {
	// wrapper
	return add_instance(id, index_collection_item_ptr_type(NULL));
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
never_ptr<const instance_collection_base>
context::add_instance(const token_identifier& id, 
		index_collection_item_ptr_type dim) {
	typedef	never_ptr<const instance_collection_base>	return_type;
	assert(current_fundamental_type);
	never_ptr<scopespace> current_named_scope(get_current_named_scope());
	assert(current_named_scope);

	excl_ptr<instantiation_statement> inst_stmt =
		fundamental_type_reference::make_instantiation_statement(
			current_fundamental_type, dim);
	assert(inst_stmt);
	return_type inst_base(
		current_named_scope->add_instance(inst_stmt, id));
	// adds non-const back-reference

	if (!inst_base) {
		cerr << id.where() << endl;
		type_error_count++;
		exit(1);
	}

	excl_ptr<const instance_management_base>
		imb = inst_stmt.as_a_xfer<const instance_management_base>();
	assert(current_sequential_scope);
	current_sequential_scope->append_instance_management(imb);
	INVARIANT(!imb);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: write it, finish it -- what about arrays?
	Using the current_type_reference, 
	adds a template formal parameter.  
	Is like add_instance, above.  
	If already exists, then checks against previous formal declaration.  
	For now, only allow parameters.  
	\param id the name of the formal instance.  
	\sa add_instance
 */
never_ptr<const instance_collection_base>
context::add_template_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim, 
		count_ptr<const param_expr> d) {
	assert(current_prototype);	// valid definition_base
	assert(current_fundamental_type);
	count_ptr<const param_type_reference>
		ptype(current_fundamental_type.is_a<const param_type_reference>());
	assert(ptype);
		// valid parameter type to instantiate
	// Don't use fundamental_type_reference::add_instance_to_scope()
	// Use a variant of scopespace::add_instance.  
	excl_ptr<instantiation_statement> inst_stmt =
		fundamental_type_reference::make_instantiation_statement(
			ptype, dim);
	assert(inst_stmt);
	// instance is constructed and added in add_instance
	never_ptr<const instance_collection_base>
		inst_base(current_prototype->add_template_formal(
			inst_stmt, id));
		// same as current_named_scope? perhaps assert check?

	if (!inst_base) {
		cerr << id.where() << endl;
		type_error_count++;
		exit(1);
	}

	if (d) {
		// need modifiable pointer to param_instance_collection
		never_ptr<instance_collection_base>
			ib(inst_stmt->get_inst_base());
		never_ptr<param_instance_collection>
			pic(ib.is_a<param_instance_collection>());
		assert(pic);
		if (!pic->assign_default_value(d)) {
			// error: type check failed
			cerr << "ERROR assigning default value to " << id <<
				", type/size mismatch!  " << id.where() << endl;
			type_error_count++;
			exit(1);
		}
	}

	excl_ptr<const instance_management_base>
		imb = inst_stmt.as_a_xfer<const instance_management_base>();
	never_ptr<sequential_scope>
		seq_scope(current_prototype.is_a<sequential_scope>());
		// same as current_sequential_scope? perhaps assert check?
	assert(seq_scope);
	seq_scope->append_instance_management(imb);

	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for adding a non-arrayed template formal instantiation.  
 */
never_ptr<const instance_collection_base>
context::add_template_formal(const token_identifier& id, 
		count_ptr<const param_expr> d) {
	return add_template_formal(id, index_collection_item_ptr_type(NULL), d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TO DO: write it, finish it -- what about arrays?
	Using the current_type_reference, 
	adds a port formal parameter.  
	Is like add_instance, above.  
	If already exists, then checks against previous formal declaration.  
	For now, only allow parameters.  
	\param id the name of the formal instance.  
	\sa add_instance
 */
never_ptr<const instance_collection_base>
context::add_port_formal(const token_identifier& id, 
		index_collection_item_ptr_type dim) {
	assert(current_prototype);	// valid definition_base
	assert(!current_fundamental_type.is_a<const param_type_reference>());
		// valid port type to instantiate
	excl_ptr<instantiation_statement> inst_stmt =
		fundamental_type_reference::make_instantiation_statement(
			current_fundamental_type, dim);
	assert(inst_stmt);
	// instance is constructed and added in add_instance
	never_ptr<const instance_collection_base>
		inst_base(current_prototype->add_port_formal(
			inst_stmt, id));
		// same as current_named_scope? perhaps assert check?

	if (!inst_base) {
		cerr << id.where() << endl;
		type_error_count++;
		exit(1);
	}

	excl_ptr<const instance_management_base>
		imb = inst_stmt.as_a_xfer<const instance_management_base>();
	never_ptr<sequential_scope>
		seq_scope(current_prototype.is_a<sequential_scope>());
		// same as current_sequential_scope? perhaps assert check?
	assert(seq_scope);
	seq_scope->append_instance_management(imb);

	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper for adding a non-arrayed port formal instantiation.  
 */
never_ptr<const instance_collection_base>
context::add_port_formal(const token_identifier& id) {
	return add_port_formal(id, index_collection_item_ptr_type(NULL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Created objects are passed around on the context's object stack.  
	For now we restrict the type to param_expr and instance_reference.  
	We permit NULL objects on the stack, as error placeholders.  
 */
void
context::push_object_stack(count_ptr<object> o)
// context::push_object_stack(excl_ptr<object> o)
{
	if (o) {
		const object* oself = &o->self();
		// can we go back to not using maked pointer for check?
		if (!(IS_A(const param_expr*, oself) ||
			IS_A(const ART::entity::index_expr*, oself) ||
			IS_A(const object_list*, oself) ||
//			IS_A(const pint_const*, oself) ||
				// eliminate object_list after making others
			IS_A(const range_expr_list*, oself) ||
			IS_A(const index_list*, oself) ||
			IS_A(const instance_reference_base*, oself))) {
			oself->what(cerr << "Unexpectedly got a ") <<
				" on the context's object stack." << endl;
		}
		assert(IS_A(const param_expr*, oself) ||
			IS_A(const ART::entity::index_expr*, oself) ||
			IS_A(const object_list*, oself) ||
//			IS_A(const pint_const*, oself) ||
				// eliminate object_list after making others
			IS_A(const range_expr_list*, oself) ||
			IS_A(const index_list*, oself) ||
			IS_A(const instance_reference_base*, oself));
	}
	object_stack.push(o);
}

count_ptr<object>
// excl_ptr<object>
context::pop_top_object_stack(void) {
	count_ptr<object> ret = object_stack.top();
//	excl_ptr<object> ret = object_stack.top();
	object_stack.pop();
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// automatic indentation for nicer debug printing
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
}	// end namespace entity
}	// end namespace ART

