/**
	\file "AST/art_parser_prs_fwd.h"
	Forward declarations for the PRS namespace.  
	$Id: art_parser_prs_fwd.h,v 1.2.52.1 2005/12/11 00:45:09 fang Exp $
 */

#ifndef	__AST_HAC_PARSER_PRS_FWD_H__
#define	__AST_HAC_PARSER_PRS_FWD_H__

#include "AST/art_parser_fwd.h"

namespace HAC {
namespace parser {
namespace PRS {
// the following class are defined in "art_parser_prs.h"

class body;
class body_item;
class rule;
typedef	node_list<const body_item>	rule_list;
class loop;

}	// end namespace PRS
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_PRS_FWD_H__

