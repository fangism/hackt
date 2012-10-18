/**
	\file "Object/expr/meta_func_call.hh"
	$Id: meta_func_call.hh,v 1.2 2010/09/21 00:18:16 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_FUNC_CALL_H__
#define	__HAC_OBJECT_EXPR_META_FUNC_CALL_H__

#include <string>
#include "Object/expr/pint_expr.hh"
#include "Object/expr/pbool_expr.hh"
#include "Object/expr/preal_expr.hh"
#include "Object/expr/pstring_expr.hh"
#include "Object/expr/dlfunction_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "util/memory/excl_ptr.hh"	// for never_ptr

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
	The sub-typed function call all share the same structures
	and call interface.  
 */
class meta_func_call_base {
	// virtual public param_expr?
	typedef	meta_func_call_base		this_type;
public:
	// Q: is there a good reason for using a pointer, when the
	// dereferenced type should suffice?
	typedef	count_ptr<const dynamic_param_expr_list>
						fargs_ptr_type;
	// is a never_ptr
	typedef	meta_function_ptr_type		function_ptr_type;
protected:
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
	meta_func_call_base();
	meta_func_call_base(const string&, const fargs_ptr_type&);
	~meta_func_call_base();

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	const fargs_ptr_type&
	get_func_args(void) const { return fargs; }

	size_t
	dimensions(void) const;

	// this allocates the right type, based on name lookup
	static
//	meta_func_call_base*
	param_expr*
	make_meta_func_call(const string&, const fargs_ptr_type&);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end class meta_func_call_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Function call that is type-checked.  
	This allows calls to be statically bound by their known return type.
	\param B is the parent expression type: pint_expr, pbool_expr, etc...
 */
template <class B>
class meta_func_call : public B, public meta_func_call_base {
	typedef	meta_func_call<B>	this_type;
protected:
	typedef	B			parent_type;
	typedef	typename parent_type::const_expr_type
					const_expr_type;
	typedef	typename parent_type::value_type
					value_type;
public:
	meta_func_call();
	meta_func_call(const string&, const fargs_ptr_type&);
	~meta_func_call();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

	bool
        must_be_equivalent(const parent_type&) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	value_type
	static_constant_value(void) const;

	good_bool
	unroll_resolve_value(const unroll_context&, value_type&) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&) const;

	count_ptr<const const_expr_type>
	__unroll_resolve_rvalue(const unroll_context&,
		const count_ptr<const parent_type>&) const;

	count_ptr<const const_param>
	unroll_resolve_rvalues(const unroll_context&,
		const count_ptr<const parent_type>&) const;

	count_ptr<const parent_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const parent_type>&) const;

	void
	accept(nonmeta_expr_visitor&) const;

	count_ptr<const parent_type>
	substitute_default_positional_parameters(
		const template_formals_manager&,
		const dynamic_param_expr_list&,
		const count_ptr<const parent_type>&) const;

	using parent_type::unroll_resolve_rvalues;
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;
	using parent_type::substitute_default_positional_parameters;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class meta_func_call

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_META_FUNC_CALL_H__

