/**
	\file "AST/CHP_fwd.h"
	Forward declarations for the CHP namespace.  
	$Id: CHP_fwd.h,v 1.8.32.1 2010/09/20 18:37:22 fang Exp $
	This file used ot be the following before rename:
	Id: art_parser_chp_fwd.h,v 1.4.42.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef	__HAC_AST_CHP_FWD_H__
#define	__HAC_AST_CHP_FWD_H__

#include "AST/AST_fwd.h"

/**
	Ancient CHP support log() statments, which have now been deprecated,
	in favor of a log() function meta-function-call.
 */
#define	OLD_CHP_LOG				0

namespace HAC {
namespace parser {
class expr;
namespace CHP {
// the following class are defined in "AST/CHP.h"

typedef	expr	chp_expr;
class body;
class statement;
typedef	node_list<const statement>		stmt_list_base;
class stmt_list;
class skip;
#if OLD_CHP_LOG
class log;
#endif
class loop;
class do_until;
class wait;
class selection;
class metaloop_selection;
class metaloop_statement;
class det_selection;
class nondet_selection;
class guarded_command;
typedef guarded_command	else_clause;
class communication;
class send;
class receive;
class binary_assignment;
class bool_assignment;
class probe_expr;
class function_call_expr;

}	// end namespace CHP
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_CHP_FWD_H__

