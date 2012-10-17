/**
	\file "Object/unroll/unroll_fwd.hh"
	Forward declarations for all unroll-related classes.
	$Id: unroll_fwd.hh,v 1.6 2011/03/23 00:36:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_UNROLL_FWD_H__
#define	__HAC_OBJECT_UNROLL_UNROLL_FWD_H__

#include "Object/traits/classification_tags_fwd.hh"

namespace HAC {
namespace entity {

class instance_management_base;
class param_expression_assignment;
template <class>
class expression_assignment;
typedef expression_assignment<pbool_tag>
	pbool_expression_assignment;
typedef expression_assignment<pint_tag>
	pint_expression_assignment;
typedef expression_assignment<preal_tag>
	preal_expression_assignment;
typedef expression_assignment<pstring_tag>
	pstring_expression_assignment;

// defined in "Object/unroll/*connection*.hh"
class meta_instance_reference_connection;

class port_connection_base;
template <class>	class port_connection;
typedef port_connection<process_tag>	process_port_connection;
typedef port_connection<channel_tag>	channel_port_connection;
typedef port_connection<datastruct_tag>	struct_port_connection;

class aliases_connection_base;
class data_alias_connection_base;
template <class>	class alias_connection;
typedef alias_connection<int_tag>
	int_alias_connection;
typedef alias_connection<bool_tag>
	bool_alias_connection;
typedef alias_connection<enum_tag>
	enum_alias_connection;
typedef alias_connection<real_tag>
	real_alias_connection;
typedef alias_connection<datastruct_tag>
	datastruct_alias_connection;
typedef alias_connection<channel_tag>
	channel_alias_connection;
typedef alias_connection<process_tag>
	process_alias_connection;

class instantiation_statement_base;
template <class>	class instantiation_statement;
typedef instantiation_statement<pbool_tag>
	pbool_instantiation_statement;
typedef instantiation_statement<pint_tag>
	pint_instantiation_statement;
typedef instantiation_statement<preal_tag>
	preal_instantiation_statement;
typedef instantiation_statement<pstring_tag>
	pstring_instantiation_statement;
typedef instantiation_statement<bool_tag>
	bool_instantiation_statement;
typedef instantiation_statement<int_tag>
	int_instantiation_statement;
typedef instantiation_statement<enum_tag>
	enum_instantiation_statement;
typedef instantiation_statement<datatype_tag>
	data_instantiation_statement;
typedef instantiation_statement<channel_tag>
	channel_instantiation_statement;
typedef instantiation_statement<process_tag>
	process_instantiation_statement;

// from "Object/unroll/template_type_completion.hh"
template <class> class template_type_completion;
typedef	template_type_completion<process_tag>
	process_template_type_completion;

// from "Object/unroll/instance_attribute.hh"
template <class> class instance_attribute;
typedef	instance_attribute<bool_tag>
	bool_instance_attribute;
typedef	instance_attribute<int_tag>
	int_instance_attribute;
typedef	instance_attribute<enum_tag>
	enum_instance_attribute;
typedef	instance_attribute<channel_tag>
	channel_instance_attribute;
typedef	instance_attribute<process_tag>
	process_instance_attribute;

// from "Object/unroll/direction_declaration.hh"
template <class> class direction_declaration;
typedef	direction_declaration<bool_tag>
	bool_direction_declaration;
typedef	direction_declaration<int_tag>
	int_direction_declaration;
typedef	direction_declaration<enum_tag>
	enum_direction_declaration;
typedef	direction_declaration<channel_tag>
	channel_direction_declaration;
typedef	direction_declaration<process_tag>
	process_direction_declaration;

class unroll_context;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_UNROLL_FWD_H__

