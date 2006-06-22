/**
	\file "Object/expr/nonmeta_index_list.h"
	Class definitions for nonmeta index lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: nonmeta_index_list.h,v 1.5.40.3 2006/06/22 04:04:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__
#define	__HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__

#include <vector>
#include "util/persistent.h"
#include "Object/common/multikey_index.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class nonmeta_index_expr_base;
struct expr_dump_context;
class unroll_context;
class const_index_list;
using std::vector;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::good_bool;

//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
	NOTE: value_type is const pointer. 
 */
class nonmeta_index_list : public persistent, 
		public vector<count_ptr<const nonmeta_index_expr_base> > {
	typedef	nonmeta_index_list		this_type;
	typedef	vector<count_ptr<const nonmeta_index_expr_base> >
						list_type;
public:
	typedef	list_type::value_type		value_type;
	typedef	list_type::iterator		iterator;
	typedef	list_type::const_iterator	const_iterator;
public:
	nonmeta_index_list();

	explicit
	nonmeta_index_list(const size_t);

	~nonmeta_index_list();

	size_t
	dimensions_collapsed(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	count_ptr<this_type>
	unroll_resolve_copy(const unroll_context&) const;

	good_bool
	make_const_index_list(multikey_index_type&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__

