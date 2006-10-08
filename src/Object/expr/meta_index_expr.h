/**
	\file "Object/expr/meta_index_expr.h"
	Base class related to lists of meta index expressions.
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	$Id: meta_index_expr.h,v 1.8.8.2.8.2 2006/10/08 05:52:36 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_META_INDEX_EXPR_H__
#define __HAC_OBJECT_EXPR_META_INDEX_EXPR_H__

#include "Object/expr/nonmeta_index_expr_base.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/devel_switches.h"

//=============================================================================
namespace HAC {
namespace entity {
class const_index;
class unroll_context;
#if SUBSTITUTE_DEFAULT_PARAMETERS
class param_expr;
class template_formals_manager;
class dynamic_param_expr_list;
#endif
using util::memory::count_ptr;

//=============================================================================
/**
	Abstract interface to an index (not a range), which may be 
	a single integer interpreted as a dimension-collapse x[i], 
	or a range that preserves the dimension x[i..j].  
	For example, x[i] refers to a single instance (0-dimension)
	of the x array, whereas x[i..i] refers to a 
	1-dimensional sub-array of x, size 1, in this case.  
	A "meta_range_expr" (below) may be an index, but not vice versa.  
 */
class meta_index_expr : virtual public nonmeta_index_expr_base {
	typedef	nonmeta_index_expr_base		parent_type;
	typedef	meta_index_expr			this_type;
protected:
	meta_index_expr() : parent_type() { }

public:
virtual	~meta_index_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual size_t
	dimensions(void) const = 0;

#if ENABLE_STATIC_ANALYSIS
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;
#endif

virtual bool
	is_static_constant(void) const = 0;

virtual	bool
	is_relaxed_formal_dependent(void) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	count_ptr<const_index>
	resolve_index(void) const = 0;
#endif

virtual	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const = 0;

virtual	bool
	must_be_equivalent_index(const meta_index_expr& ) const = 0;

	UNROLL_RESOLVE_COPY_NONMETA_INDEX_PROTO;

#define	UNROLL_RESOLVE_COPY_META_INDEX_PROTO				\
	count_ptr<const const_index>					\
	unroll_resolve_copy(const unroll_context&,			\
		const count_ptr<const meta_index_expr>&) const

virtual	UNROLL_RESOLVE_COPY_META_INDEX_PROTO = 0;

#if SUBSTITUTE_DEFAULT_PARAMETERS
#define	SUBSTITUTE_DEFAULT_PARAMETERS_META_INDEX_PROTO			\
	count_ptr<const meta_index_expr>				\
	substitute_default_positional_parameters(			\
		const template_formals_manager&,			\
		const dynamic_param_expr_list&,				\
		const count_ptr<const meta_index_expr>&) const

virtual	SUBSTITUTE_DEFAULT_PARAMETERS_META_INDEX_PROTO = 0;
#endif

// additional virtual functions for dimensionality...
};	// end class meta_index_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_META_INDEX_EXPR_H__

