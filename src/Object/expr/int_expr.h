/**
	\file "Object/expr/int_expr.h"
	Base class for integer data expressions.  
	NOTE: for revision histry purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the ARTXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: int_expr.h,v 1.2.22.1 2005/10/13 01:27:03 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_EXPR_H__
#define	__OBJECT_EXPR_INT_EXPR_H__

#include "Object/expr/data_expr.h"
#include "Object/expr/nonmeta_index_expr_base.h"

namespace ART {
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
#if USE_EXPR_DUMP_CONTEXT
	dump(ostream& o, const expr_dump_context&) const = 0;
#else
	dump(ostream&) const = 0;

virtual	ostream&
	dump_brief(ostream&) const = 0;
#endif

};	// end class int_expr

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_INT_EXPR_H__

