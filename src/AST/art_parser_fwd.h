/**
	\file "art_parser_fwd.h"
	Forward declarations for classes in ART::parser.
	$Id: art_parser_fwd.h,v 1.1 2004/12/06 07:11:09 fang Exp $
 */

#ifndef	__ART_PARSER_FWD_H__
#define	__ART_PARSER_FWD_H__

namespace ART {
namespace parser {

//-----------------------------------------------------------------------------
// declared in "art_parser_base.h"
class node;
class expr;
class terminal;
class token_char;
class token_string;
class token_identifier;
class token_keyword;

template <class T>
class node_list_base;			// defined in "art_parser.tcc"

template <class T, const char[]>
class node_list;			// defined in "art_parser.tcc"

class expr_list;
class root_item;
class root_body;
class qualified_id;
class qualified_id_slice;
class namespace_id;
class id_expr;
class type_base;
class type_id;
class data_type_ref_list;
class chan_type;
class statement;
class incdec_stmt;
class assign_stmt;
class def_body_item;
class definition_body;
class language_body;
class namespace_body;
class using_namespace;
class instance_management;
class template_argument_list;
class concrete_type_ref;

//-----------------------------------------------------------------------------
// declared in "art_parser_definition.h"
class definition;
class prototype;
class signature_base;
class process_signature;
class process_prototype;
class process_def;
class user_data_type_signature;
class user_data_type_prototype;
class user_data_type_def;
class enum_signature;
class enum_prototype;
class enum_member_list;
class enum_def;
class user_chan_type_signature;
class user_chan_type_prototype;
class user_chan_type_def;
class typedef_alias;

//-----------------------------------------------------------------------------
// declared in "art_parser_expr.h"
class paren_expr;
class id_expr;
class range;
class range_list;
class dense_range_list;
class unary_expr;
class prefix_expr;
class postfix_expr;
class member_expr;
class index_expr;
class binary_expr;
class arith_expr;
class relational_expr;
class logical_expr;
class array_concatenation;
class loop_concatenation;
class array_construction;

//-----------------------------------------------------------------------------
// declared in "art_parser_formal.h"
class data_param_id;
class data_param_id_list;
class data_param_decl;
class data_param_decl_list;
class port_formal_id;
class port_formal_id_list;
class port_formal_decl;
class port_formal_decl_list;
class template_formal_id;
class template_formal_id_list;
class template_formal_decl;
class template_formal_decl_list;

//-----------------------------------------------------------------------------
// declared in "art_parser_instance.h"
class connection_argument_list;
class data_type_ref_list;
class instance_management;
class alias_list;
class actuals_base;
class instance_base;
class instance_id_list;
class instance_array;
class instance_declaration;
class instance_connection;
class connection_statement;
class instance_alias;
class loop_instantiation;
class guarded_definition_body;
class guarded_definition_body_list;
class conditional_instantiation;

//-----------------------------------------------------------------------------
// declared in "art_parser_token.h"
class token_int;
class token_float;
class token_string;
class token_identifier;
class qualified_id;
class qualified_id_slice;
class token_keyword;
class token_bool;
class token_else;
class token_quoted_string;
class token_type;
class token_datatype;
class token_int_type;
class token_bool_type;
class token_paramtype;
class token_pbool_type;
class token_pint_type;

//-----------------------------------------------------------------------------
// declared in "Object/art_context.h"
class context;

//-----------------------------------------------------------------------------
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_FWD_H__

