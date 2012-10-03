/**
	\file "main/spice-grind.cc"
	$Id: $
	Program that reads in spice file and converts it to other formats.
 */

#include <iostream>
#include <cstdio>			// for stdin
// #include "main/spice-grind.h"
#include "net/netlist_generator.h"
#include "parser/spice-parse-real.h"
#include "lexer/file_manager.h"
#include "lexer/yyin_manager.h"
#include "lexer/flex_lexer_state.h"
#include "util/syntax.h"
#include "main/main_funcs.h"

using util::memory::count_ptr;
using util::syntax::node;
using namespace std;

extern	int	spice_parse(void*, YYSTYPE&, flex::lexer_state&);
extern	HAC::lexer::file_manager	spice_parse_file_manager;
extern	HAC::lexer::embedded_file_stack_type	spice_embedded_file_stack;

// publicize later?
static
count_ptr<node>
parse_spice_to_AST(FILE* yyin) {
	typedef	count_ptr<node>		return_type;
	YYSTYPE lval;
	NEVER_NULL(yyin);
	try {
		flex::lexer_state f(yyin);
		spice_parse(NULL, lval, f);
	} catch (...) {
		return return_type(NULL);
	}
	return return_type(lval._list);
	
}

int
main(int argc, char* argv[]) {
	FILE* infile = stdin;
	// any options?
	if (argc > 2) {
		cout << "usage:\n"
			"spice-grind [FILE]\n"
			"spice-grind < FILE" << endl;
		return 1;
	}
	if (argc == 2) {
		infile = fopen(argv[1], "r");
		if (!infile) {
			cerr << "Error opening file for reading: " << argv[1]
				<< endl;
			return 1;
		}
	}
	const count_ptr<node> r(parse_spice_to_AST(infile));
	r->dump(cout) << endl;
	fclose(infile);
	return 0;
}

