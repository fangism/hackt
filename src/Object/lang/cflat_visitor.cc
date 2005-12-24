/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.1.2.2 2005/12/24 02:33:35 fang Exp $
 */

#include "Object/lang/cflat_visitor.h"
#include "Object/lang/PRS_footprint.h"

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================
// struct cflat_visitor class definition

/**
	Possibly move class definition into header, leaving methods
	in this file.  
 */
class cflat_visitor::expr_pool_setter {
private:
	cflat_visitor&                          cfv;
public:
	expr_pool_setter(cflat_visitor& _cfv, const footprint& _fp) :
			cfv(_cfv) {
		cfv.expr_pool = &_fp.get_expr_pool();
	}

	~expr_pool_setter() {
		cfv.expr_pool = NULL;
	}
};      // end struct expr_pool_setter

//=============================================================================
/**
	Default traversal for cflat_visitor over the PRS::footprint.  
 */
void
cflat_visitor::visit(const footprint& f) {
	typedef	footprint::rule_pool_type::const_iterator
						const_rule_iterator;
	const expr_pool_setter temp(*this, f);	// will expire end of scope
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

