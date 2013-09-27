/**
	\file "AST/RTE_fwd.hh"
	Forward declarations for the RTE namespace.  
 */

#ifndef	__HAC_AST_RTE_FWD_H__
#define	__HAC_AST_RTE_FWD_H__

#include "AST/AST_fwd.hh"

namespace HAC {
namespace parser {
namespace RTE {
// the following class are defined in "AST/RTE.hh"

class body;
class body_item;
class assignment;
typedef	node_list<const body_item>	assignment_list;
/**
class loop;
class conditional;
class guarded_body;
**/

}	// end namespace RTE
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_RTE_FWD_H__

