/**
	\file "Object/inst/datatype_instance_collection.h"
	Instance collection classes for ART.  
	This file came from "Object/art_object_instance.h" in a previous life. 
	$Id: datatype_instance_collection.h,v 1.3 2005/08/08 16:51:08 fang Exp $
 */

#ifndef	__OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
// #include "Object/common/multikey_index.h"

namespace ART {
namespace entity {
class data_type_reference;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_collection : public physical_instance_collection {
private:
	typedef	physical_instance_collection	parent_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	count_ptr<const data_type_reference>	type_ref_ptr_type;
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
protected:
	explicit
	datatype_instance_collection(const size_t d) : parent_type(d) { }

	datatype_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

public:

virtual	~datatype_instance_collection();

virtual	ostream&
	what(ostream& o) const = 0;

#if 0
	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

virtual	void
	establish_collection_type(const type_ref_ptr_type&) = 0;

	// a better return type?
virtual	bad_bool
	commit_type(const type_ref_ptr_type& ) = 0;

// methods for connection and aliasing?

// need to do this for real... using object not parse tree
//	bool equals_template_formal(const template_formal_decl& tf) const;
virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

virtual good_bool
	instantiate_indices(const const_range_list& i, 
		const instance_relaxed_actuals_type&, 
		const unroll_context&) = 0;

virtual	good_bool
	create_unique_state(const const_range_list&) = 0;

virtual	never_ptr<const const_param_expr_list>
	get_actual_param_list(void) const;	// = 0;
	
protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_DATATYPE_INSTANCE_COLLECTION_H__

