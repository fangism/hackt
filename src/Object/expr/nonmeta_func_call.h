/**
	\file "Object/expr/nonmeta_func_call.h"
	$Id: nonmeta_func_call.h,v 1.1.2.1 2007/07/20 21:07:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_FUNC_CALL_H__
#define	__HAC_OBJECT_EXPR_NONMETA_FUNC_CALL_H__

#include <string>
#include "Object/expr/data_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class nonmeta_expr_list;
using std::string;
using std::ostream;
using std::istream;
using util::persistent_object_manager;

//=============================================================================
/**
	Class that represents a run-time function call.  
 */
class nonmeta_func_call : public data_expr {
	typedef	nonmeta_func_call		this_type;
public:
	typedef	count_ptr<const nonmeta_expr_list>	fargs_ptr_type;
private:
	/**
		Name of function to call.
		Will be mapped to a dynamically loaded symbol eventually.
	 */
	string					fname;
	/**
		Argument expressions.
	 */
	fargs_ptr_type				fargs;
	// mutable function pointer to be bound at run-time
public:
	nonmeta_func_call();
	~nonmeta_func_call();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	const fargs_ptr_type&
	get_func_args(void) const { return fargs; }

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;
#if USE_DATA_EXPR_EQUIVALENCE
	DATA_EXPR_MAY_EQUIVALENCE_PROTO;
#endif
	UNROLL_RESOLVE_COPY_DATA_PROTO;
	NONMETA_RESOLVE_COPY_DATA_PROTO;
	EVALUATE_WRITE_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class data_expr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_FUNC_CALL_H__

