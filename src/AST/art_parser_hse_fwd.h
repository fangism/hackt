/**
	\file "AST/art_parser_hse_fwd.h"
	Forward declarations for the HSE namespace.  
	$Id: art_parser_hse_fwd.h,v 1.3.52.1 2005/12/11 00:45:07 fang Exp $
 */

#ifndef	__AST_HAC_PARSER_HSE_FWD_H__
#define	__AST_HAC_PARSER_HSE_FWD_H__

#include "AST/art_parser_fwd.h"

namespace HAC {
namespace parser {
namespace HSE {
// the following class are defined in "art_parser_hse.h"

class body;
class statement;
typedef	node_list<const statement>		stmt_list;
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
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_HSE_FWD_H__

