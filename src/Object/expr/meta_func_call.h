/**
	\file "Object/expr/meta_func_call.h"
	$Id: meta_func_call.h,v 1.1.2.1 2010/09/15 00:57:53 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_FUNC_CALL_H__
#define	__HAC_OBJECT_EXPR_META_FUNC_CALL_H__

#include <string>
#include "Object/expr/param_expr.h"
// #include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/dlfunction_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"	// for never_ptr

namespace HAC {
namespace entity {
class param_expr_list;
class const_param_expr_list;
class dynamic_param_expr_list;
using std::string;
using std::ostream;
using std::istream;
using util::persistent_object_manager;
using util::memory::count_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Class that represents a compile-time function call.  
 */
class meta_func_call : public param_expr {
	typedef	meta_func_call			this_type;
public:
	// Q: is there a good reason for using a pointer, when the
	// dereferenced type should suffice?
	typedef	count_ptr<const dynamic_param_expr_list>
						fargs_ptr_type;
	// is a never_ptr
	typedef	meta_function_ptr_type		function_ptr_type;
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
		mutable function pointer to be bound at compile-time.
		This cannot be retained persistently, of course.  
		The first time this function is encountered, it will 
		try to be resolved against the registry of loaded
		and bound functions (from dlopen, plug-ins, etc.).
	 */
	mutable function_ptr_type		fsym;
public:
	meta_func_call();
	meta_func_call(const string&, const fargs_ptr_type&);
	~meta_func_call();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	const fargs_ptr_type&
	get_func_args(void) const { return fargs; }

	size_t
	dimensions(void) const;

	bool
        may_be_equivalent_generic(const param_expr&) const;

	bool
        must_be_equivalent_generic(const param_expr&) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	count_ptr<const const_param>
	static_constant_param(void) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&,
		const count_ptr<const param_expr>&) const;

#if 0
	count_ptr<const this_type>
	__unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const this_type>&) const;

	UNROLL_RESOLVE_COPY_DATA_PROTO;
	META_RESOLVE_COPY_DATA_PROTO;
	EVALUATE_WRITE_PROTO;
#endif
	void
	accept(nonmeta_expr_visitor&) const;

	SUBSTITUTE_DEFAULT_PARAMETERS_PROTO;
	MAKE_PARAM_EXPRESSION_ASSIGNMENT_PROTO;
	MAKE_AGGREGATE_META_VALUE_REFERENCE_PROTO;

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

#endif	// __HAC_OBJECT_EXPR_META_FUNC_CALL_H__

