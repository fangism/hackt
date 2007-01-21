/**
	\file "Object/expr/expr_visitor.h"
	Expression visitor interface.  
	$Id: expr_visitor.h,v 1.2 2007/01/21 05:58:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_EXPR_VISITOR_H__
#define	__HAC_OBJECT_EXPR_EXPR_VISITOR_H__

#include "Object/expr/expr_fwd.h"
#include "Object/ref/references_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================
#if 0
/** 
	Visitor for all meta expression types.  
 */
class meta_expr_visitor {
};
#endif

/**
	Can visit all expressions.
	NOTE: visitors must be non-modifying.
 */
class nonmeta_expr_visitor {
protected:
virtual	~nonmeta_expr_visitor() { }
public:
// these are catch-all overloads to abstract base classes
/**
	NOTE: this macro should only live temporarily, remember
	to undefine before end of this header.  
 */
#define	VISIT_PROTO(type)	void visit(const type&)
virtual	VISIT_PROTO(data_expr);
// virtual	VISIT_PROTO(bool_expr);
// virtual	VISIT_PROTO(int_expr);
// virtual	VISIT_PROTO(real_expr);
#if 0
virtual	VISIT_PROTO(enum_expr) = 0;
virtual	VISIT_PROTO(struct_expr) = 0;
#endif
virtual	VISIT_PROTO(nonmeta_index_expr_base);
virtual	VISIT_PROTO(nonmeta_range_expr_base);
virtual	VISIT_PROTO(param_expr);
virtual	VISIT_PROTO(const_param);
virtual	VISIT_PROTO(param_expr_list);
virtual	VISIT_PROTO(meta_index_expr);
// virtual	VISIT_PROTO(pbool_expr);
// virtual	VISIT_PROTO(pint_expr);
// virtual	VISIT_PROTO(preal_expr);
virtual	VISIT_PROTO(meta_range_expr);
virtual	VISIT_PROTO(meta_range_list);
virtual	VISIT_PROTO(meta_index_list);
#if 0
meta_instance_reference_base;
nonmeta_instance_reference_base;
data_nonmeta_instance_reference;
simple_meta_indexed_reference_base;
simple_nonmeta_instance_reference_base;
aggregate_meta_value_reference_base;	// exists?
aggregate_meta_instance_reference_base;	// exists?
channel_meta_instance_reference_base;
process_meta_instance_reference_base;
int_meta_instance_reference_base;
bool_meta_instance_reference_base;
enum_meta_instance_reference_base;
struct_meta_instance_reference_base;
meta_value_reference_base;
pbool_meta_value_reference_base;
pint_meta_value_reference_base;
preal_meta_value_reference_base;
#endif

// concrete classes
virtual	VISIT_PROTO(int_arith_expr) = 0;
virtual	VISIT_PROTO(int_negation_expr) = 0;
virtual	VISIT_PROTO(bool_negation_expr) = 0;
// virtual	VISIT_PROTO(real_negation_expr) = 0;
virtual	VISIT_PROTO(int_relational_expr) = 0;
// virtual	VISIT_PROTO(real_arith_expr) = 0;
// virtual	VISIT_PROTO(real_relational_expr) = 0;
virtual	VISIT_PROTO(bool_logical_expr) = 0;
virtual	VISIT_PROTO(int_range_expr) = 0;

virtual	VISIT_PROTO(nonmeta_index_list);	// defaulting list traversal
// virtual	VISIT_PROTO(nonmeta_range_list); // defaulting list traversal

virtual	VISIT_PROTO(const_param_expr_list);	// defaulting list traversal
virtual	VISIT_PROTO(dynamic_param_expr_list);	// defaulting list traversal
virtual	VISIT_PROTO(const_index) = 0;
virtual	VISIT_PROTO(pint_const) = 0;
virtual	VISIT_PROTO(pbool_const) = 0;
virtual	VISIT_PROTO(preal_const) = 0;
virtual	VISIT_PROTO(pint_unary_expr) = 0;
virtual	VISIT_PROTO(pbool_unary_expr) = 0;
virtual	VISIT_PROTO(preal_unary_expr) = 0;
virtual	VISIT_PROTO(pint_arith_expr) = 0;
virtual	VISIT_PROTO(pint_relational_expr) = 0;
virtual	VISIT_PROTO(preal_arith_expr) = 0;
virtual	VISIT_PROTO(preal_relational_expr) = 0;
virtual	VISIT_PROTO(pbool_logical_expr) = 0;
virtual	VISIT_PROTO(pint_range) = 0;
virtual	VISIT_PROTO(const_range) = 0;
virtual	VISIT_PROTO(const_range_list);		// defaulting list traversal
virtual	VISIT_PROTO(dynamic_meta_range_list);	// defaulting list traversal
virtual	VISIT_PROTO(const_index_list);		// defaulting list traversal
virtual	VISIT_PROTO(dynamic_meta_index_list);	// defaulting list traversal

virtual	VISIT_PROTO(pint_const_collection) = 0;
virtual	VISIT_PROTO(pbool_const_collection) = 0;
virtual	VISIT_PROTO(preal_const_collection) = 0;


#if 1
virtual	VISIT_PROTO(simple_channel_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_process_nonmeta_instance_reference) = 0;
//	simple_datatype_nonmeta_instance_reference;
#endif

virtual	VISIT_PROTO(simple_int_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_bool_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_enum_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_datastruct_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_pbool_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_pint_nonmeta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_preal_nonmeta_instance_reference) = 0;

virtual	VISIT_PROTO(simple_channel_meta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_process_meta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_enum_meta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_datastruct_meta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_bool_meta_instance_reference) = 0;
virtual	VISIT_PROTO(simple_int_meta_instance_reference) = 0;


virtual	VISIT_PROTO(aggregate_channel_meta_instance_reference) = 0;
virtual	VISIT_PROTO(aggregate_process_meta_instance_reference) = 0;
virtual	VISIT_PROTO(aggregate_enum_meta_instance_reference) = 0;
virtual	VISIT_PROTO(aggregate_datastruct_meta_instance_reference) = 0;
virtual	VISIT_PROTO(aggregate_bool_meta_instance_reference) = 0;
virtual	VISIT_PROTO(aggregate_int_meta_instance_reference) = 0;

virtual	VISIT_PROTO(process_member_meta_instance_reference) = 0;
virtual	VISIT_PROTO(channel_member_meta_instance_reference) = 0;
virtual	VISIT_PROTO(enum_member_meta_instance_reference) = 0;
virtual	VISIT_PROTO(datastruct_member_meta_instance_reference) = 0;
virtual	VISIT_PROTO(bool_member_meta_instance_reference) = 0;
virtual	VISIT_PROTO(int_member_meta_instance_reference) = 0;

virtual	VISIT_PROTO(simple_pint_meta_value_reference) = 0;
virtual	VISIT_PROTO(simple_pbool_meta_value_reference) = 0;
virtual	VISIT_PROTO(simple_preal_meta_value_reference) = 0;

virtual	VISIT_PROTO(aggregate_pint_meta_value_reference) = 0;
virtual	VISIT_PROTO(aggregate_pbool_meta_value_reference) = 0;
virtual	VISIT_PROTO(aggregate_preal_meta_value_reference) = 0;

#if 0
virtual	VISIT_PROTO(simple_int_nonmeta_value_reference) = 0;
virtual	VISIT_PROTO(simple_bool_nonmeta_value_reference) = 0;
simple_enum_nonmeta_value_reference;
simple_struct_nonmeta_value_reference;
#endif

#undef	VISIT_PROTO

};	// end class nonmeta_expr_visitor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	nonmeta_expr_visitor			expr_visitor;
typedef	expr_visitor				reference_visitor;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_EXPR_VISITOR_H__

