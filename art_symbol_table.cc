// "art_symbol_table.cc"

#include <assert.h>

#include "art_parser.h"
#include "art_symbol_table.h"
#include "art_object.h"

namespace ART {
namespace parser {
using namespace std;

//=============================================================================
// class context method definition

/// default constructor
context::context() : 
		indent(0),		// reset formatting indentation
		type_error_count(0), 	// type-check error count
		// create the global namespace
		current_ns(new name_space("",NULL)) {
	assert(current_ns);		// make sure allocated
	// add to the global namespace all built-in types and definitions
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// currently, default destructor
context::~context() {
	// don't delete current_ns... yet

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// opens up namespace, error occurs if name conflicts
name_space*
context::open_namespace(const string& id) {
	indent++;
	current_ns = current_ns->add_open_namespace(id);
	if (!current_ns) {
		type_error_count++;
		cerr << id << endl;		// where?
		exit(1);			// temporary
	}
	else return current_ns;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// closes namespace, leaving the scope
name_space*
context::close_namespace(void) {
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	current_ns = current_ns->leave_namespace();
	return current_ns;
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

