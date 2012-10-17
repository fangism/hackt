/**
	\file "Object/traits/data_traits.hh"
	Traits and policies for generic data types.  
	This file used to be "Object/art_object_data_traits.h".
	$Id: data_traits.hh,v 1.10 2006/10/18 20:58:20 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_DATA_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_DATA_TRAITS_H__

#include "Object/traits/class_traits.hh"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
/**
	This specialization is only a temporary adaptation from old code
	to new code using class_traits.  
	Plan to do away with this generic type, and subtype.  
 */
template <>
struct class_traits<datatype_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	data_instantiation_statement
					instantiation_statement_type;
	typedef	datatype_instance_collection
					instance_collection_generic_type;
	typedef	datatype_instance_placeholder
					instance_placeholder_type;
	typedef	data_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
	typedef	canonical_generic_datatype	resolved_type_ref_type;
	// pointer not necessary

	/// defined in "Object/unroll/datatype_instantiation_statement.h"
	class instantiation_statement_type_ref_base;
	// not used yet, may become an abstract base class of canonical_type.
	typedef	canonical_generic_datatype
					instance_collection_parameter_type;

};	// end struct class_traits<datatype_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_DATA_TRAITS_H__

