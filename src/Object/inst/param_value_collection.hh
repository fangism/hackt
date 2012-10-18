/**
	\file "Object/inst/param_value_collection.hh"
	Parameter instance collection classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: param_value_collection.hh,v 1.16 2006/11/07 06:34:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__

#include "Object/inst/instance_collection_base.hh"
#include "Object/expr/types.hh"
#include "util/boolean_types.hh"
#include "util/memory/count_ptr.hh"
#include "util/persistent_fwd.hh"

namespace HAC {
namespace entity {
class const_param;
class param_type_reference;
class meta_value_reference_base;
class param_value_placeholder;
using util::memory::count_ptr;
using util::bad_bool;
using util::good_bool;
using util::persistent_object_manager;

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
	param_value_collection() : parent_type() { }

virtual	~param_value_collection();

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

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_unresolved_type_ref(void) const = 0;

virtual	count_ptr<const param_type_reference>
	get_param_type_ref(void) const = 0;


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
virtual	void
	write_object(const persistent_object_manager&, ostream&) const = 0;

virtual	void
	load_object(footprint&, const persistent_object_manager&, istream&) = 0;
};	// end class param_value_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PARAM_VALUE_COLLECTION_H__

