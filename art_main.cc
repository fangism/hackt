// "art_main.cc"

#include <iostream>
#include <unistd.h>			// for getopt()

#include "art_parser.h"
using namespace ART;
#include "art_switches.h"
#include "y.tab.h"

using namespace std;

extern  YYSTYPE yyval;			// root token
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
	void yyerror(const char*);
}

int main(int argc, char* argv[]) {
	parser::node* root;	///< root of the syntax tree
	entity::object* top;	///< root of type-checked and bound object
	try {
		yyparse();
	} catch (...) {
		yyerror("uncaught exception in parsing");
	}
	cerr << "We have a root node! perhaps we should type-check it..."
		<< endl;
	root = yyval.n;
	if (root)
		root->what(cerr) << endl;	// what's our top root?

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	top = root->check_build(NULL);

	// massive recursive deletion of syntax tree, reclaim memory
	SAFEDELETE(root);

	return 0;
}

