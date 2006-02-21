/**
	\file "Object/expr/meta_range_expr.h"
	Base class related to lists of meta expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: meta_range_expr.h,v 1.7 2006/02/21 04:48:24 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_META_RANGE_EXPR_H__
#define __HAC_OBJECT_EXPR_META_RANGE_EXPR_H__

#include "Object/expr/meta_index_expr.h"
#include "Object/expr/nonmeta_range_expr_base.h"
#include "util/boolean_types.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_range;
class const_index;
class unroll_context;
using util::good_bool;
using util::bad_bool;

//=============================================================================
/**
	Abstract interface to range expression objects.  
	Note: meta_range_expr is not a param_expr.
	Range expressions are not the same as index expressions:
	Range expressions are used exclusively for declarations of arrays, 
	whereas index expressions are used for instance references.  
	Since the syntax allows declaration of x[N] as a short-hand for 
	x[0..N-1], we will need to explicitly convert from the former 
	to the latter.  
 */
class meta_range_expr : virtual public meta_index_expr, 
		public nonmeta_range_expr_base {
	typedef	meta_range_expr		this_type;
	typedef	meta_index_expr		parent_type;
	typedef	nonmeta_range_expr_base	nonmeta_parent_type;
protected:
	meta_range_expr() : parent_type(), nonmeta_parent_type() { }

public:
virtual	~meta_range_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

/**
	A range is always 0-dimensional, which is not the same
	as the dimensions *represented* by the range.  
	Is virtual so that sub-classes that use the index interface can
	override this.  
 */
virtual	size_t
	dimensions(void) const { return 0; }

/** is initialized if is resolved to constant or some other formal */
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

/** is sane if range makes sense */
virtual	bool
	is_sane(void) const = 0;

/** can be resolved to static constant value */
virtual bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

	count_ptr<const_index>
	resolve_index(void) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

virtual	good_bool
	resolve_range(const_range& r) const = 0;

virtual	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const = 0;

virtual	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const = 0;

	bool
	must_be_equivalent_index(const meta_index_expr& ) const;

	static
	count_ptr<const this_type>
	make_explicit_range(const count_ptr<const parent_type>&);
};	// end class meta_range_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_META_RANGE_EXPR_H__

