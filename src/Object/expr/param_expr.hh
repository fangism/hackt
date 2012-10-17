/**
	\file "Object/expr/param_expr.hh"
	Base class related to program expressions, symbolic and parameters.  
	NOTE: this file originally came from "Object/art_object_expr_base.h"
		for the sake of revision history tracking.  
	TODO: rename to meta_expr_base.hh
	$Id: param_expr.hh,v 1.18 2007/07/31 23:23:24 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PARAM_EXPR_H__
#define __HAC_OBJECT_EXPR_PARAM_EXPR_H__

#include "util/persistent.hh"
#include "util/memory/pointer_classes_fwd.hh"

//=============================================================================
namespace HAC {
namespace entity {
class param_expression_assignment;
class aggregate_meta_value_reference_base;
class const_param;
class const_range_list;
class unroll_context;
struct expr_dump_context;
class template_formals_manager;
class dynamic_param_expr_list;
class nonmeta_expr_visitor;
using util::persistent;
using std::ostream;
using util::memory::count_ptr;

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
public:
	typedef	count_ptr<param_expression_assignment>	assignment_ptr_type;
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
	assignment_ptr_type
	make_param_expression_assignment(const count_ptr<const this_type>&);

	static
	count_ptr<aggregate_meta_value_reference_base>
	make_aggregate_meta_value_reference(const count_ptr<const this_type>&);

virtual	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const param_expr>&) const = 0;

	// helper functor, defined in "Object/expr/param_expr_functor.h"
	struct unroller;

/**
	Performs copy-on-write expression substitution.  
	If the referenced value placeholder
	is a template formal member of the template_formal_manager, 
	then substitute using the expression corresponding to 
	its positional index, indexed into the dynamic_param_expr_list.
	Otherwise return itself, unmodified.  
 */
#define	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO				\
	count_ptr<const param_expr>					\
	substitute_default_positional_parameters(			\
		const template_formals_manager&,			\
		const dynamic_param_expr_list&,				\
		const count_ptr<const param_expr>&) const

virtual	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO = 0;

virtual	void
	accept(nonmeta_expr_visitor&) const = 0;

private:
#define	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO				\
	assignment_ptr_type						\
	make_param_expression_assignment_private(			\
		const count_ptr<const param_expr>&) const

virtual	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO = 0;

#define	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO			\
	count_ptr<aggregate_meta_value_reference_base>			\
	make_aggregate_meta_value_reference_private(			\
		const count_ptr<const param_expr>&) const

virtual	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO = 0;

};	// end class param_expr

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PARAM_EXPR_H__

