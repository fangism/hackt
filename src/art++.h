/**
	\file "art++.h"
	The only header file needed to write top-level applications for ART.

	$Id: art++.h,v 1.7 2005/02/27 22:54:07 fang Exp $
 */

#ifndef	__ARTXX_H__
#define	__ARTXX_H__

#include "getopt_portable.h"		// for getopt()

#include "memory/pointer_classes.h"
#include "art_debug.h"
#include "art_parser.h"			// everything needed for "y.tab.h"
#include "art_object_base.h"
#include "art_object_module.h"
#include "art_context.h"
#include "persistent_object_manager.h"

using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
using namespace util::memory;
using util::persistent_object_manager;

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
int	artxx_parse(void);		// in "art++-parse-prefix.cc"
}

using namespace std;

#endif	// __ARTXX_H__

