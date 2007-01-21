/**
	\file "Object/expr/nonmeta_range_expr_base.h"
	Base classes for data range expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	TODO: future rename this file to nonmeta_expr_base.h
	$Id: nonmeta_range_expr_base.h,v 1.5 2007/01/21 05:58:56 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_RANGE_EXPR_BASE_H__
#define	__HAC_OBJECT_EXPR_NONMETA_RANGE_EXPR_BASE_H__

#include "Object/expr/nonmeta_index_expr_base.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Base class for general range expressions, meta or nonmeta.  
	NOTE: after CHP resolution, we may not allow nonmeta
		range expressions.  
 */
class nonmeta_range_expr_base : virtual public nonmeta_index_expr_base {
	typedef	nonmeta_index_expr_base		parent_type;
protected:
	nonmeta_range_expr_base() : parent_type() { }
public:
virtual	~nonmeta_range_expr_base() { }

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

};	// end clas nonmeta_range_expr_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_RANGE_EXPR_BASE_H__

