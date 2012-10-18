/**
	\file "Object/type/template_actuals.hh"
	Class for template arguments, a.k.a. actuals.  
	This file came from "Object/type/template_actuals.h"
		in a previous life.  
	$Id: template_actuals.hh,v 1.9 2006/10/24 07:27:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_TEMPLATE_ACTUALS_H__
#define	__HAC_OBJECT_TYPE_TEMPLATE_ACTUALS_H__

#include <iosfwd>
#include "util/string_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace util {
class persistent_object_manager;
}

namespace HAC {
namespace entity {
class unroll_context;
class dynamic_param_expr_list;
class const_param_expr_list;
class fundamental_type_reference;
class template_formals_manager;
class param_expr;
using std::string;
using std::istream;
using std::ostream;
using util::persistent_object_manager;
using util::memory::count_ptr;

//=============================================================================

/**
	Bundle of template arguments for making complete type-references.
 */
class template_actuals {
friend class fundamental_type_reference;
public:
	typedef	template_actuals		this_type;
	// later this will become more generalized, as template_expr_list_type
	typedef	dynamic_param_expr_list		expr_list_type;
	typedef	count_ptr<expr_list_type>	arg_list_ptr_type;
	typedef	count_ptr<const expr_list_type>	const_arg_list_ptr_type;
protected:
	const_arg_list_ptr_type			strict_template_args;
	const_arg_list_ptr_type			relaxed_template_args;
public:
	template_actuals();

	template_actuals(const const_arg_list_ptr_type& s,
		const const_arg_list_ptr_type& r);

	// special purpose constructor
	template_actuals(const template_actuals& t, 
		const const_arg_list_ptr_type&);

	// use default copy-constructor

	~template_actuals();

	string
	dump_string(void) const;

	ostream&
	dump(ostream&) const;

	const_arg_list_ptr_type
	get_strict_args(void) const;

#if 0
	arg_list_ptr_type
	get_strict_args(void);
#else
	void
	replace_strict_args(const const_arg_list_ptr_type&);
#endif

	const_arg_list_ptr_type
	get_relaxed_args(void) const;

	count_ptr<const const_param_expr_list>
	make_const_param_list(void) const;

	bool
	is_resolved(void) const;

	operator bool () const;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	bool
	is_constant(void) const;

	bool
	is_dynamically_parameter_dependent(void) const;

	bool
	is_strictly_compatible_with(const this_type&) const;

	bool
	may_be_strict_equivalent(const this_type&) const;

	bool
	must_be_strict_equivalent(const this_type&) const;

	bool
	may_be_relaxed_equivalent(const this_type&) const;

	bool
	must_be_relaxed_equivalent(const this_type&) const;

	this_type
	unroll_resolve(const unroll_context&) const;

	this_type
	transform_template_actuals(const this_type&, 
		const template_formals_manager&) const;

	// is_constant
	// is_...

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class template_actuals

//=============================================================================
#if 0
/**
	Template parameters resolved to constants.  
	Used by canonical_type.  
	Needs to be comparable.  
	NOTE: making resolved parameters one list make it easier
		to compare and sort them.  
 */
class resolved_template_actuals {
	typedef	resolved_template_actuals	this_type;
public:
	typedef	count_ptr<const const_param_expr_list>	const_arg_list_ptr_type;
private:
	const_arg_list_ptr_type		strict_template_args;
	const_arg_list_ptr_type		relaxed_template_args;
public:
	resolved_template_actuals();

	resolved_template_actuals(const const_arg_list_ptr_type&, 
		const const_arg_list_ptr_type&);

	~resolved_template_actuals();

};	// end class resolved_template_actuals
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_TEMPLATE_ACTUALS_H__

