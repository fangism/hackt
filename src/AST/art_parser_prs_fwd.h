/**
	\file "AST/art_parser_prs_fwd.h"
	Forward declarations for the PRS namespace.  
	$Id: art_parser_prs_fwd.h,v 1.2 2005/05/10 04:51:08 fang Exp $
 */

#ifndef	__AST_ART_PARSER_PRS_FWD_H__
#define	__AST_ART_PARSER_PRS_FWD_H__

#include "AST/art_parser_fwd.h"

namespace ART {
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
}	// end namespace ART

#endif	// __AST_ART_PARSER_PRS_FWD_H__

