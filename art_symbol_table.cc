// "art_symbol_table.cc"

#include <assert.h>

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
		// create the global namespace
		current_ns(new name_space("",NULL)) {
	assert(current_ns);		// make sure allocated
	// add to the global namespace all built-in types and definitions
}

/// currently, default destructor
context::~context() {
	// don't delete current_ns... yet

}

/// opens up namespace
name_space*
context::open_namespace(const string& id) {
	indent++;
	return current_ns->add_open_namespace(id);
}

/// closes namespace, leaving the scope
name_space*
context::close_namespace(void) {
	indent--;
	// null out member pointers to other sub structures: 
	//	types, definitions...
	return current_ns->leave_namespace();
}

/// adds a using namespace directive, adds namespace to search list
name_space*
context::using_namespace(const id_expr& id) {
	return current_ns->add_using_directive(id);
}

/// adds a using namespace directive under a different local name
name_space*
context::alias_namespace(const id_expr& id, const string& a) {
	// obviously not done
	return NULL;
}

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

