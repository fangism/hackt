// "art_symbol_table.cc"

#include <assert.h>
#include <iostream>

#include "art_parser.h"
#include "art_symbol_table.h"
#include "art_object.h"

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
		ns_stack(), 
		current_chan_def(NULL), 
		check_against_prev_chan(false), 
		current_type_def(NULL), 
		check_against_prev_type(false), 
		current_proc_def(NULL), 
		check_against_prev_process(false), 
		inst_proc_def(NULL), 
		inst_type_def(NULL)
//		inst_chan_def(NULL)
		{
	// perhaps verify that g is indeed global?
	ns_stack.push(g);

	// "current_namespace" is macro-defined to ns_stack.top()
	assert(current_namespace);		// make sure allocated properly

	// Add to the global namespace all built-in types and definitions.  
	// When sub-namespaces are created, they will inherit aliases to these
	// built-in types to speed-up lookup-resolutions in namespaces.  
	assert(current_namespace->add_built_in_type_definition(
		new built_in_type_def(current_namespace, "bool")));
	assert(current_namespace->add_built_in_type_definition(
		new built_in_type_def(current_namespace, "int")));
	assert(current_namespace->add_built_in_type_definition(
		new built_in_type_def(current_namespace, "pbool")));
	assert(current_namespace->add_built_in_type_definition(
		new built_in_type_def(current_namespace, "pint")));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Context destructor.  Need not explicitly clear structure members, 
	as their default destructors are automatically invoked.  
	Reminder, that the global namespace in the ns_stack is not owned
	by this context, and thus should not be deleted.  
 */
context::~context() {
//	ns_stack.pop();			// redundant
// possible sanity check: the remaining namespace is the global
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attempts to open up namespace, modifying the context, 
	and pushes the new namespace onto ns_stack.
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
//		current_namespace = insub;
		ns_stack.push(insub);
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
//	current_namespace = current_namespace->leave_namespace();	// obsolete
	new_top = current_namespace->leave_namespace();
	ns_stack.pop();
	assert(ns_stack.top() == new_top);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive, adds namespace to search list
// error possible
void
// const name_space*
context::using_namespace(const id_expr& id) {
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
context::alias_namespace(const id_expr& id, const string& a) {
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
	check_against_prev_process = 
		(current_namespace->probe_process(pname) != NULL);
	process_definition* p = current_namespace->add_proc_declaration(pname);
	if (p) {
		assert(!current_proc_def);	// sanity check
		current_proc_def = p;
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
	check_against_prev_process = 
		(current_namespace->probe_process(pname) != NULL);
	process_definition* p = current_namespace->add_proc_definition(pname);
	if (p) {
		assert(!current_proc_def);	// sanity check
		current_proc_def = p;
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
	Closes a process definition in the context.  
	Just sets current_proc_def to NULL.  
 */
void
context::close_process(void) {
	assert(current_proc_def);		// sanity check
	check_against_prev_process = false;
	indent--;
	current_proc_def = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a user-defined data type definition in the context.  
	Just sets current_type_def to NULL.  
 */
void
context::close_datatype(void) {
	assert(current_type_def);
	indent--;
	current_type_def = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies the context's current type definition (inst_type_def) 
	if the referenced type is resolved without error.  
	\param id the name of the type (unqualified).  
	\return pointer to defined or declared type if unique found, 
		else NULL of no match or ambiguous.  
 */
const type_definition*
context::set_type_def(const token_string& id) {
	// lookup type (will be built-in int or bool)
	assert(current_namespace);
	const type_definition* ret = current_namespace->lookup_unqualified_type(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} else {
		assert(!inst_type_def);		// sanity check
		inst_type_def = ret;
		indent++;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies the context's current type definition (inst_type_def) 
	if the referenced type is resolved without error.  
	TODO: Currently doesn't check template specialization type. 
	Punting on this until template mechanism is more generalized.  
	Propose to do template-checking outside of this function, 
	do it as it is encountered in tree-traversal.  
	\param tid the name of the type (unqualified).  
	\return pointer to defined or declared type if unique found, 
		else NULL of no match or ambiguous.  
 */
const type_definition*
context::set_type_def(const id_expr& tid) {
	// lookup type (will be built-in int or bool)
	assert(current_namespace);
	const type_definition* ret = 
		current_namespace->lookup_qualified_type(tid);
	if (!ret) {
		type_error_count++;
		// a more helpful message, please!
		cerr << tid.where() << endl;
		exit(1);			// temporary
	} else {
		assert(!inst_type_def);		// sanity check
		inst_type_def = ret;
		indent++;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies the context's current type definition (inst_type_def) 
	if the referenced type is resolved without error.  
	\param id the name of the type (unqualified).  
	\return pointer to defined or declared type if unique found, 
		else NULL of no match or ambiguous.  
 */
const type_definition*
context::set_type_def(const token_type& id, const token_int& w) {
	// lookup type (will be built-in int or bool)
#if 0
	assert(current_namespace);
	const type_definition* ret = current_namespace->lookup_unqualified_type(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} else {
		assert(!inst_type_def);		// sanity check
		inst_type_def = ret;
		indent++;
	}
	// to do: set template width using w
	return ret;
#else
	return NULL;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the current type definition to NULL.
 */
void
context::unset_type_def(void) {
	if (inst_type_def) {
		indent--;
		inst_type_def = NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

type_instantiation*
context::add_type_instance(const token_identifier& id) {
	type_instantiation* ret;
	assert(current_namespace);
	assert(inst_type_def);
	ret = current_namespace->add_type_instantiation(*inst_type_def, id);
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
	TO DO: write it, finish it.  
	\param id the name of the formal instance.  
 */
const type_instantiation*
context::add_template_formal(const token_identifier& id) {
#if 0
	const type_instantiation* ret;
	assert(inst_type_def);
	assert(current_namespace);
//	ret = inst_type_def->add_template_formal();
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
};	// end namespace entity
};	// end namespace ART

