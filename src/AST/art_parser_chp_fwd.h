/**
	\file "art_parser_chp_fwd.h"
	Forward declarations for the CHP namespace.  
	$Id: art_parser_chp_fwd.h,v 1.1.8.1 2005/03/12 03:43:05 fang Exp $
 */

#ifndef	__ART_PARSER_CHP_FWD_H__
#define	__ART_PARSER_CHP_FWD_H__

#include "art_parser_fwd.h"

namespace ART {
namespace parser {
namespace CHP {
// the following class are defined in "art_parser_chp.h"

class body;
class statement;
#if USE_NEW_NODE_LIST
typedef	node_list<const statement>		stmt_list;
#else
typedef	node_list<const statement,semicolon>	stmt_list;
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

#endif	// __ART_PARSER_CHP_FWD_H__

