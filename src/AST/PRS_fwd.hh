/**
	\file "AST/PRS_fwd.hh"
	Forward declarations for the PRS namespace.  
	$Id: PRS_fwd.hh,v 1.9 2009/07/20 22:41:35 fang Exp $
	This used to be the following before it was renamed:
	Id: art_parser_prs_fwd.h,v 1.2.52.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_PRS_FWD_H__
#define	__HAC_AST_PRS_FWD_H__

#include "AST/AST_fwd.hh"

namespace HAC {
namespace parser {
namespace PRS {
// the following class are defined in "AST/PRS.hh"

class body;
class subcircuit;
class body_item;
class literal;
class precharge;
class rule;
typedef	node_list<const body_item>	rule_list;
class loop;
class conditional;
class guarded_body;
class macro;
typedef	node_list<const guarded_body>	guarded_prs_list;

}	// end namespace PRS
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_PRS_FWD_H__

