/**
	\file "AST/range.h"
	Expression-related parser classes for HAC.
	$Id: range.h,v 1.5.10.1 2007/10/06 22:10:29 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_range.h,v 1.6.42.1 2005/12/11 00:45:10 fang Exp
 */

#ifndef __HAC_AST_RANGE_H__
#define __HAC_AST_RANGE_H__

#include "AST/expr_base.h"
#include "util/what_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
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
	// using ref-count ptr for copy-convenience
	const count_ptr<const expr>	lower;	///< inclusive lower bound
	const count_ptr<const expr>	upper;	///< inclusive upper bound
public:
	struct copy_allocator;
public:
/// simple constructor for when range is just one integer expression
	explicit
	range(const expr* l);

	explicit
	range(const count_ptr<const expr>& l);
/**
	Full range constructor with min and max.  
 */
	range(const expr* l, const expr* u);

	range(const count_ptr<const expr>& l,
		const count_ptr<const expr>& u);

	~range();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	range::meta_return_type
	check_meta_index(const context& c) const;

	range::nonmeta_return_type
	check_nonmeta_index(const context& c) const;

	static
	count_ptr<const range>
	make_explicit_range(const count_ptr<const range>&);

};	// end class range

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

namespace util {
SPECIALIZE_UTIL_WHAT_DECLARATION(HAC::parser::range)
}

#endif	// __HAC_AST_RANGE_H__

