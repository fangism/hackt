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
	Creates and initializes the global namespace.  
 */
context::context() : 
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		// create the global namespace
		current_ns(new name_space("",NULL)), 
		current_dt(NULL) {
	assert(current_ns);		// make sure allocated properly

	// add to the global namespace all built-in types and definitions
	assert(current_ns->add_built_in_type_definition(
		new built_in_type_def(current_ns, "bool")));
	assert(current_ns->add_built_in_type_definition(
		new built_in_type_def(current_ns, "int")));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// currently, default destructor
context::~context() {
	SAFEDELETE(current_ns);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attempts to open up namespace, modifying the context, and updating
	current_ns.
	Error occurs if name conflicts, as defined by the implementation
	within add_open_namespace.  
	\param id is the name of the namespace to enter.  
	\return pointer to opened namespace.
	\sa close_namespace
 */
name_space*
context::open_namespace(const token_identifier& id) {
	name_space* insub;
	insub = current_ns->add_open_namespace(id);
	
	// caution: assigning to NULL may ruin the context!
	// if this returns NULL, we signal to the caller to skip
	// over this malformed namespace body, don't even enter it...
	// which means no need to leave it.  

	if (!insub) {
		// leave current_ns as it is
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
		// return NULL
	} else {
		current_ns = insub;
		indent++;
	}
	return insub;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// closes namespace, leaving the scope
void
context::close_namespace(void) {
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	current_ns = current_ns->leave_namespace();
	// should always be safe, right?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive, adds namespace to search list
// error possible
name_space*
context::using_namespace(const id_expr& id) {
	name_space* ret = current_ns->add_using_directive(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adds a using namespace directive under a different local name
name_space*
context::alias_namespace(const id_expr& id, const string& a) {
	name_space* ret = current_ns->add_using_alias(id, a);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Modifies the context's current type definition (current_dt) 
	if the referenced type is resolved without error.  
	\param id the name of the type (unqualified).  
	\return pointer to defined or declared type if unique found, 
		else NULL of no match or ambiguous.  
 */
type_definition*
context::set_type_def(const token_string& id) {
	// lookup type (will be built-in int or bool)
	assert(current_ns);
	type_definition* ret = current_ns->lookup_unique_type(id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} else {
		current_dt = ret;
		indent++;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the current type definition to NULL.
 */
void
context::unset_type_def(void) {
	if (current_dt) {
		indent--;
		current_dt = NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

type_instantiation*
context::add_type_instance(const token_identifier& id) {
	type_instantiation* ret;
	assert(current_ns);
	assert(current_dt);
	ret = current_ns->add_type_instantiation(*current_dt, id);
	if (!ret) {
		type_error_count++;
		cerr << id.where() << endl;
		exit(1);			// temporary
	} 
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
};	// end namespace entity
};	// end namespace ART

