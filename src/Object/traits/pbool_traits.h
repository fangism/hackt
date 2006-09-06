/**
	\file "Object/traits/pbool_traits.h"
	Traits and policies for parameter booleans.  
	This file used to be "Object/art_object_pbool_traits.h".
	$Id: pbool_traits.h,v 1.12.8.2 2006/09/06 04:20:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__
#define	__HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__

#include "Object/traits/class_traits.h"

namespace HAC {
namespace entity {
//-----------------------------------------------------------------------------
template <>
struct class_traits<pbool_tag> {
	template <class Tag>
	struct rebind {	typedef	class_traits<Tag>	type; };

	typedef	pbool_tag			tag_type;
	static const char			tag_name[];
	static const char			value_type_name[];
	typedef	pbool_instance			instance_type;
	typedef	pbool_value_type		value_type;
	typedef	bool_value_type			data_value_type;
	enum {		is_nonmeta_data_lvalue = false		};

	typedef	pbool_instance_collection	instance_collection_generic_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	param_value_placeholder		value_placeholder_parent_type;
	typedef	pbool_value_placeholder		instance_placeholder_type;
#endif
	typedef	instance_collection_generic_type
						value_collection_generic_type;
	typedef	param_value_collection		value_collection_parent_type;
	template <size_t D>
	struct value_array {
		typedef	entity::value_array<tag_type,D>	type;
	};
	typedef	pbool_const			const_expr_type;
	typedef	pbool_const_collection		const_collection_type;

	typedef	instantiation_statement_base
					instantiation_statement_parent_type;
	typedef	pbool_instantiation_statement
					instantiation_statement_type;

	// define this elsewhere, in "traits/inst_stmt_chan.h"
	class instantiation_statement_type_ref_base;

	typedef	data_nonmeta_instance_reference
					nonmeta_instance_reference_base_type;
	typedef	simple_pbool_nonmeta_instance_reference
					simple_nonmeta_instance_reference_type;
	typedef	simple_pbool_meta_value_reference
					simple_meta_value_reference_type;
	typedef	pbool_meta_value_reference_base
				meta_value_reference_parent_type;
	typedef	bool_expr			data_expr_base_type;
	typedef	pbool_expr			expr_base_type;
	typedef const_param			const_collection_parent_type;

	typedef	never_ptr<pbool_instance>	value_reference_ptr_type;
	typedef	packed_array_generic<pint_value_type, value_reference_ptr_type>
						value_reference_collection_type;
	typedef	pbool_expression_assignment	expression_assignment_type;
	typedef	param_expression_assignment	expression_assignment_parent_type;
	enum { assignment_chunk_size = 64 };

	typedef	void			value_collection_parameter_type;
	typedef	null_parameter_type	instance_collection_parameter_type;

	typedef	param_type_reference		type_ref_type;
	typedef	fundamental_type_reference	type_ref_parent_type;
	typedef	count_ptr<const type_ref_type>	type_ref_ptr_type;
#if USE_RESOLVED_DATA_TYPES
	// distinguish between meta-type and nonmeta-type
	// typedef	canonical_generic_datatype	resolved_type_ref_type;
	// pointer not necessary
#endif

	/**
		Replacement for built-in type pointer.  
		Initialized in "Object/traits/class_traits_types.cc"
	 */
	static const type_ref_ptr_type		built_in_type_ptr;
private:
	static const built_in_param_def		built_in_definition;
};	// end struct class_traits<pbool_tag>

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TRAITS_PBOOL_TRAITS_H__

