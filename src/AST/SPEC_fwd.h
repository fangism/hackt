/**
	\file "AST/SPEC_fwd.h"
	$Id: SPEC_fwd.h,v 1.2 2006/02/04 06:43:15 fang Exp $
 */

#ifndef	__HAC_AST_SPEC_FWD_H__
#define	__HAC_AST_SPEC_FWD_H__

#include "AST/AST_fwd.h"

namespace HAC {
namespace parser {
namespace SPEC {

class body;
class directive;
typedef	node_list<const directive>	directive_list;

}	// end namespace SPEC
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_SPEC_FWD_H__

