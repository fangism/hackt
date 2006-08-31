/**
	\file "Object/inst/datatype_instance_placeholder.h"
	Instance placeholder classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life. 
	$Id: datatype_instance_placeholder.h,v 1.1.2.3 2006/08/31 07:28:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__

#include "Object/type/canonical_type_fwd.h"		// for conditional
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/devel_switches.h"
#include "Object/traits/data_traits.h"

namespace HAC {
namespace entity {
class data_type_reference;
class footprint;

//=============================================================================
/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_placeholder : public physical_instance_placeholder {
private:
	typedef	physical_instance_placeholder	parent_type;
	typedef	datatype_instance_placeholder	this_type;
	typedef	class_traits<datatype_tag>	traits_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	count_ptr<const data_type_reference>	type_ref_ptr_type;
#if 0
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	// is not a true canonical type, rather, a base type.  
#endif
#if 0
	typedef	traits_type::instance_placeholder_parameter_type
					instance_placeholder_parameter_type;
#endif
protected:
	explicit
	datatype_instance_placeholder(const size_t d) : parent_type(d) { }

	datatype_instance_placeholder(const scopespace& o, const string& n, 
		const size_t d);

#if 0
	datatype_instance_placeholder(const this_type& t, const footprint& f) :
		parent_type(t, f) { }
#endif
public:

virtual	~datatype_instance_placeholder();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	void
	attach_initial_instantiation_statement(
		const never_ptr<const data_instantiation_statement>) = 0;

	// inappropriate for placeholders
#if 0
virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const = 0;

	good_bool
	establish_placeholder_type(const instance_placeholder_parameter_type&);

	bad_bool
	check_established_type(const instance_placeholder_parameter_type&) const;
#endif

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

#if 0
// this should really do the work with the collection, not placeholder
virtual good_bool
	instantiate_indices(const const_range_list& i, 
		const instance_relaxed_actuals_type&, 
		const unroll_context&) = 0;
#endif

#if 0
virtual	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;	// = 0;
#endif

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__

