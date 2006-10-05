/**
	\file "Object/expr/dynamic_meta_index_list.h"
	Dynamic meta parameter index list class.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: dynamic_meta_index_list.h,v 1.7.32.3 2006/10/05 01:57:02 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_DYNAMIC_META_INDEX_LIST_H__
#define __HAC_OBJECT_EXPR_DYNAMIC_META_INDEX_LIST_H__

#include <vector>
#include "Object/expr/meta_index_list.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class meta_index_expr;
using std::vector;
using std::string;
using util::memory::count_ptr;

//=============================================================================
/**
	Elements of this index list are no necessarily static constants.  
 */
class dynamic_meta_index_list : public meta_index_list, 
		private vector<count_ptr<const meta_index_expr> > {
	typedef	dynamic_meta_index_list			this_type;
protected:
	typedef	vector<count_ptr<const meta_index_expr> >	parent_type;
public:
	typedef	parent_type::value_type			value_type;
	typedef parent_type::iterator			iterator;
	typedef parent_type::const_iterator		const_iterator;
	typedef parent_type::reverse_iterator		reverse_iterator;
	typedef parent_type::const_reverse_iterator	const_reverse_iterator;
public:
	dynamic_meta_index_list();

	explicit
	dynamic_meta_index_list(const size_t);

	~dynamic_meta_index_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	using parent_type::begin;
	using parent_type::end;
	using parent_type::rbegin;
	using parent_type::rend;

	void
	push_back(const count_ptr<const meta_index_expr>&);

/** NOT THE SAME **/
	size_t
	size(void) const;

	size_t
	dimensions_collapsed(void) const;

#if ENABLE_STATIC_ANALYSIS
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;
#endif

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

#if !USE_INSTANCE_PLACEHOLDERS
	const_index_list
	resolve_index_list(void) const;
#endif

#if 0
	bool
	resolve_multikey(excl_ptr<multikey_index_type>& k) const;
#endif

	const_index_list
	unroll_resolve_indices(const unroll_context&) const;

	bool
	must_be_equivalent_indices(const meta_index_list& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class dynamic_meta_index_list

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_DYNAMIC_META_INDEX_LIST_H__

