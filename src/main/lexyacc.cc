/**
	\file "main/lexyacc.cc"
	Test program using unmangled, unhacked flex/yacc code.
	$Id: lexyacc.cc,v 1.2 2009/02/20 20:39:41 fang Exp $
 */

#define	ENABLE_STATIC_TRACE		0
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "parser/hackt-union.h"	// for forward declarations
#include "parser/hackt-parse.h"	// for YYSTYPE
#include "AST/root.h"
#include "util/memory/count_ptr.h"

using util::memory::count_ptr;
using HAC::parser::root_body;

extern
root_body*
global_hackt_lval;

extern
int
hackt_parse(void);

int
main(int argc, char* argv[]) {
	count_ptr<root_body> p;
	try {
		hackt_parse();
		p = count_ptr<root_body>(global_hackt_lval);
	} catch (...) {
		return 1;
	}
	return 0;
}

DEFAULT_STATIC_TRACE_END

