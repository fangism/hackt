/**
	\file "AST/SPEC_fwd.hh"
	$Id: SPEC_fwd.hh,v 1.3 2008/10/31 02:11:41 fang Exp $
 */

#ifndef	__HAC_AST_SPEC_FWD_H__
#define	__HAC_AST_SPEC_FWD_H__

#include "AST/AST_fwd.hh"

namespace HAC {
namespace parser {
namespace SPEC {

class body;
class directive_base;
class directive;
class invariant;
typedef	node_list<const directive_base>	directive_list;

}	// end namespace SPEC
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_SPEC_FWD_H__

