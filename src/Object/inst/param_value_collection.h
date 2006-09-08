/**
	\file "Object/inst/param_value_collection.h"
	Parameter instance collection classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: param_value_collection.h,v 1.13.8.10 2006/09/08 03:43:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__

#include "Object/inst/instance_collection_base.h"
#include "Object/expr/types.h"
#include "util/boolean_types.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class const_param;
class param_type_reference;
class meta_value_reference_base;
#if USE_INSTANCE_PLACEHOLDERS
class param_value_placeholder;
#endif
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
class param_value_collection : public instance_collection_base {
	typedef	param_value_collection		this_type;
	typedef	instance_collection_base	parent_type;
public:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type
						member_inst_ref_ptr_type;

protected:
#if USE_INSTANCE_PLACEHOLDERS
	param_value_collection() : parent_type() { }
#else
	explicit
	param_value_collection(const size_t d);

	param_value_collection(const this_type& t, const footprint& f) :
		parent_type(t, f) { }

	param_value_collection(const scopespace& o, const string& n, 
		const size_t d);
#endif

private:
#if !USE_INSTANCE_PLACEHOLDERS
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;
#endif
#if USE_INSTANCE_PLACEHOLDERS
public:
	size_t
	get_dimensions(void) const;

	never_ptr<const scopespace>
	get_owner(void) const;

	const string&
	get_name(void) const;

	never_ptr<const instance_placeholder_base>
	__get_placeholder_base(void) const;

virtual	never_ptr<const param_value_placeholder>
	get_placeholder_base(void) const = 0;
#endif

public:
virtual	~param_value_collection();

virtual	ostream&
	what(ostream&) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	bool
	is_partially_unrolled(void) const = 0;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
virtual	bool
	is_loop_variable(void) const = 0;
#endif

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	ostream&
	dump_formal(ostream&, const unroll_context&) const = 0;

virtual	ostream&
	dump_formal(ostream&) const = 0;
#endif

virtual	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const = 0;

virtual	count_ptr<const param_type_reference>
	get_param_type_ref(void) const = 0;

// we want to dissociate values from instances
// these should return NULL, as values are no longer instances
public:
#if !USE_INSTANCE_PLACEHOLDERS
virtual	count_ptr<meta_value_reference_base>
	make_meta_value_reference(void) const = 0;

private:
virtual	count_ptr<nonmeta_instance_reference_base>
	make_nonmeta_instance_reference(void) const = 0;

public:
	/** should just assert fail, forbid reference to param members */
	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type& b) const;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr>
	default_value(void) const = 0;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
// screw compile-time analysis
/**
	A parameter is considered "usable" if it is either initialized
	to another expression, or it is a template formal.  

	Think we may need two flavors of initialization query, 
	because of conservativeness and imprecision:
	"may_be_initialized" and "definitely_initialized"

	TECHNICALITY: what about conditional assignments to a variable?
	Will need assignment expression stack to resolve statically...
	\return true if initialized to an expression.  
	\sa initialize
 */
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

virtual	good_bool
	assign_default_value(const count_ptr<const param_expr>& p) = 0;
#endif

#if !USE_INSTANCE_PLACEHOLDERS
	bool
	template_formal_equivalent(const this_type&) const;
#endif

// used by param_expr_list::certify_template_arguments
virtual	good_bool
	may_type_check_actual_param_expr(const param_expr&) const = 0;

virtual	good_bool
	must_type_check_actual_param_expr(const const_param&, 
		const unroll_context&) const = 0;

// down-copied from instance_collection_base
protected:
	good_bool
	may_check_expression_dimensions(const param_expr&) const;

	good_bool
	must_check_expression_dimensions(const const_param&, 
		const unroll_context&) const;

public:
#if !USE_INSTANCE_PLACEHOLDERS
/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool
	is_static_constant(void) const;
#endif

virtual	const_index_list
	resolve_indices(const const_index_list&) const = 0;

#if 0
NOTE: these functions should only be applicable to simple_param_meta_value_references.  
/**
	Whether or not this parameter is itself a loop index,
	or if indexed, its indices depend on some loop index.
	This may be applicable to instance_collection_base in general.  
 */
	bool is_loop_independent(void) const;

	bool is_unconditional(void) const;
#endif
protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class param_value_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__

