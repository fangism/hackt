// "art_symbol_table.cc"

#include <assert.h>
#include <iostream>

#include "art_symbol_table.h"
#include "art_parser.h"
#include "art_object.h"
#include "art_object_expr.h"

// #include "list_of_ptr_template_methods.h"	// PHASE OUT

//=============================================================================
namespace ART {
using namespace entity;

namespace parser {
using namespace std;

//=============================================================================
// class context method definition

/**
	Default context constructor.  Should really only be invoked
	once (per source file or module).  
	Creates the global namespace and initializes it with
	built-in types.  
	\param g pointer to global namespace.
 */
context::context(name_space* g) : 
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		namespace_stack(), 
		current_open_definition(NULL), 
		check_against_previous_definition_signature(false), 
		current_definition_reference(NULL), 
		current_fundamental_type(NULL), 
		current_instance_to_connect(NULL), 
		current_template_arguments(NULL), 
		current_array_dimensions(NULL), 
		dynamic_scope_stack(), 
		global_namespace(g) {

	// perhaps verify that g is indeed global?  can't be any namespace
	namespace_stack.push(g);
	dynamic_scope_stack.push(NULL);	// else top() will seg-fault

	// "current_namespace" is macro-defined to namespace_stack.top()
	assert(current_namespace);	// make sure allocated properly
	assert(global_namespace);	// same pointer

	// write-able pointer to global namespace
	name_space* modify_global = current_namespace;

	// should built-ins be in a super namespace about the globals?
	// some static global set?  shared among all objects?
	// thinking of merging object hierarchies in the future for linking...
	// want to avoid multiple definitions in the global namespace.  

	// must do parameter definitions first, "int" depends on "pint"
	built_in_param_def* pbool_pd = 
		new built_in_param_def(modify_global, "pbool");
	assert(pbool_pd == modify_global->add_definition(pbool_pd));
	built_in_param_def* pint_pd = 
		new built_in_param_def(modify_global, "pint");
	assert(pint_pd == modify_global->add_definition(pint_pd));

	// then add built-in param type *references*
	param_type_reference* pbool_pr =
		new param_type_reference(pbool_pd);	// "pbool"
	assert(pbool_pr == modify_global->add_type_reference(pbool_pr));
	param_type_reference* pint_pr =
		new param_type_reference(pint_pd);	// "pint"
	assert(pint_pr == modify_global->add_type_reference(pint_pr));


	// Add to the global namespace all built-in types definitions.  
	// Lookups of built-in type always goes to global namespace.  
	built_in_datatype_def* bool_dd =
		new built_in_datatype_def(modify_global, "bool");
	assert(bool_dd == modify_global->add_definition(bool_dd));
	built_in_datatype_def* int_dd =
		new built_in_datatype_def(modify_global, "int");
	assert(int_dd);
	int_dd->add_template_formal(
		new param_instantiation(*modify_global, *pint_pr, "width", 
			new param_const_int(32)));
		// effectively: template <pint width=32> deftype int;
	assert(int_dd == modify_global->add_definition(int_dd));
	// "int" is parameterized by width, but don't need to do anything
	//	differently here... or should we construct it properly?
	// using template <pint width [=32]>?


	// add built-in data type *references*
	// remember to hash using template arguments, if applicable
	data_type_reference* bool_dr = 
		new data_type_reference(bool_dd);	// "bool"
	assert(bool_dr);
	assert(bool_dr == modify_global->add_type_reference(bool_dr));
	// somewhere we need to check consistency between supplied
	//	template arguments and template formals!!!
	// recall that "int" is a template
	// shall we temporarily de-template it?

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
// name_space*
void
context::open_namespace(const token_identifier& id) {
	name_space* insub;
	insub = current_namespace->add_open_namespace(id);
	
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
//	return insub;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes the current namespace, leaving the scope.  
	Also pops the namespace off of the stack.  
	\sa open_namespace
 */
void
context::close_namespace(void) {
	const name_space* new_top;
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	new_top = current_namespace->leave_namespace();
	namespace_stack.pop();
	assert(current_namespace == new_top);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive, adds namespace to search list
// error possible
void
// const name_space*
context::using_namespace(const qualified_id& id) {
	const name_space* ret = current_namespace->add_using_directive(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
//	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive under a different local name
void
// const name_space*
context::alias_namespace(const qualified_id& id, const string& a) {
	const name_space* ret = current_namespace->add_using_alias(id, a);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
//	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Peeks at the top of the namespace stack.  
	\return pointer at the top of the namespace stack.  
 */
const name_space*
context::top_namespace(void) const {
	return current_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a process prototype signature.  
	Type-checking occurs in the check_build of the template
	and port formals.  
	\param pname is just the name of the process prototype.  
 */
void
context::declare_process(const token_identifier& pname) {
	check_against_previous_definition_signature = 
		(current_namespace->probe_process(pname) != NULL);
	process_definition* p = current_namespace->add_proc_declaration(pname);
	if (p) {
		assert(!current_open_definition);	// sanity check
		current_open_definition = p;
		indent++;
	} else {
		// no real reason why this should ever fail...
		// leave current_open_definition as it is
		type_error_count++;
		cerr << pname.where() << endl;
		exit(1);			// temporary
		// return NULL
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a process definition's signature.  
	\param ps the process signature, which contains and identifier, 
		optional template signature, and port signature.  
	Details: checks to see if prototype was already declared.  
	If already declared, then this re-declaration MUST be indentical, 
	else report error of mismatched re-declaration.  
	If not already declared, create an entry...
 */
void
context::open_process(const token_identifier& pname) {
	check_against_previous_definition_signature = 
		(current_namespace->probe_process(pname) != NULL);
	process_definition* p = current_namespace->add_proc_definition(pname);
	if (p) {
		assert(!current_open_definition);	// sanity check
		current_open_definition = p;
		indent++;
	} else {
		// no real reason why this should ever fail...
		// leave current_proc_def as it is
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
	current_open_definition = NULL;
	check_against_previous_definition_signature = false;
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
	MUST_BE_A(process_definition*, current_open_definition);
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
	MUST_BE_A(datatype_definition*, current_open_definition);
	close_current_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a user-defined channel type definition in the context.  
	Just sets current_open_definition to NULL.  
 */
void
context::close_chantype_definition(void) {
	MUST_BE_A(channel_definition*, current_open_definition);
	close_current_definition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const built_in_param_def*
context::get_current_param_definition(void) const {
	return IS_A(const built_in_param_def*, current_definition_reference);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const datatype_definition*
context::get_current_datatype_definition(void) const {
	return IS_A(const datatype_definition*, current_definition_reference);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const channel_definition*
context::get_current_channel_definition(void) const {
	return IS_A(const channel_definition*, current_definition_reference);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const process_definition*
context::get_current_process_definition(void) const {
	return IS_A(const process_definition*, current_definition_reference);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is this obsolete?
	Modifies the context's current_definition_reference, 
	using a built-in data-type definition.  
	(Approach: don't treat int<> as a corner case of a built-in
		template; follow a standard framework)
	\param id the name of the type (unqualified).  
	\return pointer to defined or declared type if unique found, 
		else NULL of no match or ambiguous.  
 */
const datatype_definition*
context::set_datatype_def(const token_datatype& id) {
	// lookup type (will be built-in int or bool)
	assert(current_namespace);

	// should always lookup in global namespace... fix this?
	const datatype_definition* ret = IS_A(const built_in_datatype_def*, 
		current_namespace->get_global_namespace()->
			lookup_object_here(id));
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} else {
		assert(!current_definition_reference);	// sanity check
		current_definition_reference = ret;
		indent++;
	}
	// to do elsewhere: set template width using w
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the current instantiating data type definition.
	Checks for NULL first.  
	\param dd the data type definition.  
	\return the data type definition.
 */
const datatype_definition*
context::set_inst_data_def(const datatype_definition& dd) {
	assert(!current_definition_reference);
	current_definition_reference = &dd;
	return &dd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the current type definition to NULL.
 */
void
context::reset_current_definition_reference(void) {
	if (current_definition_reference) {
		indent--;
		current_definition_reference = NULL;
	}
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
		current_fundamental_type = NULL;
	} else {
		cerr << "warning: current_fundamental_type was already NULL."
			<< endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets current parameter instantiation type.
 */
const built_in_param_def*
context::set_param_def(const token_paramtype& pt) {
	assert(current_namespace);
	assert(!current_definition_reference);
	// always just lookup in global namespace
	const built_in_param_def* ret = IS_A(const built_in_param_def*, 
		current_namespace->get_global_namespace()->
			lookup_object_here(pt));
	if (!ret) {
		type_error_count++;
		cerr << pt.where() << endl;
		exit(1);			// temporary
	} else {
		assert(!current_definition_reference);	// sanity check
		current_definition_reference = ret;
		indent++;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the current instantiating param type definition.
	Checks for NULL first.  
	\param pd the param type definition.  
	\return the param type definition.
 */
const built_in_param_def*
context::set_inst_param_def(const built_in_param_def& pd) {
	assert(!current_definition_reference);
	current_definition_reference = &pd;
	return &pd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the current instantiating channel type definition.
	Checks for NULL first.  
	\param cd the channel type definition.  
	\return the channel type definition.
 */
const channel_definition*
context::set_inst_chan_def(const channel_definition& cd) {
	assert(!current_definition_reference);
	current_definition_reference = &cd;
	return &cd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the current instantiating process type definition.
	Checks for NULL first.  
	\param pd the process type definition.  
	\return the process type definition.
 */
const process_definition*
context::set_inst_proc_def(const process_definition& pd) {
	assert(!current_definition_reference);
	current_definition_reference = &pd;
	return &pd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const data_type_reference*
context::set_inst_data_type_ref(const data_type_reference& dr) {
	assert(!current_fundamental_type);
	current_fundamental_type = &dr;
	return &dr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const param_type_reference*
context::set_inst_param_type_ref(const param_type_reference& dr) {
	assert(!current_fundamental_type);
	current_fundamental_type = &dr;
	return &dr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const channel_type_reference*
context::set_inst_chan_type_ref(const channel_type_reference& cr) {
	assert(!current_fundamental_type);
	current_fundamental_type = &cr;
	return &cr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const process_type_reference*
context::set_inst_proc_type_ref(const process_type_reference& pr) {
	assert(!current_fundamental_type);
	current_fundamental_type = &pr;
	return &pr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using the current definition, and current set of template arguments, 
	check whether template arguments are consistent with definition.  
	If so, create a valid type reference, and set
	current_fundamental_type to it.  
	Called from concrete_type_ref::check_build.
	\return the new type reference if it was valid, else NULL.  
 */
const fundamental_type_reference*
context::set_current_fundamental_type(void) {
	assert(!current_fundamental_type);		// redundant
		// otherwise, someone forgot to reset it!
	assert(current_definition_reference);
	// current_template_arguments is optional
	// if it exists, then it is already checked.

	// check type cache, per major scope
	const fundamental_type_reference* ret =
		current_definition_reference->
			set_context_fundamental_type(*this);
		// changes current_fundamental_type
	if (ret)
		indent++;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check current_namespace's used_id_map for previously existing 
	matched fundamental_type_reference before adding?
	TO DO: later... encapsulate in a function
 */
const fundamental_type_reference*
context::set_current_fundamental_type(const fundamental_type_reference& tr) {
	assert(!current_fundamental_type);
	current_fundamental_type = &tr;
	return &tr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const object*
context::lookup_object(const qualified_id& id) const {
	return get_current_scope()->lookup_object(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
const definition_base*
context::lookup_definition(const token_identifier& id) const {
	assert(current_namespace);
	return IS_A(const definition_base*, 
		current_namespace->lookup_object(id));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ok to start search in namespace, because definitions
	can only be found in namespaces, not other types of scopes.  
 */
const definition_base*
context::lookup_definition(const qualified_id& id) const {
	assert(current_namespace);
	return IS_A(const definition_base*, 
		current_namespace->lookup_object(id));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const instantiation_base*
context::lookup_instance(const token_identifier& id) const {
	assert(current_namespace);
	return IS_A(const instantiation_base*, 
		current_namespace->lookup_object(id));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const instantiation_base*
context::lookup_instance(const qualified_id& id) const {
	assert(current_namespace);
	return IS_A(const instantiation_base*, 
		current_namespace->lookup_object(id));
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
const scopespace*
context::get_current_scope(void) const {
	if (current_dynamic_scope)
		return current_dynamic_scope;
	else if (current_open_definition)
		return current_open_definition;
	else
		return current_namespace;
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
scopespace*
context::get_current_scope(void) {
	if (current_dynamic_scope)
		return current_dynamic_scope;
	else if (current_open_definition)
		return current_open_definition;
	else
		return current_namespace;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current_fundamental_type
	to the current_scope.  
 */
const instantiation_base*
context::add_instance(const token_identifier& id) {
	assert(current_fundamental_type);
	scopespace* current_scope = get_current_scope();
	assert(current_scope);
	// virtual
	return current_fundamental_type->add_instance_to_scope(
		*current_scope, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current data-type with a particular
	identifier.  Array instantiation is handled in a separate method.  
	\param id name of instance.  
	\return valid pointer to instance object.  
 */
datatype_instantiation*
context::add_datatype_instance(const token_identifier& id) {
	datatype_instantiation* ret;
	const data_type_reference* dtr = 
		IS_A(const data_type_reference*, current_fundamental_type);
	assert(current_namespace);
	assert(dtr);

	// TO DO: need not just definition, but type_reference
	// for now, ignore template parameters, add them later
	ret = current_namespace->add_datatype_instantiation(*dtr, id);

	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} 
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an instance of the current param-type with a particular
	identifier.  Array instantiation is handled in a separate method.  
	\param id name of instance.  
	\return valid pointer to instance object.  
 */
param_instantiation*
context::add_paramtype_instance(const token_identifier& id) {
	param_instantiation* ret;
	const param_type_reference* ptr = 
		IS_A(const param_type_reference*, current_fundamental_type);
	assert(current_namespace);
	assert(ptr);

	// TO DO: need not just definition, but type_reference
	// for now, ignore template parameters, add them later
	ret = current_namespace->
		add_paramtype_instantiation(*ptr, id);

	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} 
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Using the current set type definition, adds a template formal 
	parameter.  
	If already exists, then checks against previous formal declaration.  
	TO DO: write it, finish it -- what about arrays?
	\param id the name of the formal instance.  
 */
const datatype_instantiation*
context::add_template_formal(const token_identifier& id) {
#if 0
	const datatype_instantiation* ret;
	assert(inst_data_def);
	assert(current_namespace);
	ret = current_open_definition->add_template_formal(id);
#else
	return NULL;
#endif
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
// explicit template instantiations

// template class list_of_const_ptr<param_expr>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

