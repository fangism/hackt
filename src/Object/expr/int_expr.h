/**
	\file "Object/expr/int_expr.h"
	Base class for integer data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: int_expr.h,v 1.5 2006/01/22 18:19:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_INT_EXPR_H__
#define	__HAC_OBJECT_EXPR_INT_EXPR_H__

#include "Object/expr/data_expr.h"
#include "Object/expr/nonmeta_index_expr_base.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Abstract boolean data type expression.  
	NOTE: may not be useable as an index, 
		depending on CHP language resolution.  
 */
class int_expr : virtual public nonmeta_index_expr_base, public data_expr {
	typedef	data_expr			parent_type;
protected:
	int_expr() : nonmeta_index_expr_base(), parent_type() { }
public:
virtual	~int_expr() { }

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

};	// end class int_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_INT_EXPR_H__

