/**
	\file "death.h"
	The only header file needed to write top-level applications for ART.

	$Id: death.h,v 1.1.2.1 2005/02/22 03:33:18 fang Exp $
 */

#ifndef	__DEATH_H__
#define	__DEATH_H__

// #include "getopt_portable.h"		// for getopt()

// #include "memory/pointer_classes.h"
// #include "art_debug.h"


#if 0
#include "art_parser.h"			// everything needed for "y.tab.h"
#else

#if 0
#include "art_parser_base_death.h"    // minimal parser interface base classes
#else
namespace ART {
namespace entity {
}
using namespace entity;
}
#endif

#if 0
#include "art_parser_expr.h"
#include "art_parser_token.h"
#include "art_parser_formal.h"
#include "art_parser_definition.h"
#include "art_parser_instance.h"
#include "art_parser_prs.h"
#include "art_parser_chp.h"
#include "art_parser_hse.h"
#endif
#endif


#if 0
#include "art_object_base_death.h"
#else
namespace util { }
namespace ART {
namespace entity {
using namespace util;
}
}
#endif
// #include "art_object_module.h"
// #include "art_context.h"
#include "persistent_object_manager_death.h"	// needed for death

#if 0
using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
#endif
#if 0
using namespace util::memory;
#else
using namespace util:memory;	
	// this typo brings darwin-gcc-3.3 (cc1plus) to its knees
	// exhausting virtual memory
#endif

#include "art_switches.h"		// needed for death
// #include "yacc_or_bison.h"
// #include "art++-parse-prefix.h"

#if 0
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
#endif

// using namespace std;

#endif	// __DEATH_H__

