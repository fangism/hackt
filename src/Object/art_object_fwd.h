/**
	\file "art_object_fwd.h"
	Forward declarations for all ART::entity classes and typedefs.
	$Id: art_object_fwd.h,v 1.6.2.1.2.3.2.1 2005/02/20 20:35:51 fang Exp $
 */

#ifndef	__ART_OBJECT_FWD_H__
#define	__ART_OBJECT_FWD_H__

#include <cstddef>			// for size_t

#include "memory/pointer_classes_fwd.h"
#include "STL/deque_fwd.h"

namespace ART {
namespace entity {
	USING_DEQUE
	using namespace util::memory;

// TODO: organize into groups by where full declarations are found

	class module;
	class object;
	class scopespace;
	class sequential_scope;
	class name_space;
	class built_in_datatype_def;
	class fundamental_type_reference;
	class simple_instance_reference;
	class instance_collection_base;
	class instance_reference_base;
	class definition_base;
	class user_def_chan;
	class user_def_type;
	class channel_definition_base;
	class channel_type_reference;
	class channel_instance_collection;
	class datatype_definition_base;
	class data_type_reference;
	class datatype_instance_collection;
	class datatype_instance_reference;
	class process_definition_base;
	class process_definition;
	class process_type_reference;
	class process_instance_collection;
	class built_in_param_def;
	class typedef_base;

	// subclasses of datatype_instance_collection
	class bool_instance_collection;
	class int_instance_collection;
	class enum_instance_collection;
	class struct_instance_collection;

#if 0
	class process_instance_reference;
	class channel_instance_reference;
	class datastruct_instance_reference;
	class bool_instance_reference;
	class int_instance_reference;
	class enum_instance_reference;
#else
	template <class, class>
	class instance_reference;

	typedef	instance_reference<channel_instance_collection, 
			simple_instance_reference>
		channel_instance_reference;
	typedef	instance_reference<process_instance_collection, 
			simple_instance_reference>
		process_instance_reference;
	typedef	instance_reference<bool_instance_collection, 
			datatype_instance_reference>
		bool_instance_reference;
	typedef	instance_reference<int_instance_collection, 
			datatype_instance_reference>
		int_instance_reference;
	typedef	instance_reference<enum_instance_collection, 
			datatype_instance_reference>
		enum_instance_reference;
	typedef	instance_reference<struct_instance_collection, 
			datatype_instance_reference>
		datastruct_instance_reference;
#endif


	template <class>
	class member_instance_reference;

	typedef	member_instance_reference<process_instance_reference>
		process_member_instance_reference;
	typedef	member_instance_reference<channel_instance_reference>
		channel_member_instance_reference;
	typedef	member_instance_reference<bool_instance_reference>
		bool_member_instance_reference;
	typedef	member_instance_reference<int_instance_reference>
		int_member_instance_reference;
	typedef	member_instance_reference<enum_instance_reference>
		enum_member_instance_reference;
	typedef	member_instance_reference<datastruct_instance_reference>
		datastruct_member_instance_reference;

	class param_type_reference;
	class pbool_type_reference;
	class pint_type_reference;

	class param_instance_collection;
	class pbool_instance_collection;
	class pint_instance_collection;

	class pint_instance;
	class pbool_instance;
	class int_instance;
	class bool_instance;
	class enum_instance;
	class struct_instance;

	template <size_t>	class int_instance_alias;
	template <size_t>	class bool_instance_alias;
	class enum_instance_alias;
	class struct_instance_alias;
	class proc_instance_alias;
	class chan_instance_alias;

	class instance_management_base;
	class param_expression_assignment;
	class instance_reference_connection;
	class port_connection;
	class aliases_connection_base;
#if 1
	// defined in "art_object_connect.h"
	class data_alias_connection_base;
	template <class, class>	class alias_connection;
	typedef alias_connection<int_instance_reference, 
			data_alias_connection_base>
		int_alias_connection;
	typedef alias_connection<bool_instance_reference, 
			data_alias_connection_base>
		bool_alias_connection;
	typedef alias_connection<enum_instance_reference,
			data_alias_connection_base>
		enum_alias_connection;
	typedef alias_connection<datastruct_instance_reference,
			data_alias_connection_base>
		datastruct_alias_connection;
	typedef alias_connection<channel_instance_reference,
			aliases_connection_base>
		channel_alias_connection;
	typedef alias_connection<process_instance_reference,
			aliases_connection_base>
		process_alias_connection;
#endif

	class instantiation_statement;
	class param_instantiation_statement;
	class pbool_instantiation_statement;
	class pint_instantiation_statement;
	class channel_instantiation_statement;
	class data_instantiation_statement;
	class process_instantiation_statement;

// expressions and family
	class param_instance_reference;
	class pint_instance_reference;
	class pbool_instance_reference;

	class param_expr;
	class const_param;
	class param_expr_list;
	class const_param_expr_list;
	class dynamic_param_expr_list;
	class index_expr;               // BEWARE also in ART::parser!
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
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
//	class unconditional_range_list;
//	class conditional_range_list;
//	class loop_range_list;
	class index_list;
	class const_index_list;
	class dynamic_index_list;

	/**
		The global integer-type for parameter integers.  
	 */
	typedef	long		pint_value_type;

	/**
		The global boolean-type for parameter integers.  
	 */
	typedef	bool		pbool_value_type;

	class instance_collection_stack_item;

	/**
		Value type of this needs to be more general
		to accommodate loop and conditional scopes?
	 */
	// try to convert this to excl_ptr or sticky_ptr...
	typedef count_ptr<const range_expr_list>
			index_collection_item_ptr_type;

	/**
		UPDATE: now contains reference to instantiation_statements, 
		which *contain* the index/range expressions.  

		We keep track of the state of instance collections at
		various program points with this container.
	 */
	typedef DEFAULT_DEQUE(never_ptr<const instantiation_statement>)
			index_collection_type;

	class unroll_context;

}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_FWD_H__

