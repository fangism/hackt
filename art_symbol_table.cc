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

name_space*
context::close_namespace(void) {
	indent--;
	return current_ns->leave_namespace();
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

