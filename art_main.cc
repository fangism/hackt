// "art_main.cc"

#include <iostream>
#include "getopt_portable.h"		// for getopt()

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
//	void yyerror(const char*);	// ancient compiler rejects
//	void yyerror(char*);	// replace with this if necessary
}

int main(int argc, char* argv[]) {
	parser::node* root;	///< root of the syntax tree
	/// root of type-checked object
	const entity::object* top = NULL;
	entity::name_space* global = new name_space("",NULL);
	parser::context* the_context = new context(global);

//	try {
		yyparse();
//	} catch (...) {
//		cerr << "uncaught exception in parsing";
//		exit(1);
//	}
DEBUG(DEBUG_BASIC, 
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
)
	root = yyval._root_body;
if (root) {
DEBUG(DEBUG_BASIC, 
	root->what(cerr << "root is a ") << endl;	// what's our top root?
	cerr << "----------------------------------------------------";
)

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	// global = new context();
	top = root->check_build(the_context);
}	// end if (root)
DEBUG(DEBUG_BASIC, cerr << endl)

	// massive recursive deletion of syntax tree, reclaim memory
	SAFEDELETE(root);

	// IMPORTANT: should be the same, don't delete both
	if (top) assert(top == global);
	// delete ART object and its hierarchy
	SAFEDELETE(global);
	return 0;
}

