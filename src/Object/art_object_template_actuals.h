/**
	\file "Object/art_object_template_actuals.h"
	Class for template arguments, a.k.a. actuals.  
	$Id: art_object_template_actuals.h,v 1.1.4.3 2005/07/07 06:02:22 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TEMPLATE_ACTUALS_H__
#define	__OBJECT_ART_OBJECT_TEMPLATE_ACTUALS_H__

#include <iosfwd>
#include "util/string_fwd.h"
#include "util/memory/count_ptr.h"

namespace util {
class persistent_object_manager;
}

namespace ART {
namespace entity {
class unroll_context;
class param_expr_list;
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
	typedef	param_expr_list			expr_list_type;
	typedef	count_ptr<expr_list_type>	arg_list_ptr_type;
	typedef	count_ptr<const expr_list_type>	const_arg_list_ptr_type;
protected:
	arg_list_ptr_type			strict_template_args;
	const_arg_list_ptr_type			relaxed_template_args;
public:
	template_actuals();

	template_actuals(const arg_list_ptr_type&,
		const const_arg_list_ptr_type&);

	// special purpose constructor
	template_actuals(const template_actuals& t, 
		const const_arg_list_ptr_type&);

	~template_actuals();

	string
	dump_string(void) const;

	ostream&
	dump(ostream&) const;

	const_arg_list_ptr_type
	get_strict_args(void) const;

#if 0
	arg_list_ptr_type
	get_strict_args_const(void) const;
#endif

	arg_list_ptr_type
	get_strict_args(void);

	const_arg_list_ptr_type
	get_relaxed_args(void) const;

#if 0
	arg_list_ptr_type
	get_relaxed_args(void);
#endif

	operator bool () const;

	count_ptr<const param_expr>
	operator [] (const size_t) const;

	bool
	is_constant(void) const;

	bool
	is_dynamically_parameter_dependent(void) const;

	bool
	may_be_strict_equivalent(const this_type&) const;

	bool
	must_be_strict_equivalent(const this_type&) const;

	bool
	may_be_relaxed_equivalent(const this_type&) const;

	bool
	must_be_relaxed_equivalent(const this_type&) const;

	this_type
	unroll_resolve(unroll_context&) const;

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
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_TEMPLATE_ACTUALS_H__

