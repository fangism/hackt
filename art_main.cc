// "art_main.cc"

#include <iostream>
#include "art_parser.h"
using namespace ART::parser;
#include "art_switches.h"
#include "y.tab.h"

using namespace std;

extern  YYSTYPE yyval;			// root token
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
}

int main(int argc, char* argv[]) {
	yyparse();
	return 0;
}

