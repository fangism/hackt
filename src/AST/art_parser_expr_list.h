/**
	\file "art_parser_expr_list.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_expr_list.h,v 1.2.18.3 2005/04/11 17:59:13 fang Exp $
 */

#ifndef __ART_PARSER_EXPR_LIST_H__
#define __ART_PARSER_EXPR_LIST_H__

#include "art_parser_expr_base.h"
#include "art_parser_node_list.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	List of expressions.  
	Implemented in "art_parser_expr.cc".  
	All expression lists are comma-separated.  
	No need to sub-class expression lists into 
	template args and actuals.  

 */
typedef node_list<const expr>				expr_list_base;

/**
	General parser expression list.  
	Class connection_argument_list is derived from expr_list, 
	so we should keep methods virtual.
 */
class expr_list : public expr_list_base {
protected:
	typedef	expr_list_base			parent_type;
public:
	expr_list();

	explicit
	expr_list(const expr* e);

virtual	~expr_list();

virtual	ostream&
	what(ostream& o) const;

	using parent_type::leftmost;
	using parent_type::rightmost;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class expr_list

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_EXPR_LIST_H__

