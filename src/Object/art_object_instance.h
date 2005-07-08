/**
	\file "Object/art_object_instance.h"
	Instance collection classes for ART.  
	$Id: art_object_instance.h,v 1.40.4.3 2005/07/08 03:03:45 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_H__

#include "Object/art_object_instance_base.h"
#include "Object/art_object_index.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"


namespace ART {
namespace entity {
//=============================================================================
class data_type_reference;
class const_param_expr_list;
USING_LIST
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
// class instance_collection_base declared in "art_object_instance_base.h"

/**
	Base class for physical entity collections, 
	as opposed to value collections.  
 */
class physical_instance_collection : public instance_collection_base {
private:
	typedef	instance_collection_base	parent_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
protected:
	explicit
	physical_instance_collection(const size_t d) : parent_type(d) { }

	physical_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

public:

virtual	~physical_instance_collection();

	ostream&
	dump(ostream& o) const;

#if 0
	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

#if 0
	// TODO: instance-references, simple and aggregate
	// unveil when the time is right
	good_bool
	may_check_reference_dimensions(...) const;

	good_bool
	must_check_reference_dimensions(...) const;
#endif

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class physical_instance_collection

//=============================================================================
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
		const instance_relaxed_actuals_type&) = 0;

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

#endif	// __OBJECT_ART_OBJECT_INSTANCE_H__

