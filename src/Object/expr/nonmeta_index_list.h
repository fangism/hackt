/**
	\file "Object/expr/nonmeta_index_list.h"
	Class definitions for nonmeta index lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: nonmeta_index_list.h,v 1.1.2.1 2005/07/05 01:16:30 fang Exp $
 */

#ifndef	__OBJECT_EXPR_NONMETA_INDEX_LIST_H__
#define	__OBJECT_EXPR_NONMETA_INDEX_LIST_H__

#include <list>
#include "util/persistent.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class nonmeta_index_expr_base;
using std::list;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::count_ptr;
//=============================================================================
/**
	List of indices, which may contain non-meta expressions, 
	that will be run-time evaluated.  
 */
class nonmeta_index_list : public persistent, 
		public list<count_ptr<const nonmeta_index_expr_base> > {
	typedef	nonmeta_index_list		this_type;
	typedef	list<count_ptr<const nonmeta_index_expr_base> >	list_type;
public:
	nonmeta_index_list();
	~nonmeta_index_list();

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

#endif	// __OBJECT_EXPR_NONMETA_INDEX_LIST_H__

