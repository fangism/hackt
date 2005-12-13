/**
	\file "AST/HSE_fwd.h"
	Forward declarations for the HSE namespace.  
	$Id: HSE_fwd.h,v 1.2 2005/12/13 04:14:47 fang Exp $
	This used to be the following before it was renamed:
	Id: art_parser_hse_fwd.h,v 1.3.52.1 2005/12/11 00:45:07 fang Exp
 */

#ifndef	__HAC_AST_HSE_FWD_H__
#define	__HAC_AST_HSE_FWD_H__

#include "AST/AST_fwd.h"

namespace HAC {
namespace parser {
namespace HSE {
// the following class are defined in "AST/HSE.h"

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

#endif	// __HAC_AST_HSE_FWD_H__

