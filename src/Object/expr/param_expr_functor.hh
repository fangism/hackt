/**
	\file "Object/expr/param_expr_functor.hh"
	Handy functor definitions for param_expr.  
	$Id: param_expr_functor.hh,v 1.4 2006/07/04 07:25:59 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_FUNCTOR_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_FUNCTOR_H__

#include "Object/expr/param_expr.hh"
#include "util/memory/count_ptr.hh"
#include "Object/expr/const_param.hh"

//=============================================================================
namespace HAC {
namespace entity {

//=============================================================================
/**
	Helper functor for unrolling a sequence of param_expr pointers.  
 */
struct param_expr::unroller {
	typedef	count_ptr<const const_param>		return_type;
	const unroll_context&	_context;

	explicit
	unroller(const unroll_context& c) : _context(c) { }

	template <class P>
	return_type
	operator () (const P& p) const {
		return (p ? p->unroll_resolve_rvalues(_context, p) : 
			return_type(NULL));
	}
};	// end struct unroller

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_FUNCTOR_H__

