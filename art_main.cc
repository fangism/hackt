// "art_main.cc"

#include <iostream>
#include <unistd.h>			// for getopt()

#include "art_parser.h"
using namespace ART::parser;
#include "art_switches.h"
#include "y.tab.h"

using namespace std;

extern  YYSTYPE yyval;			// root token
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
	void yyerror(const char*);
}

int main(int argc, char* argv[]) {
	node* root;
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

	// massive recursive deletion of syntax tree
	SAFEDELETE(root);
	return 0;
}

