// "art++.h"

#ifndef	__ARTXX_H__
#define	__ARTXX_H__

#include "getopt_portable.h"		// for getopt()

#include "ptrs.h"
#include "art_debug.h"
#include "art_parser.h"			// everything needed for "y.tab.h"
#include "art_object_base.h"
#include "art_object_IO.h"
#include "art_context.h"
using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
#include "art_switches.h"
#include "yacc_or_bison.h"
#include "art++-parse-prefix.h"

#if	USING_YACC
extern  YYSTYPE artxx_val;		// root token (was yyval)
#elif	USING_BISON
extern	excl_ptr<root_body>	AST_root;
#else
#error	"USING_YACC or USING_BISON?  Both can't be false."
#endif
extern	FILE* artxx_in;
extern "C" {
	int artxx_parse(void);		// in "art++-parse-prefix.cc"
}

using namespace std;

#endif	// __ARTXX_H__

