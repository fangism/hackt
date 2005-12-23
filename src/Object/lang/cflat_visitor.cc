/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.1.2.1 2005/12/23 05:44:08 fang Exp $
 */

#include "Object/lang/cflat_visitor.h"
#include "Object/lang/PRS_footprint.h"

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================
/**
	Default traversal for cflat_visitor over the PRS::footprint.  
 */
void
cflat_visitor::visit(const footprint& f) {
	typedef	footprint::rule_pool_type::const_iterator
						const_rule_iterator;
	const_rule_iterator i(f.rule_pool.begin());
	const const_rule_iterator e(f.rule_pool.end());
	for ( ; i!=e; i++) {
		i->accept(*this);
	}
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

