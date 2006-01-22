/**
	\file "AST/range.h"
	Expression-related parser classes for HAC.
	$Id: range_list.h,v 1.3 2006/01/22 06:52:54 fang Exp $
	This file used to be the following before it was renamed:
	$Id: range_list.h,v 1.3 2006/01/22 06:52:54 fang Exp $
 */

#ifndef __HAC_AST_RANGE_LIST_H__
#define __HAC_AST_RANGE_LIST_H__

#include "AST/range.h"
#include "AST/node_list.h"
#include "util/boolean_types.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
	class meta_index_list;
	class meta_range_list;
	class nonmeta_index_list;
	class int_range_list;		// a.k.a. nonmeta_index_list
}	// end namespace entity
namespace parser {
using util::good_bool;
using std::default_vector;

//=============================================================================
/// base class for range_list
typedef node_list<const range>		range_list_base;

/**
	All sparse range lists are C-style x[N][M], 
	now limited to 4-dimensions.  
	range_list currently has two uses:
	1) meta_index_expr: to index sub-slices of arrays
	2) instance_array: to declare dense arrays
 */
class range_list : public range_list_base {
public:
	typedef	range_list_meta_return_type	checked_meta_indices_type;
	typedef	count_ptr<entity::meta_range_list>
						checked_meta_ranges_type;
	typedef	range_list_nonmeta_return_type	checked_nonmeta_indices_type;
	typedef	count_ptr<entity::int_range_list>
						checked_nonmeta_ranges_type;
protected:
	typedef	range_list_base				parent_type;
	// no additional members
public:
	explicit
	range_list(const range* r);

	~range_list();

	checked_meta_indices_type
	check_meta_indices(const context& c) const;

	checked_meta_ranges_type
	check_meta_ranges(const context& c) const;

	checked_nonmeta_indices_type
	check_nonmeta_indices(const context& c) const;

	checked_nonmeta_ranges_type
	check_nonmeta_ranges(const context& c) const;

private:
	typedef	default_vector<range::meta_return_type>::type
							meta_check_type;
	typedef	default_vector<range::nonmeta_return_type>::type
							nonmeta_check_type;

	/**
		Intermediate check.  
	 */
	good_bool
	postorder_check_meta(meta_check_type&, const context&) const;

	/**
		Intermediate check.  
	 */
	good_bool
	postorder_check_nonmeta(nonmeta_check_type&, const context&) const;
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
	typedef	count_ptr<entity::meta_range_list>	meta_return_type;
	typedef	count_ptr<entity::int_range_list>	nonmeta_return_type;
protected:
	typedef	dense_range_list_base			parent_type;
	// no additional members
public:
	explicit
	dense_range_list(const expr* r);

	~dense_range_list();

	meta_return_type
	check_formal_dense_ranges(const context& c) const;

private:
	typedef	default_vector<expr::meta_return_type>::type
						meta_check_type;
	typedef	default_vector<expr::nonmeta_return_type>::type
						nonmeta_check_type;
	
	/**
		Intermediate check.  
	 */
	good_bool
	postorder_check_meta(meta_check_type&, const context&) const;
};	// end class range_list

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_RANGE_LIST_H__

