/**
	\file "AST/reference.h"
	Reference-related parser classes for HAC.
	$Id: reference.h,v 1.1.52.1 2007/07/09 02:40:17 fang Exp $
 */

#ifndef __HAC_AST_REFERENCE_H__
#define __HAC_AST_REFERENCE_H__

#include "AST/expr_list.h"	// for array_concatenation
#include "AST/identifier.h"

namespace HAC {
namespace parser {
//=============================================================================
// class inst_ref_expr defined in "AST/expr_base.h"

//=============================================================================
/**
	Generalized scoped identifier expression.  
	Has two modes: absolute or relative, depending on whether or 
	not the un/qualified identifier was prefixed by an additional
	scope (::) operator.  
	Even short unqualified identifiers are wrapped into this class
	for uniform use.  
	When type checking with this class, remember to check the 
	path mode before searching, and use all but the last identifier
	chain as the namespace path prefix.  
	e.g. for A::B::C, search for namespace match of A::B with member C.  
 */
class id_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr		parent_type;
	/**
		Wraps around a qualified_id.  
	 */
	excl_ptr<qualified_id>	qid;
public:
	explicit
	id_expr(qualified_id* i);

	explicit
	id_expr(const token_identifier&);

	explicit
	id_expr(const count_ptr<const token_identifier>&);

	id_expr(const id_expr&);

	~id_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_REFERENCE_PROTO;
	CHECK_NONMETA_REFERENCE_PROTO;

	never_ptr<const qualified_id>
	get_id(void) const { return qid; }
//		{ return never_ptr<const qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

	excl_ptr<qualified_id>
	release_qualified_id(void) { return qid; }

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const string_punctuation_type* s);

	bool
	is_absolute(void) const;

// want a method for splitting off the last id, isolating namespace portion
	qualified_id
	copy_namespace_portion(void) const
		{ return qid->copy_namespace_portion(); }
		// remember to delete this after done using!?

	friend
	ostream&
	operator << (ostream& o, const id_expr& id);
};	// end class id_expr

//=============================================================================
/// class for member (of user-defined type) expressions
// is not really unary, derive directly from expr?
// final class?
class member_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr		parent_type;
protected:
	const excl_ptr<const inst_ref_expr>	owner;	///< the argument expr
	/// the member name
	const excl_ptr<const token_identifier>	member;
public:
	member_expr(const inst_ref_expr* l, const token_identifier* m);

	// non-default copy-constructor?

	~member_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_REFERENCE_PROTO;
	CHECK_NONMETA_REFERENCE_PROTO;
};	// end class member_expr

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
class index_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr			parent_type;
protected:
	const excl_ptr<const inst_ref_expr>	base;	///< the argument expr
	const excl_ptr<const range_list>	ranges;	///< index
public:
	index_expr(const inst_ref_expr* l, const range_list* i);

	~index_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_REFERENCE_PROTO;
	CHECK_NONMETA_REFERENCE_PROTO;

private:
	range_list_meta_return_type
	intercept_meta_indices_error(const context& c) const;

	inst_ref_expr::meta_return_type
	intercept_base_meta_ref_error(const context& c) const;

	range_list_nonmeta_return_type
	intercept_nonmeta_indices_error(const context& c) const;

	inst_ref_expr::nonmeta_return_type
	intercept_base_nonmeta_ref_error(const context& c) const;
};	// end class index_expr

//=============================================================================
/**
	A collection of references (may be recursive).  
	This is used by spec-directives, that can take group arguments.  
 */
class reference_group_construction : public inst_ref_expr {
protected:
	const excl_ptr<const char_punctuation_type>	lb;
	const excl_ptr<const inst_ref_expr_list>	ex;
	const excl_ptr<const char_punctuation_type>	rb;
public:
	reference_group_construction(const char_punctuation_type* l, 
		const inst_ref_expr_list* e, const char_punctuation_type* r);

	~reference_group_construction();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_REFERENCE_PROTO;
	CHECK_NONMETA_REFERENCE_PROTO;		// unimplemented

	// overrides inst_ref_expr::check_grouped_literals
	bool
	check_grouped_literals(checked_bool_group_type&, const context&) const;

};	// end class reference_group_construction

//=============================================================================

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_REFERENCE_H__

