/**
	\file "Object/expr/int_range_list.h"
	Class definitions for nonmeta range lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_range_list.h,v 1.1.2.1 2005/07/05 01:16:29 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_RANGE_LIST_H__
#define	__OBJECT_EXPR_INT_RANGE_LIST_H__

#include "util/persistent.h"
#include <list>
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class int_range_expr;
using std::ostream;
using std::list;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class int_range_list : public persistent, 
		public list<count_ptr<const int_range_expr> > {
	typedef	int_range_list			this_type;
	typedef	list<count_ptr<const int_range_expr> >	list_type;
public:
	int_range_list();
	~int_range_list();

	size_t
	dimensions_collapsed(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_INT_RANGE_LIST_H__

