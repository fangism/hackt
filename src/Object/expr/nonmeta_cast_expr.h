/**
	\file "Object/expr/nonmeta_cast_expr.h"
	Convert an untyped value at run time.
	For example, the return type of a nonmeta function call
	is unknown until it is invoked.  
	$Id: nonmeta_cast_expr.h,v 1.1.2.1 2007/08/10 06:49:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_H__
#define	__HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_H__

#include "Object/expr/data_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;
using util::persistent_object_manager;
using std::ostream;
using std::istream;

#define	NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE	template <class To, class From>
#define	NONMETA_CAST_EXPR_CLASS		nonmeta_cast_expr<To, From>

//=============================================================================
/**
	Run-time conversion expression from one type to another.
	Unlike convert_expr's, these are intended for run-time conversions
		and type-checking.  
	Basically a wrapped dynamic_cast.  
	Intended To-types: int_expr, bool_expr, real_expr
	\param To type to convert to (through checking).
	\param From type to convert from.  
		Really intended for nonmeta_func_expr, or other data_expr
		direct descendant.
 */
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
class nonmeta_cast_expr : public To {
	typedef	NONMETA_CAST_EXPR_CLASS		this_type;
public:
	typedef	From				rvalue_type;
	typedef	To				result_type;
	typedef	result_type			parent_type;
	typedef	typename result_type::const_expr_type
						const_expr_type;
private:
	count_ptr<const rvalue_type>		rvalue;
public:
	nonmeta_cast_expr();

	explicit
	nonmeta_cast_expr(const count_ptr<const rvalue_type>&);

	~nonmeta_cast_expr();

	size_t
	dimensions(void) const;

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	count_ptr<const result_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const result_type>&) const;

	count_ptr<const const_param>
	nonmeta_resolve_copy(const nonmeta_context_base&, 
		const count_ptr<const result_type>&) const;

	count_ptr<const const_expr_type>
	__nonmeta_resolve_rvalue(const nonmeta_context_base&, 
		const count_ptr<const result_type>&) const;

	EXPR_ACCEPT_VISITOR_PROTO;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

private:
	using result_type::unroll_resolve_copy;
	using result_type::nonmeta_resolve_copy;

};	// end class nonmeta_cast_expr


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_H__

