/**
	\file "AST/art_parser_hse_fwd.h"
	Forward declarations for the HSE namespace.  
	$Id: art_parser_hse_fwd.h,v 1.3 2005/05/10 04:51:08 fang Exp $
 */

#ifndef	__AST_ART_PARSER_HSE_FWD_H__
#define	__AST_ART_PARSER_HSE_FWD_H__

#include "AST/art_parser_fwd.h"

namespace ART {
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
}	// end namespace ART

#endif	// __AST_ART_PARSER_HSE_FWD_H__

