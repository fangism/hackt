/**
	\file "art++.h"
	The only header file needed to write top-level applications for ART.

	$Id: art++.h,v 1.11 2005/05/20 19:28:30 fang Exp $
 */

#ifndef	__ARTXX_H__
#define	__ARTXX_H__

#include "util/getopt_portable.h"	// for getopt()

#include "util/memory/pointer_classes.h"
#include "AST/art_parser_fwd.h"		// everything needed for "y.tab.h"
#include "AST/art_parser_chp_fwd.h"	// everything needed for "y.tab.h"
#include "AST/art_parser_hse_fwd.h"	// everything needed for "y.tab.h"
#include "AST/art_parser_prs_fwd.h"	// everything needed for "y.tab.h"
#include "AST/art_parser_root_item.h"	// everything needed for "y.tab.h"
#include "Object/art_object_base.h"
#include "Object/art_object_module.h"
#include "Object/art_context.h"
#include "util/persistent_object_manager.h"

using namespace ART;
using namespace ART::entity;
using namespace ART::parser;
using namespace util::memory;
using util::persistent_object_manager;

#include "parser/yacc_or_bison.h"
#include "parser/art++-parse-prefix.h"

#if	USING_YACC
extern  YYSTYPE artxx_val;		// root token (was yyval)
#elif	USING_BISON
extern	excl_ptr<root_body>	AST_root;
#else
#error	"USING_YACC or USING_BISON?  Both can't be false."
#endif
extern	FILE* artxx_in;

// extern "C" {
int	artxx_parse(void);		// in "art++-parse-prefix.cc"
// }

#include "util/using_ostream.h"
using std::ifstream;
using std::ofstream;
using std::ios_base;

#endif	// __ARTXX_H__

