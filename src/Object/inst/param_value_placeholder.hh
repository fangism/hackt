/**
	\file "Object/inst/param_value_placeholder.hh"
	Parameter instance placeholder classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: param_value_placeholder.hh,v 1.6 2011/02/25 23:19:31 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PARAM_VALUE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_PARAM_VALUE_PLACEHOLDER_H__

#include "Object/inst/instance_placeholder_base.hh"
#include "Object/expr/types.hh"
#include "util/boolean_types.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class param_expr;
class const_param;
class param_type_reference;
class meta_value_reference_base;
using util::memory::count_ptr;
using util::bad_bool;
using util::good_bool;

//=============================================================================
/**
	Instance of a built-in parameter type, such as pint and pbool.  
	Virtualizing.  
	Going to sub-type into pint and pbool.  
	TO DO: derive from a interface for template_argument.  
 */
class param_value_placeholder : public instance_placeholder_base {
	typedef	param_value_placeholder		this_type;
	typedef	instance_placeholder_base	parent_type;
public:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;
#if NONMETA_MEMBER_REFERENCES
	typedef	parent_type::nonmeta_ref_ptr_type	nonmeta_ref_ptr_type;
	typedef	parent_type::member_nonmeta_ref_ptr_type
						member_nonmeta_ref_ptr_type;
#endif

protected:
	param_value_placeholder() : parent_type() { }

	explicit
	param_value_placeholder(const size_t d);

	param_value_placeholder(const scopespace& o, const string& n, 
		const size_t d);

public:
virtual	~param_value_placeholder();

virtual	ostream&
	what(ostream&) const = 0;

virtual	bool
	is_loop_variable(void) const = 0;

virtual	ostream&
	dump_formal(ostream&, const unroll_context&) const = 0;

virtual	ostream&
	dump_formal(ostream&) const = 0;

	size_t
	is_template_formal(void) const;

	bool
	is_relaxed_template_formal(void) const;

virtual	count_ptr<const param_type_reference>
	get_param_type_ref(void) const = 0;

// we want to dissociate values from instances
// these should return NULL, as values are no longer instances
public:
virtual	count_ptr<meta_value_reference_base>
	make_meta_value_reference(void) const = 0;

private:
virtual	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const = 0;

public:
	/** should just assert fail, forbid reference to param members */
	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type& b) const;

#if NONMETA_MEMBER_REFERENCES
	member_nonmeta_ref_ptr_type
	make_member_nonmeta_instance_reference(
		const nonmeta_ref_ptr_type& b) const;
#endif

	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr>
	default_value(void) const = 0;

virtual	good_bool
	assign_default_value(const count_ptr<const param_expr>& p) = 0;

	bool
	template_formal_equivalent(const this_type&) const;

// used by param_expr_list::certify_template_arguments
virtual	good_bool
	may_type_check_actual_param_expr(const param_expr&) const = 0;

virtual	good_bool
	must_type_check_actual_param_expr(const const_param&, 
		const unroll_context&) const = 0;

virtual	good_bool
	unroll_assign_formal_parameter(const unroll_context&, 
		const count_ptr<const param_expr>&) const = 0;

// down-copied from instance_placeholder_base
protected:
	good_bool
	may_check_expression_dimensions(const param_expr&) const;

	good_bool
	must_check_expression_dimensions(const const_param&, 
		const unroll_context&) const;

public:
#if 0
NOTE: these functions should only be applicable to simple_param_meta_value_references.  
/**
	Whether or not this parameter is itself a loop index,
	or if indexed, its indices depend on some loop index.
	This may be applicable to instance_placeholder_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class param_value_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PARAM_VALUE_PLACEHOLDER_H__

