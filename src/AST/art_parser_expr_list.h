/**
	\file "art_parser_expr_list.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_expr_list.h,v 1.2 2005/02/25 07:23:55 fang Exp $
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
typedef node_list<const expr,comma>			expr_list_base;

/**
	General parser expression list.  
	Class connection_argument_list is derived from expr_list, 
	so we should keep methods virtual.
 */
class expr_list : public expr_list_base {
protected:
	typedef	expr_list_base			parent;
public:
	expr_list();

	explicit
	expr_list(const expr* e);

virtual	~expr_list();

virtual	ostream&
	what(ostream& o) const;

	using parent::leftmost;
	using parent::rightmost;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class expr_list

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_EXPR_LIST_H__

