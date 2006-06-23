/**
	\file "Object/expr/int_expr.h"
	Base class for integer data expressions.  
	NOTE: for revision history purposes, this file was copied from the
		last revision of "Object/art_object_data_expr_base.h"
		on the HACXX-00-01-04-main-00-48-connect-01 branch, 
		branch revision -11.
	$Id: int_expr.h,v 1.5.40.4 2006/06/23 21:08:13 fang Exp $
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
	typedef	int_expr			this_type;
	typedef	data_expr			parent_type;
	typedef	nonmeta_index_expr_base		index_parent_type;
protected:
	int_expr() : index_parent_type(), parent_type() { }
public:
virtual	~int_expr() { }

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

#define	UNROLL_RESOLVE_COPY_INT_PROTO					\
	count_ptr<const int_expr>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const int_expr>&) const

virtual UNROLL_RESOLVE_COPY_INT_PROTO = 0;
protected:
	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO;
	UNROLL_RESOLVE_COPY_DATA_PROTO;

};	// end class int_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_INT_EXPR_H__

