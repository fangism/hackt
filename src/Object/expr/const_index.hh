/**
	\file "Object/expr/const_index.hh"
	Classes related to constant index expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_index.hh,v 1.7 2007/11/26 20:11:10 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_INDEX_H__
#define __HAC_OBJECT_EXPR_CONST_INDEX_H__

#include "Object/expr/meta_index_expr.hh"
#include "Object/expr/types.hh"

namespace HAC {
namespace entity {
class const_range;
//=============================================================================
/**
	Abstract interface for constant indices and index ranges.  
 */
class const_index : virtual public meta_index_expr {
	typedef	meta_index_expr			parent_type;
protected:
	const_index() : parent_type() { }

public:
virtual	~const_index() { }

// same pure virtual functions, and more...
virtual	bool
	negative(void) const = 0;

virtual	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const = 0;

virtual	pint_value_type
	lower_bound(void) const = 0;

virtual	pint_value_type
	upper_bound(void) const = 0;

virtual	bool
	operator == (const const_range& c) const = 0;

virtual	bool
	range_size_equivalent(const const_index& i) const = 0;

};	// end class const_index

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_INDEX_H__

