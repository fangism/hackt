/**
	\file "AST/CHP_fwd.h"
	Forward declarations for the CHP namespace.  
	$Id: CHP_fwd.h,v 1.7 2007/07/31 23:22:57 fang Exp $
	This file used ot be the following before rename:
	Id: art_parser_chp_fwd.h,v 1.4.42.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef	__HAC_AST_CHP_FWD_H__
#define	__HAC_AST_CHP_FWD_H__

#include "AST/AST_fwd.h"

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
class log;
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
class stmt_attribute;
typedef	node_list<const stmt_attribute>		stmt_attr_list;
class function_call_expr;

}	// end namespace CHP
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_CHP_FWD_H__

