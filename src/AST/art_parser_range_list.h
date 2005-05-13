/**
	\file "AST/art_parser_range.h"
	Expression-related parser classes for ART.
	$Id: art_parser_range_list.h,v 1.4.2.3 2005/05/13 20:04:13 fang Exp $
 */

#ifndef __AST_ART_PARSER_RANGE_LIST_H__
#define __AST_ART_PARSER_RANGE_LIST_H__

#include "AST/art_parser_range.h"
#include "AST/art_parser_node_list.h"
#include "util/boolean_types.h"
#include "util/STL/vector_fwd.h"

namespace ART {
namespace entity {
	class index_list;
	class range_expr_list;
}	// end namespace entity
namespace parser {
using util::good_bool;

//=============================================================================
/// base class for range_list
typedef node_list<const range>		range_list_base;

/**
	All sparse range lists are C-style x[N][M], 
	now limited to 4-dimensions.  
	range_list currently has two uses:
	1) index_expr: to index sub-slices of arrays
	2) instance_array: to declare dense arrays
 */
class range_list : public range_list_base {
public:
	typedef	range_list_return_type			checked_indices_type;
	typedef	count_ptr<entity::range_expr_list>	checked_ranges_type;
protected:
	typedef	range_list_base				parent_type;
	// no additional members
public:
	explicit
	range_list(const range* r);

	~range_list();

#if HAVE_EXPR_CHECK_BUILD
	never_ptr<const object>
	check_build(context& c) const;
#endif

	range_list::checked_indices_type
	check_indices(context& c) const;

	range_list::checked_ranges_type
	check_ranges(context& c) const;

private:
	typedef	DEFAULT_VECTOR(range::return_type)	check_type;

	/**
		Intermediate check.  
	 */
	good_bool
	postorder_check(check_type&, context&) const;
};	// end class range_list

//-----------------------------------------------------------------------------
/// base class for dense_range_list
typedef node_list<const expr>		dense_range_list_base;

/**
	All dense range lists are C-style x[N][M], 
	now limited to 4-dimensions.  
	dense_range_lists are used in template and port formal declarations.  
 */
class dense_range_list : public dense_range_list_base {
public:
	typedef	count_ptr<entity::range_expr_list>	return_type;
protected:
	typedef	dense_range_list_base			parent_type;
	// no additional members
public:
	explicit
	dense_range_list(const expr* r);

	~dense_range_list();

#if 0
	never_ptr<const object>
	check_build(context& c) const;
#endif

	dense_range_list::return_type
	check_formal_dense_ranges(context& c) const;

private:
	typedef	DEFAULT_VECTOR(expr::return_type)	check_type;
	
	/**
		Intermediate check.  
	 */
	good_bool
	postorder_check(check_type&, context&) const;
};	// end class range_list

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_RANGE_LIST_H__

