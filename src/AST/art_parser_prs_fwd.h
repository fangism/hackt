/**
	\file "art_parser_prs_fwd.h"
	Forward declarations for the PRS namespace.  
	$Id: art_parser_prs_fwd.h,v 1.1 2005/03/06 22:45:50 fang Exp $
 */

#ifndef	__ART_PARSER_PRS_FWD_H__
#define	__ART_PARSER_PRS_FWD_H__

#include "art_parser_fwd.h"

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

#endif	// __ART_PARSER_PRS_FWD_H__

