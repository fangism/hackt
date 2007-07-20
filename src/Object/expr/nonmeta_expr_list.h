/**
	\file "Object/expr/nonmeta_expr_list.h"
	$Id: nonmeta_expr_list.h,v 1.1.2.1 2007/07/20 21:07:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_EXPR_LIST_H__
#define	__HAC_OBJECT_EXPR_NONMETA_EXPR_LIST_H__

#include <vector>
#include "util/persistent.h"
#include "util/memory/count_ptr.h"
// #include "Object/expr/expr_visitor.h"

namespace HAC {
namespace entity {
class data_expr;
class expr_dump_context;
class nonmeta_expr_visitor;
using std::istream;
using std::ostream;
using util::persistent_object_manager;
using util::memory::count_ptr;

//=============================================================================
/**
	Just list of run-time (nonmeta) expressions.  
 */
class nonmeta_expr_list : 
		public util::persistent, 	// if dynamically allocated
		public std::vector<count_ptr<const data_expr> > {
	typedef	nonmeta_expr_list			this_type;
	typedef	count_ptr<const data_expr>		expr_value_type;
	typedef	std::vector<expr_value_type>		list_type;
public:
	typedef	list_type::const_iterator		const_iterator;
	typedef	list_type::iterator			iterator;
public:
	nonmeta_expr_list();
	~nonmeta_expr_list();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	using list_type::begin;
	using list_type::end;

	void
	accept(nonmeta_expr_visitor&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class nonmeta_expr_list

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_EXPR_LIST_H__

