/**
	\file "Object/expr/nonmeta_index_list.hh"
	Class definitions for nonmeta index lists.
	NOTE: this file was spanwed off of "Object/art_object_data_expr.h"
		for revision history tracking purposes.  
	$Id: nonmeta_index_list.hh,v 1.8 2007/01/21 05:58:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__
#define	__HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__

#include <vector>
#include "util/persistent.hh"
#include "Object/common/multikey_index.hh"
#include "Object/ref/meta_index_list_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
class nonmeta_index_expr_base;
struct expr_dump_context;
class unroll_context;
class nonmeta_context_base;
class const_index_list;
class dynamic_meta_index_list;
class nonmeta_expr_visitor;
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

	count_ptr<dynamic_meta_index_list>
	make_meta_index_list(void) const;

	count_ptr<const_index_list>
	nonmeta_resolve_copy(const nonmeta_context_base&) const;

	PERSISTENT_METHODS_DECLARATIONS

	void
	accept(nonmeta_expr_visitor&) const;

};	// end class nonmeta_index_list

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_INDEX_LIST_H__

