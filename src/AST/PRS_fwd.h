/**
	\file "AST/PRS_fwd.h"
	Forward declarations for the PRS namespace.  
	$Id: PRS_fwd.h,v 1.4 2006/02/10 21:50:34 fang Exp $
	This used to be the following before it was renamed:
	Id: art_parser_prs_fwd.h,v 1.2.52.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_PRS_FWD_H__
#define	__HAC_AST_PRS_FWD_H__

#include "AST/AST_fwd.h"

namespace HAC {
namespace parser {
namespace PRS {
// the following class are defined in "AST/PRS.h"

class body;
class body_item;
class literal;
class rule;
typedef	node_list<const body_item>	rule_list;
class loop;
class macro;
class attribute;
typedef	node_list<const attribute>	attribute_list;

}	// end namespace PRS
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_PRS_FWD_H__

