/**
	\file "Object/inst/param_value_collection.h"
	Parameter instance collection classes for HAC.  
	This file came from "Object/art_object_instance_param.h"
		in a previous life.  
	$Id: param_value_collection.h,v 1.6.4.1 2006/01/29 04:42:34 fang Exp $
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
using util::memory::count_ptr;
// using util::qmap;
// using util::multikey_map;
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
	param_value_collection(const size_t d);

	param_value_collection(const this_type& t, const footprint& f) :
		parent_type(t, f) { }

	param_value_collection(const scopespace& o, const string& n, 
		const size_t d);

private:
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
virtual	~param_value_collection();

virtual	ostream&
	what(ostream&) const = 0;

private:
	using parent_type::dump;	// don't intend to use directly

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

virtual	bool
	is_partially_unrolled(void) const = 0;

virtual	bool
	is_loop_variable(void) const = 0;

virtual	ostream&
	dump_unrolled_values(ostream& o) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const = 0;

virtual	count_ptr<const param_type_reference>
	get_param_type_ref(void) const = 0;

virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

	/** should just assert fail, forbid reference to param members */
	member_inst_ref_ptr_type
	make_member_meta_instance_reference(const inst_ref_ptr_type& b) const;

	/** appropriate for the context of a template parameter formal */
virtual	count_ptr<const param_expr>
	default_value(void) const = 0;

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

	bool
	template_formal_equivalent(const this_type&) const;

// used by param_expr_list::certify_template_arguments
virtual	good_bool
	may_type_check_actual_param_expr(const param_expr& pe) const = 0;

virtual	good_bool
	must_type_check_actual_param_expr(const const_param& pe) const = 0;

// down-copied from instance_collection_base
protected:
	good_bool
	may_check_expression_dimensions(const param_expr& pr) const;

	good_bool
	must_check_expression_dimensions(const const_param& pr) const;

public:
/**
	whether or not this can be resolved to some static constant value.
	Will also need two flavors.  
 */
	bool
	is_static_constant(void) const;

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

