// "art_main.cc"

#include <iostream>
#include "getopt_portable.h"		// for getopt()

#include "ptrs.h"
#include "art_debug.h"
#include "art_parser.h"
#include "art_parser_chp.h"		// need forward declarations for these
#include "art_parser_hse.h"
#include "art_parser_prs.h"
#include "art_object.h"
#include "art_symbol_table.h"
using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
#include "art_switches.h"
#include "y.tab.h"

using namespace std;

extern  YYSTYPE yyval;			// root token
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
}

int main(int argc, char* argv[]) {
	excl_ptr<parser::node> root;		///< root of the syntax tree
	never_const_ptr<entity::object> top;	///< root type-checked object
	excl_ptr<entity::name_space> global(new name_space(""));
	excl_ptr<parser::context> the_context(new context(global));

		yyparse();
DEBUG(DEBUG_BASIC, 
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
)
	root = excl_ptr<parser::node>(yyval._root_body);
if (root) {
DEBUG(DEBUG_BASIC, 
	root->what(cerr << "root is a ") << endl;	// what's our top root?
	cerr << "----------------------------------------------------";
)

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	top = root->check_build(the_context);	// useless return value
}	// end if (root)
DEBUG(DEBUG_BASIC, cerr << endl)

	if (top) {
		assert(global == the_context->get_current_namespace());
DEBUG(DEBUG_BASIC, top->dump(cerr))
	}
	// massive recursive deletion of syntax tree, reclaim memory
	// root will delete itself (also recursively)
	// global will delete itself (also recursively)
	return 0;
}

