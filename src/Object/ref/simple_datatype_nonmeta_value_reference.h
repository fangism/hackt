/**
	\file "Object/ref/simple_datatype_nonmeta_value_reference.h"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: simple_datatype_nonmeta_value_reference.h,v 1.3 2005/12/13 04:15:36 fang Exp $
 */

#ifndef	__OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__
#define	__OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__

#include "Object/ref/simple_nonmeta_instance_reference_base.h"

namespace HAC {
namespace entity {
class data_type_reference;
using util::memory::count_ptr;

//=============================================================================
/**
	A reference to a nonmeta data type.  
	Abstract base class.
	NOTE: both true datatypes and nonmeta-dependent 
		meta parameter references fall into this category.  
	This will be the base class interface for all
		simple_nonmeta_value_references.  
 */
class simple_datatype_nonmeta_value_reference :
	public simple_nonmeta_instance_reference_base {
protected:
	typedef	simple_nonmeta_instance_reference_base	parent_type;
protected:
	simple_datatype_nonmeta_value_reference() : parent_type() { }
public:
virtual	~simple_datatype_nonmeta_value_reference() { }

virtual	count_ptr<const data_type_reference>
	get_data_type_ref(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class simple_datatype_nonmeta_value_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_REF_SIMPLE_DATATYPE_NONMETA_VALUE_REFERENCE_H__

