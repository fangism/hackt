// "art++.h"

#ifndef	__ARTXX_H__
#define	__ARTXX_H__

#include "getopt_portable.h"		// for getopt()

#include "ptrs.h"
#include "art_debug.h"
#include "art_parser.h"			// everything needed for "y.tab.h"
#include "art_object_base.h"
#include "art_object_IO.h"
#include "art_symbol_table.h"
using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
#include "art_switches.h"
#include "y.tab.h"

extern  YYSTYPE yyval;			// root token
extern	FILE* yyin;
extern "C" {
	int yyparse(void);              // in "y.tab.cc"
}

using namespace std;

#endif	// __ARTXX_H__

