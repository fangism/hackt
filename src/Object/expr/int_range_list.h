/**
	\file "Object/expr/int_range_list.h"
	Class definitions for nonmeta range lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: int_range_list.h,v 1.2.22.1 2005/10/13 01:27:03 fang Exp $
 */

#ifndef	__OBJECT_EXPR_INT_RANGE_LIST_H__
#define	__OBJECT_EXPR_INT_RANGE_LIST_H__

#include "util/persistent.h"
#include <vector>
#include "util/memory/count_ptr.h"
#include "Object/devel_switches.h"

namespace ART {
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

#if USE_EXPR_DUMP_CONTEXT
	ostream&
	dump(ostream& o, const expr_dump_context&) const;
#else
	ostream&
	dump(ostream&) const;
#endif

	PERSISTENT_METHODS_DECLARATIONS
};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_EXPR_INT_RANGE_LIST_H__

