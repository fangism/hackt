/**
	\file "Object/inst/datatype_instance_collection.h"
	Instance collection classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life. 
	$Id: datatype_instance_collection.h,v 1.11 2006/10/18 01:19:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

#include "Object/type/canonical_type_fwd.h"		// for conditional
#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/data_traits.h"

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
#if USE_INSTANCE_PLACEHOLDERS
	datatype_instance_collection() : parent_type() { }
#else
	explicit
	datatype_instance_collection(const size_t d) : parent_type(d) { }

	datatype_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

	datatype_instance_collection(const this_type& t, const footprint& f) :
		parent_type(t, f) { }
#endif

public:
virtual	~datatype_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

#if !USE_INSTANCE_PLACEHOLDERS
virtual	void
	attach_initial_instantiation_statement(
		const never_ptr<const data_instantiation_statement>) = 0;
#endif

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
#if !USE_INSTANCE_PLACEHOLDERS
virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;
#endif

virtual good_bool
	instantiate_indices(const const_range_list& i, 
		const instance_relaxed_actuals_type&, 
		const unroll_context&) = 0;

virtual	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;	// = 0;
	
protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

