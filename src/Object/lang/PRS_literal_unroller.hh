/**
	\file "Object/lang/PRS_literal_unroller.hh"
	$Id: PRS_literal_unroller.hh,v 1.2 2006/02/04 06:43:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_LITERAL_UNROLLER_H__
#define	__HAC_OBJECT_LANG_PRS_LITERAL_UNROLLER_H__

#include "Object/lang/PRS.hh"

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================
/**
	Functor: Resolves local node index.  
	Binds the unroll_context argument.  
 */
struct literal::unroller {
	const unroll_context& _context;

	explicit
	unroller(const unroll_context& c) : _context(c) { }

	// make sure argument pointer type matches
	// macro::const_reference
	size_t
	operator () (const count_ptr<const literal>& l) const {
		NEVER_NULL(l);
		return l->unroll_node(_context);
	}
	
};      // end struct unroller

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_LITERAL_UNROLLER_H__

