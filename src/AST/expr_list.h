/**
	\file "AST/expr_list.h"
	Base set of classes parser expression lists.  
	$Id: expr_list.h,v 1.5 2006/04/12 08:53:12 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_expr_list.h,v 1.8.34.1 2005/12/11 00:45:06 fang Exp
 */

#ifndef __HAC_AST_EXPR_LIST_H__
#define __HAC_AST_EXPR_LIST_H__

#include "AST/expr_base.h"
#include "AST/node_list.h"
#include "util/STL/vector_fwd.h"

namespace HAC {
namespace entity {
class template_actuals;
}
namespace parser {
using std::vector;
using std::default_vector;
//=============================================================================
/**
	List of expressions.  
	Implemented in "AST/expr.cc".  
	All expression lists are comma-separated.  
	No need to sub-class expression lists into 
	template args and actuals.  

 */
typedef node_list<const expr>				expr_list_base;
typedef node_list<const inst_ref_expr>			inst_ref_expr_list_base;

//=============================================================================
/**
	General parser expression list.  
	Class connection_argument_list is derived from expr_list, 
	so we should keep methods virtual.
	TODO: typedef expr_list	connection_argument_list;
	TODO: useful return values to postorder checks.  
 */
class expr_list : public expr_list_base {
protected:
	typedef	expr_list_base			parent_type;
public:
	typedef	parent_type::const_iterator	const_iterator;
	typedef	default_vector<expr::generic_meta_return_type>::type
						checked_meta_generic_type;
	typedef	default_vector<expr::meta_return_type>::type
						checked_meta_exprs_type;
	typedef	default_vector<expr::nonmeta_return_type>::type
						checked_nonmeta_exprs_type;
	typedef	default_vector<inst_ref_meta_return_type>::type
						checked_meta_refs_type;
public:
	expr_list();

	explicit
	expr_list(const expr* e);

virtual	~expr_list();

	using parent_type::leftmost;
	using parent_type::rightmost;

	void
	postorder_check_meta_generic(checked_meta_generic_type&, 
		const context&) const;

	void
	postorder_check_meta_exprs(checked_meta_exprs_type&,
		const context&) const;

	void
	postorder_check_nonmeta_exprs(checked_nonmeta_exprs_type&, 
		const context&) const;

	static
	void
	select_checked_meta_exprs(const checked_meta_generic_type&, 
		checked_meta_exprs_type&);

	static
	void
	select_checked_meta_refs(const checked_meta_generic_type&, 
		checked_meta_refs_type&);

	static
	meta_expr_return_type
	make_aggregate_value_reference(const checked_meta_exprs_type&, 
		const bool cat);

};	// end class expr_list

//=============================================================================
/**
	List of instance references.  
 */
class inst_ref_expr_list : public inst_ref_expr_list_base {
	typedef	inst_ref_expr_list		this_type;
protected:
	typedef	inst_ref_expr_list_base		parent_type;
public:
	typedef	default_vector<prs_literal_ptr_type>::type
						checked_bool_refs_type;
	typedef	default_vector<inst_ref_meta_return_type>::type
						checked_meta_refs_type;
	typedef	default_vector<inst_ref_nonmeta_return_type>::type
						checked_nonmeta_refs_type;
	typedef	default_vector<data_ref_nonmeta_return_type>::type
						checked_nonmeta_data_refs_type;
	/// note the pluralization of groups
	typedef	inst_ref_expr::checked_bool_group_type
					checked_bool_group_type;
	typedef	default_vector<inst_ref_expr::checked_bool_group_type>::type
					checked_bool_groups_type;
public:
	explicit
	inst_ref_expr_list(const inst_ref_expr*);

	~inst_ref_expr_list();

	using parent_type::leftmost;
	using parent_type::rightmost;

	// generic refs (UNUSED)
//	check_meta_references...
	void
	postorder_check_bool_refs(checked_bool_refs_type&, 
		const context&) const;

	// results in a collection of groups
	bool
	postorder_check_grouped_bool_refs(checked_bool_groups_type&, 
		const context&) const;

	// results in a single amorphous group
	bool
	postorder_check_grouped_bool_refs(checked_bool_group_type&, 
		const context&) const;

	void
	postorder_check_meta_refs(checked_meta_refs_type&, 
		const context&) const;

//	check_nonmeta_references...
	void
	postorder_check_nonmeta_refs(checked_nonmeta_refs_type&, 
		const context&) const;

	void
	postorder_check_nonmeta_data_refs(checked_nonmeta_data_refs_type&, 
		const context&) const;

	static
	inst_ref_meta_return_type
	make_aggregate_instance_reference(const checked_meta_refs_type&, 
		const bool cat);

};	// end class inst_ref_expr_list

//=============================================================================
/**
	TODO: once we introduce types in templates, then extend the list type.  
	Contains a strict template argument list and a relaxed template
	argument list.  
	Intended for use with concrete_type_ref.
 */
class template_argument_list_pair {
protected:
	typedef	expr_list			list_type;
	const excl_ptr<const list_type>		strict_args;
	const excl_ptr<const list_type>		relaxed_args;
public:
	typedef	entity::template_actuals	return_type;
public:
	template_argument_list_pair(const list_type*, const list_type*);
	~template_argument_list_pair();

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_template_args(const context& c) const;

};	// end class template_argument_list_pair

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_EXPR_LIST_H__

