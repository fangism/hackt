/**
	\file "AST/range.hh"
	Expression-related parser classes for HAC.
	$Id: range.hh,v 1.7 2011/02/08 02:06:46 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_range.h,v 1.6.42.1 2005/12/11 00:45:10 fang Exp
 */

#ifndef __HAC_AST_RANGE_H__
#define __HAC_AST_RANGE_H__

#include <vector>
#include "AST/expr_base.hh"
#include "util/what_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
	class meta_index_expr;
	class nonmeta_index_expr_base;
}	// end namespace entity
namespace parser {
class token_int;

//=============================================================================
/**
	Can describe either a range of integers (inclusive) or a 
	single integer.  Often found in sparse or multidimensional 
	array declarations and expressions.  
	Ranges are considered expressions, and consist of expressions.  
	Class is final, no subclasses.  
 */
class range {
	typedef	range					this_type;
public:
	typedef	count_ptr<entity::meta_index_expr>	meta_return_type;
	typedef	count_ptr<entity::nonmeta_index_expr_base>
							nonmeta_return_type;
	typedef	count_ptr<const token_int>		const_int_type;
	typedef	std::vector<const_int_type>		expanded_range_type;
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

	ostream&
	dump(ostream&) const;

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

	int
	expand_const_indices(const count_ptr<const this_type>&, 
		expanded_range_type&) const;

};	// end class range

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

namespace util {
SPECIALIZE_UTIL_WHAT_DECLARATION(HAC::parser::range)
}

#endif	// __HAC_AST_RANGE_H__

