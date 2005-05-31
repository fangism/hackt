/**
	\file "AST/art_parser_chp_fwd.h"
	Forward declarations for the CHP namespace.  
	$Id: art_parser_chp_fwd.h,v 1.3.6.1 2005/05/31 04:00:05 fang Exp $
 */

#ifndef	__AST_ART_PARSER_CHP_FWD_H__
#define	__AST_ART_PARSER_CHP_FWD_H__

#include "AST/art_parser_fwd.h"

namespace ART {
namespace parser {
namespace CHP {
// the following class are defined in "art_parser_chp.h"

class body;
class statement;
#if 0
typedef	node_list<const statement>		stmt_list_base;
class stmt_list;
#else
typedef	node_list<const statement>		stmt_list;
#endif
class skip;
class log;
class loop;
class do_until;
class wait;
class selection;
class det_selection;
class nondet_selection;
class guarded_command;
class else_clause;
class comm_list;
class communication;
class send;
class receive;
class assignment;
class incdec_stmt;

}	// end namespace CHP
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_CHP_FWD_H__

