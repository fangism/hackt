/**
	\file "AST/art_parser_expr_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_expr_base.h,v 1.5.2.1 2005/06/08 19:13:15 fang Exp $
 */

#ifndef __AST_ART_PARSER_EXPR_BASE_H__
#define __AST_ART_PARSER_EXPR_BASE_H__

#include "AST/art_parser_base.h"
#include "util/STL/pair_fwd.h"

namespace ART {
//=============================================================================
// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object_base.h"
	class object;
	class param_expr;
	class meta_index_list;
	class meta_instance_reference_base;
namespace PRS {
	class prs_expr;
	class literal;
}
}

using std::ostream;
using std::pair;
using entity::object;
using util::memory::excl_ptr;
using util::memory::count_ptr;

namespace parser {
//=============================================================================
/**
	Bah! breaks circular dependence of nested typedefs.  
 */
typedef	count_ptr<entity::meta_index_list>	range_list_return_type;

typedef	count_ptr<entity::meta_instance_reference_base>	inst_ref_return_type;

typedef	count_ptr<entity::PRS::prs_expr>	prs_expr_return_type;

typedef	count_ptr<entity::PRS::literal>		prs_literal_ptr_type;

//=============================================================================
/**
	Abstract base class for general expressions.
	Expressions may be terminal or nonterminal.
	Defined in "art_parser_expr.h".
 */
class expr {
public:
	typedef	count_ptr<entity::param_expr>	return_type;
public:
	expr() { }
virtual ~expr() { }

virtual ostream&
	what(ostream& o) const = 0;

virtual line_position
	leftmost(void) const = 0;

virtual line_position
	rightmost(void) const = 0;

/**
	Prototype for expression check method.  
 */
#define	CHECK_EXPR_PROTO						\
	expr::return_type						\
	check_expr(context& c) const

virtual CHECK_EXPR_PROTO = 0;

public:
	/**
		KLUDGE:
		Pair of pointers.
		If both are non-NULL, they point to the same object, 
		just that each is statically cast to a different subtype.  
	 */
	typedef	pair<expr::return_type, inst_ref_return_type>
						generic_return_type;

#define	CHECK_GENERIC_PROTO						\
	generic_return_type						\
	check_generic(context& c) const
/**
	Needed by alias_list's check routine.
	The caller of this will need to #include <utility>.
 */
virtual	CHECK_GENERIC_PROTO;

#define	CHECK_PRS_EXPR_PROTO						\
	prs_expr_return_type						\
	check_prs_expr(context& c) const

virtual	CHECK_PRS_EXPR_PROTO;

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
	typedef inst_ref_return_type                    return_type;

	inst_ref_expr() : parent_type() { }
virtual ~inst_ref_expr() { }

#define CHECK_REFERENCE_PROTO                                           \
	inst_ref_expr::return_type                                      \
	check_reference(context&) const

virtual CHECK_REFERENCE_PROTO = 0;

	// NOTE: this is non-virtual
	CHECK_EXPR_PROTO;

	// NOTE: this is non-virtual
	CHECK_GENERIC_PROTO;

	prs_literal_ptr_type
	check_prs_literal(context&) const;

	CHECK_PRS_EXPR_PROTO;
	
};      // end class inst_ref_expr

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_EXPR_BASE_H__

