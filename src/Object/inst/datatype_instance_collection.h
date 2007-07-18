/**
	\file "Object/inst/datatype_instance_collection.h"
	Instance collection classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life. 
	$Id: datatype_instance_collection.h,v 1.14 2007/07/18 23:28:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

#include "Object/type/canonical_type_fwd.h"		// for conditional
#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/data_traits.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class data_type_reference;
class footprint;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_collection : public physical_instance_collection {
private:
	typedef	physical_instance_collection	parent_type;
	typedef	datatype_instance_collection	this_type;
	typedef	class_traits<datatype_tag>	traits_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	count_ptr<const data_type_reference>	type_ref_ptr_type;
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
	// is not a true canonical type, rather, a base type.  
	typedef	traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
protected:
	datatype_instance_collection() : parent_type() { }

virtual	~datatype_instance_collection();

public:
virtual	ostream&
	what(ostream& o) const = 0;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const = 0;

	good_bool
	establish_collection_type(const instance_collection_parameter_type&);

	bad_bool
	check_established_type(const instance_collection_parameter_type&) const;

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;

virtual good_bool
	instantiate_indices(const const_range_list& i, 
#if !ENABLE_RELAXED_TEMPLATE_PARAMETERS
		const instance_relaxed_actuals_type&, 
#endif
		const unroll_context&) = 0;

virtual	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;	// = 0;
	
};	// end class datatype_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

