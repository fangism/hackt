/**
	\file "AST/art_parser_range.h"
	Expression-related parser classes for ART.
	$Id: art_parser_range.h,v 1.5.4.2 2005/06/10 04:16:34 fang Exp $
 */

#ifndef __AST_ART_PARSER_RANGE_H__
#define __AST_ART_PARSER_RANGE_H__

#include "AST/art_parser_expr_base.h"

namespace ART {
namespace entity {
	class meta_index_expr;
	class nonmeta_index_expr_base;
}	// end namespace entity
namespace parser {
//=============================================================================
/**
	Can describe either a range of integers (inclusive) or a 
	single integer.  Often found in sparse or multidimensional 
	array declarations and expressions.  
	Ranges are considered expressions, and consist of expressions.  
	Class is final, no subclasses.  
 */
class range {
public:
	typedef	count_ptr<entity::meta_index_expr>	meta_return_type;
	typedef	count_ptr<entity::nonmeta_index_expr_base>
							nonmeta_return_type;
protected:
	const excl_ptr<const expr>	lower;	///< inclusive lower bound
	const excl_ptr<const expr>	upper;	///< inclusive upper bound
public:
/// simple constructor for when range is just one integer expression
	explicit
	range(const expr* l);
/**
	Full range constructor with min and max.  
 */
	range(const expr* l, const expr* u);

	~range();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	range::meta_return_type
	check_meta_index(context& c) const;

	range::nonmeta_return_type
	check_nonmeta_index(context& c) const;
};	// end class range

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_RANGE_H__

