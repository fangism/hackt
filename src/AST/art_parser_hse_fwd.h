/**
	\file "art_parser_hse_fwd.h"
	Forward declarations for the HSE namespace.  
	$Id: art_parser_hse_fwd.h,v 1.1.8.1 2005/03/12 03:43:07 fang Exp $
 */

#ifndef	__ART_PARSER_HSE_FWD_H__
#define	__ART_PARSER_HSE_FWD_H__

#include "art_parser_fwd.h"

namespace ART {
namespace parser {
namespace HSE {
// the following class are defined in "art_parser_hse.h"

class body;
class statement;
#if USE_NEW_NODE_LIST
typedef	node_list<const statement>		stmt_list;
#else
typedef	node_list<const statement,semicolon>	stmt_list;
#endif
class skip;
class loop;
class do_until;
class wait;
class selection;
class det_selection;
class nondet_selection;
class guarded_command;
class else_clause;
class assignment;

}	// end namespace HSE
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_HSE_FWD_H__

