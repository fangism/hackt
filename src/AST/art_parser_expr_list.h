/**
	\file "AST/art_parser_expr_list.h"
	Base set of classes parser expression lists.  
	$Id: art_parser_expr_list.h,v 1.6 2005/05/13 21:24:27 fang Exp $
 */

#ifndef __AST_ART_PARSER_EXPR_LIST_H__
#define __AST_ART_PARSER_EXPR_LIST_H__

#include "AST/art_parser_expr_base.h"
#include "AST/art_parser_node_list.h"
#include "util/STL/vector_fwd.h"

namespace ART {
namespace parser {
using std::vector;
//=============================================================================
/**
	List of expressions.  
	Implemented in "art_parser_expr.cc".  
	All expression lists are comma-separated.  
	No need to sub-class expression lists into 
	template args and actuals.  

 */
typedef node_list<const expr>				expr_list_base;

//=============================================================================
/**
	General parser expression list.  
	Class connection_argument_list is derived from expr_list, 
	so we should keep methods virtual.
 */
class expr_list : public expr_list_base {
protected:
	typedef	expr_list_base			parent_type;
public:
	typedef	parent_type::const_iterator	const_iterator;
        typedef	DEFAULT_VECTOR(expr::generic_return_type)
							checked_generic_type;
        typedef	DEFAULT_VECTOR(expr::return_type)	checked_exprs_type;
        typedef	DEFAULT_VECTOR(inst_ref_return_type)	checked_refs_type;
public:
	expr_list();

	explicit
	expr_list(const expr* e);

virtual	~expr_list();

#if 0
virtual	ostream&
	what(ostream& o) const;
#endif

	using parent_type::leftmost;
	using parent_type::rightmost;

#if 0
private:
virtual	never_ptr<const object>
	check_build(context& c) const;
public:
#endif

	void
	postorder_check_generic(checked_generic_type&, context&) const;

	void
	postorder_check_exprs(checked_exprs_type&, context&) const;

	static
	void
	select_checked_exprs(const checked_generic_type&, checked_exprs_type&);

	static
	void
	select_checked_refs(const checked_generic_type&, checked_refs_type&);
};	// end class expr_list

//=============================================================================
/**
	Contains a strict template argument list and a relaxed template
	argument list.  
	Intended for use with concrete_type_ref.
 */
class template_argument_list_pair {
protected:
	const excl_ptr<const expr_list>		strict_args;
	const excl_ptr<const expr_list>		relaxed_args;
public:
	template_argument_list_pair(const expr_list*, const expr_list*);
	~template_argument_list_pair();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;

};	// end class template_argument_list_pair

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_EXPR_LIST_H__

