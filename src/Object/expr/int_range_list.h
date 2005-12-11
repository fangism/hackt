/**
	\file "Object/expr/int_range_list.h"
	Class definitions for nonmeta range lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_range_list.h,v 1.3.12.1 2005/12/11 00:45:27 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_RANGE_LIST_H__
#define	__OBJECT_EXPR_INT_RANGE_LIST_H__

#include "util/persistent.h"
#include <vector>
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class int_range_expr;
struct expr_dump_context;
using std::ostream;
using std::vector;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class int_range_list : public persistent, 
		public vector<count_ptr<const int_range_expr> > {
	typedef	int_range_list			this_type;
	typedef	vector<count_ptr<const int_range_expr> >	list_type;
public:
	int_range_list();

	explicit
	int_range_list(const size_t);

	~int_range_list();

	size_t
	dimensions_collapsed(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_EXPR_INT_RANGE_LIST_H__

