/**
	\file "Object/expr/nonmeta_func_call.h"
	$Id: nonmeta_func_call.h,v 1.2.6.1 2007/08/25 08:12:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_FUNC_CALL_H__
#define	__HAC_OBJECT_EXPR_NONMETA_FUNC_CALL_H__

#include <string>
#include "Object/expr/data_expr.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"	// for never_ptr
#include "Object/expr/dlfunction.h"	// for function typedef

namespace HAC {
namespace entity {
class nonmeta_expr_list;
class const_param_expr_list;
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
	// is a never_ptr
	typedef	chp_dlfunction_ptr_type		function_ptr_type;
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
	/**
		mutable function pointer to be bound at run-time.
		This cannot be retained persistently, of course.  
		The first time this function is encountered, it will 
		try to be resolved against the registry of loaded
		and bound functions (from dlopen, plug-ins, etc.).
	 */
	mutable function_ptr_type		fsym;
public:
	nonmeta_func_call();
	nonmeta_func_call(const string&, const fargs_ptr_type&);
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
	count_ptr<const this_type>
	__unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const this_type>&) const;

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

