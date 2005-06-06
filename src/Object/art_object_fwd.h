/**
	\file "Object/art_object_fwd.h"
	Forward declarations for all ART::entity classes and typedefs.
	$Id: art_object_fwd.h,v 1.13.2.1.2.5 2005/06/06 09:25:57 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_FWD_H__
#define	__OBJECT_ART_OBJECT_FWD_H__

#include "util/size_t.h"
#include "Object/art_object_classification_tags.h"
#include "Object/art_object_classification_fwd.h"
#include "Object/art_object_expr_types.h"

#define	USE_DATA_REFERENCE		0

namespace ART {
namespace entity {

// TODO: organize into groups by where full declarations are found

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class built_in_datatype_def;
	class fundamental_type_reference;
	class simple_meta_instance_reference_base;
	class simple_nonmeta_instance_reference_base;
	class instance_collection_base;
	class physical_instance_collection;
	class meta_instance_reference_base;
	class definition_base;
	class user_def_chan;
	class user_def_type;
	class channel_definition_base;
	class channel_type_reference_base;
	class builtin_channel_type_reference;
	class channel_type_reference;
	class datatype_definition_base;
	class enum_datatype_def;
	class data_type_reference;
	class datatype_instance_collection;
	class datatype_meta_instance_reference_base;
	class process_definition_base;
	class process_definition;
	class process_type_reference;
	class built_in_param_def;
	class built_in_channel_def;
	class typedef_base;

	// from "Object/art_object_nonmeta_inst_ref.h"
	// note there are generic (non-meta) abstract base classes from which
	// the meta-versions are derived.
	class nonmeta_instance_reference_base;
	class channel_instance_reference_base;
	class process_instance_reference_base;
	class datatype_instance_reference_base;
	class param_instance_reference_base;
	class int_instance_reference_base;
	class bool_instance_reference_base;
	class enum_instance_reference_base;
	class struct_instance_reference_base;
	class pint_instance_reference_base;
	class pbool_instance_reference_base;

	template <class>
	class simple_meta_instance_reference;
	template <class>
	class simple_nonmeta_instance_reference;

	template <class>
	class instance_collection;

	template <class, size_t>
	class instance_array;
	template <class, size_t>
	class value_array;

	// subclasses of datatype_instance_collection
	typedef	instance_collection<bool_tag>
		bool_instance_collection;
	typedef	instance_collection<int_tag>
		int_instance_collection;
	typedef	instance_collection<enum_tag>
		enum_instance_collection;
	typedef	instance_collection<datastruct_tag>
		struct_instance_collection;
	typedef	instance_collection<channel_tag>
		channel_instance_collection;
	typedef	instance_collection<process_tag>
		process_instance_collection;

#if USE_DATA_REFERENCE
	template <class>
	class data_reference;
#endif

	typedef	simple_meta_instance_reference<channel_tag>
		channel_meta_instance_reference;
	typedef	simple_meta_instance_reference<process_tag>
		process_meta_instance_reference;
#if USE_DATA_REFERENCE
	typedef	data_reference<bool_tag>
		bool_meta_instance_reference;
	typedef	data_reference<int_tag>
		int_meta_instance_reference;
#else
	typedef	simple_meta_instance_reference<bool_tag>
		bool_meta_instance_reference;
	typedef	simple_meta_instance_reference<int_tag>
		int_meta_instance_reference;
#endif
	typedef	simple_meta_instance_reference<enum_tag>
		enum_meta_instance_reference;
	typedef	simple_meta_instance_reference<datastruct_tag>
		datastruct_meta_instance_reference;

	template <class>
	class member_meta_instance_reference;

	typedef	member_meta_instance_reference<process_tag>
		process_member_meta_instance_reference;
	typedef	member_meta_instance_reference<channel_tag>
		channel_member_meta_instance_reference;
	typedef	member_meta_instance_reference<bool_tag>
		bool_member_meta_instance_reference;
	typedef	member_meta_instance_reference<int_tag>
		int_member_meta_instance_reference;
	typedef	member_meta_instance_reference<enum_tag>
		enum_member_meta_instance_reference;
	typedef	member_meta_instance_reference<datastruct_tag>
		datastruct_member_meta_instance_reference;

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;

	template <class>
	class value_collection;
	class param_instance_collection;
	typedef	value_collection<pint_tag>
		pint_instance_collection;
	typedef	value_collection<pbool_tag>
		pbool_instance_collection;

	class pint_instance;
	class pbool_instance;
	class int_instance;
	class bool_instance;
	class enum_instance;
	class struct_instance;
	class channel_instance;
	class process_instance;

	template <class>		class instance_alias_info;
	template <class, size_t>	class instance_alias;

	class instance_management_base;
	class param_expression_assignment;
	template <class>
	class expression_assignment;
	typedef	expression_assignment<pbool_tag>
		pbool_expression_assignment;
	typedef	expression_assignment<pint_tag>
		pint_expression_assignment;

	// defined in "art_object_connect.h"
	class meta_instance_reference_connection;
	class port_connection;
	class aliases_connection_base;
	class data_alias_connection_base;
	template <class>	class alias_connection;
	typedef alias_connection<int_tag>
		int_alias_connection;
	typedef alias_connection<bool_tag>
		bool_alias_connection;
	typedef alias_connection<enum_tag>
		enum_alias_connection;
	typedef alias_connection<datastruct_tag>
		datastruct_alias_connection;
	typedef alias_connection<channel_tag>
		channel_alias_connection;
	typedef alias_connection<process_tag>
		process_alias_connection;

	template <class>
	class instantiation_statement;
	class instantiation_statement_base;
	class param_instantiation_statement;

	typedef	instantiation_statement<pbool_tag>
		pbool_instantiation_statement;
	typedef	instantiation_statement<pint_tag>
		pint_instantiation_statement;
	typedef	instantiation_statement<datatype_tag>
		data_instantiation_statement;
	typedef	instantiation_statement<channel_tag>
		channel_instantiation_statement;
	typedef	instantiation_statement<process_tag>
		process_instantiation_statement;

// expressions and family
	class param_meta_instance_reference;
	template <class>
	class value_reference;
	typedef	value_reference<pint_tag>
		pint_meta_instance_reference;
	typedef	value_reference<pbool_tag>
		pbool_meta_instance_reference;

	// from "Object/art_object_data_expr[_base].h"
	class data_expr;
	class bool_expr;
	class int_expr;
	class int_arith_expr;
	class int_range_expr;
	class nonmeta_index_expr_base;
	class nonmeta_range_expr_base;
	class nonmeta_index_list;
	class nonmeta_range_list;

	class param_expr;
	class const_param;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class meta_index_expr;
	class const_index;
	class pbool_expr;
	class pint_expr;
	class pint_const;
	class pbool_const;
	class param_unary_expr;
	class pint_unary_expr;
	class pbool_unary_expr;
	class param_binary_expr;
	class arith_expr;
	class relational_expr;
	class logical_expr;
	class meta_range_expr;
	class pint_range;
	class const_range;
	class meta_range_list;
	class const_range_list;
	class dynamic_meta_range_list;
//	class unconditional_range_list;
//	class conditional_range_list;
//	class loop_range_list;
	class meta_index_list;
	class const_index_list;
	class dynamic_meta_index_list;

	template <class>
	class const_collection;
	typedef	const_collection<pint_tag>
		pint_const_collection;
	typedef	const_collection<pbool_tag>
		pbool_const_collection;

	class instance_collection_stack_item;

	class unroll_context;

}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_FWD_H__

