/**
	\file "art_parser_hse_fwd.h"
	Forward declarations for the HSE namespace.  
	$Id: art_parser_hse_fwd.h,v 1.1 2005/03/06 22:45:50 fang Exp $
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
typedef	node_list<const statement,semicolon>	stmt_list;
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

