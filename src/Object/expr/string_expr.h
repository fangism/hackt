/**
	\file "Object/expr/string_expr.h"
	$Id: string_expr.h,v 1.1.2.1 2007/08/23 00:23:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_STRING_EXPR_H__
#define	__HAC_OBJECT_EXPR_STRING_EXPR_H__

#include <string>
#include "Object/expr/data_expr.h"
#include "Object/expr/const_param.h"

namespace HAC {
namespace entity {
using std::string;
using std::istream;
using util::persistent_object_manager;

//=============================================================================
/**
	For now the only strings supported in the nonmeta languages
	are constant literal strings.  
	Thus, they are only used as arguments to functions, 
	since string lvalues don't exist... yet.  
 */
class string_expr : public data_expr, public const_param
{
	typedef	string_expr			this_type;
private:
	string				_string;
	// any need to keep line information?
public:
	string_expr();
	// implicit OK
	string_expr(const string&);
	~string_expr();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;
//	DATA_EXPR_MAY_EQUIVALENCE_PROTO;
	UNROLL_RESOLVE_COPY_DATA_PROTO;
	NONMETA_RESOLVE_COPY_DATA_PROTO;
	EVALUATE_WRITE_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

// from const_param, param_expr:
	bool
	is_true(void) const;

	bool
	has_static_constant_dimensions(void) const;

	const_range_list
	static_constant_dimensions(void) const;

	count_ptr<const const_param>
	static_constant_param(void) const;

	LESS_OPERATOR_PROTO;

	bool
	may_be_equivalent_generic(const param_expr&) const;

	bool
	must_be_equivalent_generic(const param_expr&) const;

	bool
	is_relaxed_formal_dependent(void) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&,
		const count_ptr<const param_expr>&) const;

	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO;
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO;

// from util::persistent:
	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class string_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_STRING_EXPR_H__

