// "art_main.cc"

#include <iostream>
#include <unistd.h>			// for getopt()

#include "art_parser.h"
#include "art_object.h"
#include "art_symbol_table.h"
using namespace ART;
#include "art_switches.h"
#include "y.tab.h"

using namespace std;

extern  YYSTYPE yyval;			// root token
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
	void yyerror(const char*);	// ancient compiler rejects
//	void yyerror(char*);	// replace with this if necessary
}

int main(int argc, char* argv[]) {
	parser::node* root;	///< root of the syntax tree
	entity::object* top;	///< root of type-checked and bound object
	// parser::context* global;

	try {
		yyparse();
	} catch (...) {
		yyerror("uncaught exception in parsing");
	}
	cerr << "parsing successful... tree built, on to type-checking!" 
		<< endl;
	root = yyval.n;
	if (root)
		root->what(cerr << "root is a ") << endl;
		// what's our top root?
	cerr << "----------------------------------------------------";

	// type-check, build a useful manipulable art object, and return it
	// the symbol tables will selectively retain info from the syntax tree
	// need to build global table first, then pass it in context
	// global = new context();
	top = root->check_build(new context());


	cerr << endl;
	// massive recursive deletion of syntax tree, reclaim memory
	SAFEDELETE(root);
	// delete ART object and its hierarchy
	SAFEDELETE(top);
	// delete global context
	// SAFEDELETE(global);	// no need, global is owned by top
	return 0;
}

