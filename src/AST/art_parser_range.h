/**
	\file "art_parser_range.h"
	Expression-related parser classes for ART.
	$Id: art_parser_range.h,v 1.2.4.2 2005/05/04 17:23:19 fang Exp $
 */

#ifndef __ART_PARSER_RANGE_H__
#define __ART_PARSER_RANGE_H__

#include "art_parser_expr_base.h"

namespace ART {
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
protected:
	const excl_ptr<const expr>	lower;	///< inclusive lower bound
	const excl_ptr<const string_punctuation_type>	op;	///< range operator token ".."
	const excl_ptr<const expr>	upper;	///< inclusive upper bound
public:
/// simple constructor for when range is just one integer expression
	explicit
	range(const expr* l);
/**
	Full range constructor with min and max.  
 */
	range(const expr* l, const string_punctuation_type* o, const expr* u);

	~range();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class range

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_RANGE_H__

