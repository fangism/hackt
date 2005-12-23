/**
	\file "Object/lang/cflat_visitor.h"
	$Id: cflat_visitor.h,v 1.1.2.1 2005/12/23 05:44:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__
#define	__HAC_OBJECT_LANG_CFLAT_VISITOR_H__

namespace HAC {
namespace entity {
namespace PRS {
// forward declarations of all the visitable types in this hierarchy
class footprint;
class footprint_expr_node;
class footprint_rule;

//=============================================================================
/**
	Base class from which other functional visitors are derived.  
	TODO: default visit behavior for non-terminal types.  
 */
struct cflat_visitor {
virtual	~cflat_visitor() { }
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

