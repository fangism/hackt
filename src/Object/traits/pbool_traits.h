/**
	\file "Object/traits/pbool_traits.h"
	Traits and policies for parameter booleans.  
	This file used to be "Object/art_object_pbool_traits.h".
	$Id: pbool_traits.h,v 1.6 2006/01/22 18:20:36 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<pbool_tag> {
	typedef	pbool_tag			tag_type;
	static const char			tag_name[];
	static const char			value_type_name[];
	typedef	pbool_instance			instance_type;
	typedef	pbool_value_type		value_type;
	typedef	bool_value_type			data_value_type;

	typedef	pbool_instance_collection	instance_collection_generic_type;
	typedef	instance_collection_generic_type
						value_collection_generic_type;
	typedef	param_value_collection		value_collection_parent_type;
	template <size_t D>
	struct value_array {
		typedef	entity::value_array<tag_type,D>	type;
	};
	typedef	pbool_const			const_expr_type;
	typedef	pbool_const_collection		const_collection_type;

	typedef	param_instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	pbool_instantiation_statement
					instantiation_statement_type;

	// define this elsewhere, in "traits/inst_stmt_chan.h"
	class instantiation_statement_type_ref_base;

	// this will have a different template base
	typedef	simple_pbool_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_pbool_meta_instance_reference
					simple_meta_instance_reference_type;
	typedef	pbool_instance_reference_base	
					nonmeta_instance_reference_base_type;
	typedef	pbool_meta_instance_reference_base
				meta_instance_reference_parent_type;
	typedef	pbool_instance_reference_base
				nonmeta_instance_reference_parent_type;
	typedef	bool_expr			data_expr_base_type;
	typedef	pbool_expr			expr_base_type;
	typedef const_param			const_collection_parent_type;

	typedef	never_ptr<pbool_instance>	value_reference_ptr_type;
	typedef	packed_array_generic<pbool_value_type, value_reference_ptr_type>
						value_reference_collection_type;
	typedef	pbool_expression_assignment	expression_assignment_type;
	typedef	param_expression_assignment	expression_assignment_parent_type;
	enum { assignment_chunk_size = 64 };

	typedef	void			value_collection_parameter_type;
	typedef	null_parameter_type	instance_collection_parameter_type;

	class instantiation_statement_type_ref_base;

	typedef	param_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;

	/**
		Replacement for built-in type pointer.  
		Initialized in "Object/art_built_ins.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
private:
	static const built_in_param_def		built_in_definition;
};	// end struct class_traits<pbool_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__

