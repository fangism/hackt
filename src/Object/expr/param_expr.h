/**
	\file "Object/expr/param_expr.h"
	Base class related to program expressions, symbolic and parameters.  
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_base.h
	$Id: param_expr.h,v 1.10.22.1 2006/05/17 02:22:49 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_H__

#include "util/persistent.h"
#include "util/memory/pointer_classes_fwd.h"
#include "util/memory/excl_ptr.h"

//=============================================================================
namespace HAC {
namespace entity {
class param_expression_assignment;
class aggregate_meta_value_reference_base;
class const_param;
class const_range_list;
class unroll_context;
struct expr_dump_context;
using util::persistent;
using std::ostream;
using util::memory::count_ptr;
using util::memory::excl_ptr;

//=============================================================================
/**
	Abstract base class for symbolic expressions to be written 
	to an object file.  
	Expression graphs must be directed-acyclic.  
	Deriving from object, so that it may be cached in 
	scopespaces' used_id_map.  
	Should statically sub-type into pints and pbools and pranges...
 */
class param_expr : virtual public persistent {
	typedef	param_expr			this_type;
protected:
	param_expr() : persistent() { }
public:
virtual	~param_expr() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	ostream&
	dump(ostream& o, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

/** is initialized if is resolved to constant or some other formal */
virtual bool
	may_be_initialized(void) const = 0;

virtual bool
	must_be_initialized(void) const = 0;

virtual bool
	may_be_equivalent_generic(const this_type&) const = 0;

virtual bool
	must_be_equivalent_generic(const this_type&) const = 0;

/** can be resolved to static constant value */
virtual bool
	is_static_constant(void) const = 0;

virtual bool
	is_relaxed_formal_dependent(void) const = 0;

virtual	count_ptr<const const_param>
	static_constant_param(void) const = 0;

	static
	excl_ptr<param_expression_assignment>
	make_param_expression_assignment(const count_ptr<const this_type>&);

	static
	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference(const count_ptr<const this_type>&);

virtual	count_ptr<const_param>
	unroll_resolve_rvalues(const unroll_context&) const = 0;

	// helper functor, defined in "Object/expr/param_expr_functor.h"
	struct unroller;

private:
virtual	excl_ptr<param_expression_assignment>
	make_param_expression_assignment_private(
		const count_ptr<const this_type>&) const = 0;

virtual	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference_private(
		const count_ptr<const this_type>&) const = 0;
};	// end class param_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_H__

