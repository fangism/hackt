/**
	\file "Object/expr/expr_fwd.h"
	Forward declarations of all expression-related classes.  
	$Id: expr_fwd.h,v 1.3 2007/01/21 05:58:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_EXPR_FWD_H__
#define	__HAC_OBJECT_EXPR_EXPR_FWD_H__

#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {

	// from "Object/expr/data_expr[_base].h"
	class data_expr;
	class bool_expr;
	class int_expr;
	class real_expr;        // not officially supported
	class enum_expr;
	class struct_expr;
	class int_range_expr;
	class nonmeta_index_expr_base;
	class nonmeta_range_expr_base;
	class nonmeta_index_list;
	class nonmeta_range_list;

	class int_arith_expr;
	class int_negation_expr;
	class int_relational_expr;
	class bool_negation_expr;
	class bool_logical_expr;
//	class real_negation_expr;
	class real_arith_expr;
	class real_relational_expr;

	class param_expr;
	class const_param;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class meta_index_expr;
	class const_index;
	class pbool_expr;
	class pint_expr;
	class preal_expr;
	class pint_const;
	class pbool_const;
	class preal_const;
//	class param_unary_expr;
	class pint_unary_expr;
	class pbool_unary_expr;
	class preal_unary_expr;
//	class param_binary_expr;
	class pint_arith_expr;
	class pint_relational_expr;
	class preal_arith_expr;
	class preal_relational_expr;
	class pbool_logical_expr;
	class meta_range_expr;
	class pint_range;
	class const_range;
	class meta_range_list;
	class const_range_list;
	class dynamic_meta_range_list;
//      class unconditional_range_list;
//      class conditional_range_list;
//      class loop_range_list;
	class meta_index_list;
	class const_index_list;
	class dynamic_meta_index_list;

	template <class>
	class const_collection;
	typedef const_collection<pint_tag>
		pint_const_collection;
	typedef const_collection<pbool_tag>
		pbool_const_collection;
	typedef const_collection<preal_tag>
		preal_const_collection;


}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_EXPR_FWD_H__

