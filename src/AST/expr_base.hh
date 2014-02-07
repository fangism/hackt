/**
	\file "AST/expr_base.hh"
	Base set of classes for the HAC parser.  
	$Id: expr_base.hh,v 1.9 2011/02/08 02:06:45 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_expr_base.h,v 1.7.32.1 2005/12/11 00:45:06 fang Exp
 */

#ifndef __HAC_AST_EXPR_BASE_H__
#define __HAC_AST_EXPR_BASE_H__

#include <vector>
#include "AST/common.hh"
#include "util/STL/pair_fwd.hh"
#include "Object/ref/references_fwd.hh"

namespace HAC {
//=============================================================================
// forward declaration of outside namespace and classes
namespace entity {
	// defined in "common/object_base.hh"
	class object;
	class param_expr;
	class meta_index_list;
	class meta_instance_reference_base;
	class data_expr;		// nonmeta expressions
	class nonmeta_instance_reference_base;
	class data_nonmeta_instance_reference;
	class nonmeta_index_list;
	class int_range_list;		// a.k.a. nonmeta_range_list
	// from "Object/ref/meta_reference_union.hh"
	class meta_reference_union;
	class const_meta_reference_union;
	class bool_literal;
	class proc_literal;
namespace PRS {
	class prs_expr;
	class literal;
}
namespace RTE {
	class rte_expr;
	class literal;
}
}	// end namespace entity

using std::ostream;
using std::pair;
using entity::object;
using entity::simple_bool_meta_instance_reference;
using util::memory::excl_ptr;
using util::memory::count_ptr;

namespace parser {
//=============================================================================
/**
	Bah! breaks circular dependence of nested typedefs.  
 */
typedef	count_ptr<entity::meta_index_list>	range_list_meta_return_type;
typedef	count_ptr<entity::nonmeta_index_list>	range_list_nonmeta_return_type;

typedef	entity::meta_reference_union		inst_ref_meta_return_type;

typedef	count_ptr<entity::nonmeta_instance_reference_base>
						inst_ref_nonmeta_return_type;

// why not simple_datatype_nonmeta_value_reference?
typedef	count_ptr<entity::data_nonmeta_instance_reference>
						data_ref_nonmeta_return_type;

typedef	count_ptr<entity::PRS::prs_expr>	prs_expr_return_type;
typedef	count_ptr<entity::RTE::rte_expr>	rte_expr_return_type;

typedef	count_ptr<entity::PRS::literal>		prs_literal_ptr_type;
typedef	count_ptr<entity::RTE::literal>		rte_lvalue_ptr_type;

typedef	count_ptr<entity::param_expr>		meta_expr_return_type;

typedef	count_ptr<entity::data_expr>		nonmeta_expr_return_type;

//=============================================================================
/**
	Abstract base class for general expressions.
	Expressions may be terminal or nonterminal.
	Defined in "AST/expr.hh".
 */
class expr {
public:
	typedef	meta_expr_return_type		meta_return_type;
	typedef	nonmeta_expr_return_type	nonmeta_return_type;
public:
	expr() { }
virtual ~expr() { }

virtual ostream&
	what(ostream& o) const = 0;

virtual ostream&
	dump(ostream&) const = 0;

virtual line_position
	leftmost(void) const = 0;

virtual line_position
	rightmost(void) const = 0;

/**
	Prototype for expression check method.  
 */
#define	CHECK_META_EXPR_PROTO						\
	expr::meta_return_type						\
	check_meta_expr(const context& c) const

virtual CHECK_META_EXPR_PROTO = 0;

#define	CHECK_NONMETA_EXPR_PROTO					\
	nonmeta_expr_return_type					\
	check_nonmeta_expr(const context& c) const

virtual	CHECK_NONMETA_EXPR_PROTO;

public:
	/**
		KLUDGE:
		Pair of pointers.
		If both are non-NULL, they point to the same object, 
		just that each is statically cast to a different subtype.  
	 */
	typedef	pair<expr::meta_return_type, inst_ref_meta_return_type>
						generic_meta_return_type;

#define	CHECK_GENERIC_PROTO						\
	generic_meta_return_type					\
	check_meta_generic(const context& c) const
/**
	Needed by alias_list's check routine.
	The caller of this will need to #include <utility>.
 */
virtual	CHECK_GENERIC_PROTO;

#define	CHECK_PRS_EXPR_PROTO						\
	prs_expr_return_type						\
	check_prs_expr(context& c) const

virtual	CHECK_PRS_EXPR_PROTO;

#define	CHECK_RTE_EXPR_PROTO						\
	rte_expr_return_type						\
	check_rte_expr(context& c) const

virtual	CHECK_RTE_EXPR_PROTO;

};	// end class expr

//=============================================================================
/**
	Expressions that may refer to instance or value references.
 */
class inst_ref_expr : public expr {
protected:
	typedef expr                                    parent_type;
public:
	/**
		Type of the result returned by parse-checker.
	 */
	typedef inst_ref_meta_return_type		meta_return_type;
	typedef inst_ref_nonmeta_return_type		nonmeta_return_type;
	typedef data_ref_nonmeta_return_type	nonmeta_data_return_type;
	typedef	entity::bool_literal		simple_meta_return_type;
	typedef	std::vector<simple_meta_return_type>
					checked_bool_group_type;
	typedef	std::vector<entity::proc_literal>
					checked_proc_group_type;
	typedef	std::vector<count_ptr<const inst_ref_expr> >
					reference_array_type;

	inst_ref_expr() : parent_type() { }
virtual ~inst_ref_expr() { }

#define CHECK_META_REFERENCE_PROTO					\
	inst_ref_expr::meta_return_type					\
	check_meta_reference(const context&) const

virtual CHECK_META_REFERENCE_PROTO = 0;

#define CHECK_NONMETA_REFERENCE_PROTO					\
	inst_ref_expr::nonmeta_return_type				\
	check_nonmeta_reference(const context&) const

virtual CHECK_NONMETA_REFERENCE_PROTO = 0;

	// NOTE: this is non-virtual
	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;

	nonmeta_data_return_type
	check_nonmeta_data_reference(const context&) const;

	// NOTE: this is non-virtual
	CHECK_GENERIC_PROTO;

	// overridden only by PRS::literal
virtual	prs_literal_ptr_type
	check_prs_literal(const context&) const;

	// overridden only by RTE::literal
virtual	rte_lvalue_ptr_type
	check_rte_lvalue(const context&) const;

	// overridden only by reference_group_construction
virtual	bool
	check_grouped_literals(checked_bool_group_type&, const context&) const;
virtual	bool
	check_grouped_literals(checked_proc_group_type&, const context&) const;

	CHECK_PRS_EXPR_PROTO;
	CHECK_RTE_EXPR_PROTO;

#define	EXPAND_CONST_REFERENCE_PROTO					\
	int								\
	expand_const_reference(const count_ptr<const inst_ref_expr>&,	\
		reference_array_type&) const

virtual	EXPAND_CONST_REFERENCE_PROTO = 0;

};      // end class inst_ref_expr

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_EXPR_BASE_H__

