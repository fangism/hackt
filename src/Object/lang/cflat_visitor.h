/**
	\file "Object/lang/cflat_visitor.h"
	$Id: cflat_visitor.h,v 1.1.2.3 2005/12/30 17:41:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__

#include "Object/lang/PRS_footprint_expr_pool_fwd.h"

namespace HAC {
namespace entity {
class state_manager;
namespace PRS {
// forward declarations of all the visitable types in this hierarchy
class footprint;
class footprint_expr_node;
class footprint_rule;

//=============================================================================
/**
	Base class from which other functional visitors are derived.  
	TODO: default visit behavior for non-terminal types.  
	No need to include state_manager -- its traversal is fixed.  
 */
class cflat_visitor {
protected:
	/**
		This needs to be set by the visit to the footprint.  
		Will initially be NULL, before the PRS footprint is entered. 
	 */
	const PRS_footprint_expr_pool_type*		expr_pool;

	/// helper class for maintaining expr_pool
	class expr_pool_setter;
public:
	cflat_visitor() : expr_pool(NULL) { }
virtual	~cflat_visitor() { }

#if 0
virtual	void
	visit(const state_manager&);
#endif
virtual	void
	visit(const footprint&);
virtual	void
	visit(const footprint_rule&) = 0;
virtual	void
	visit(const footprint_expr_node&) = 0;

};	// end struct cflat_visitor

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_VISITOR_H__

