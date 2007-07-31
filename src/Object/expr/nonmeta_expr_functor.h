/**
	\file "Object/expr/nonmeta_expr_functor.h"
	Handy functor definitions for nonmeta_expr.  
	$Id: nonmeta_expr_functor.h,v 1.2 2007/07/31 23:23:23 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_NONMETA_EXPR_FUNCTOR_H__
#define __HAC_OBJECT_EXPR_NONMETA_EXPR_FUNCTOR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param.h"

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
/**
	Helper functor for unrolling a sequence of nonmeta_expr pointers.  
 */
struct data_expr::unroller {
	typedef	count_ptr<const data_expr>		return_type;
	const unroll_context&				_context;

	explicit
	unroller(const unroll_context& c) : _context(c) { }

	return_type
	operator () (const return_type& p) const {
		return (p ? p->unroll_resolve_copy(_context, p) : 
			return_type(NULL));
	}
};	// end struct unroller

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper functor for unrolling a sequence of nonmeta_expr pointers.  
 */
struct data_expr::resolver {
	typedef	count_ptr<const const_param>		return_type;
	const nonmeta_context_base&			_context;

	explicit
	resolver(const nonmeta_context_base& c) : _context(c) { }

	return_type
	operator () (const count_ptr<const data_expr>& p) const {
		return (p ? p->nonmeta_resolve_copy(_context, p) : 
			return_type(NULL));
	}
};	// end struct unroller

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_NONMETA_EXPR_FUNCTOR_H__

