/**
	\file "Object/sizes.cc"
	Just dumps the sizeof for most HAC::entity classes.
	This file came from "art_persistent_table.cc".
	$Id: sizes.cc,v 1.1 2006/08/23 20:57:15 fang Exp $
 */

#include <iostream>

#include "common/sizes.h"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "Object/sizes.h"

// include all Object/*.h header files to evaluate struct sizes.
#include "Object/module.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "Object/state_manager.h"
#include "Object/def/footprint.h"
#include "Object/def/enum_datatype_def.h"
#include "Object/def/process_definition.h"
#include "Object/def/process_definition_alias.h"
#include "Object/def/user_def_chan.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/datatype_definition_alias.h"
#include "Object/def/channel_definition_alias.h"
#include "Object/def/built_in_datatype_def.h"
#include "Object/def/channel_definition_base.h"
#include "Object/def/param_definition.h"

#include "Object/type/builtin_channel_type_reference.h"
#include "Object/type/canonical_generic_chan_type.h"
#include "Object/type/canonical_type.h"
#include "Object/type/channel_type_reference.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/type/param_type_reference.h"
#include "Object/type/process_type_reference.h"
#include "Object/type/template_actuals.h"

#include "Object/traits/preal_traits.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/int_negation_expr.h"
// #include "Object/expr/real_arith_expr.h"
// #include "Object/expr/real_relational_expr.h"
// #include "Object/expr/real_negation_expr.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/pint_range.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/dynamic_meta_range_list.h"
#include "Object/expr/pbool_logical_expr.h"
#include "Object/expr/pbool_unary_expr.h"
#include "Object/expr/pint_arith_expr.h"
#include "Object/expr/pint_unary_expr.h"
#include "Object/expr/pint_relational_expr.h"
#include "Object/expr/preal_arith_expr.h"
#include "Object/expr/preal_unary_expr.h"
#include "Object/expr/preal_relational_expr.h"

#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_matcher.h"
#include "Object/inst/alias_printer.h"
#include "Object/inst/alias_visitee.h"
#include "Object/inst/alias_visitor.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/bool_instance_collection.h"
#include "Object/inst/channel_collection_type_manager.h"
#include "Object/inst/channel_instance.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/collection_fwd.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/enum_instance.h"
#include "Object/inst/enum_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/instance_alias.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/inst/instance_fwd.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/int_instance.h"
#include "Object/inst/int_instance_collection.h"
#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/pbool_value_collection.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/pint_instance.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/preal_instance.h"
#include "Object/inst/preal_value_collection.h"
#include "Object/inst/process_instance.h"
#include "Object/inst/process_instance_collection.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/struct_instance.h"
#include "Object/inst/struct_instance_collection.h"
#include "Object/inst/subinstance_manager.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/value_collection.h"

#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/ref/aggregate_meta_value_reference.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/meta_reference_union.h"
#include "Object/ref/meta_value_reference.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/references_fwd.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/simple_meta_value_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"


#include "util/multikey_map.h"
#include "util/multikey.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::cerr;
using std::endl;
using util::memory::never_ptr;
using util::memory::some_ptr;
using util::memory::excl_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	Diagnostic for inspecting sizes of classes.  
	Has nothing to do with class persistence. 
 */
ostream&
dump_class_sizes(ostream& o) {
	o << "util library structures:" << endl;
	__dump_class_size<never_ptr<int> >(o);
	__dump_class_size<excl_ptr<int> >(o);
	__dump_class_size<some_ptr<int> >(o);
	__dump_class_size<count_ptr<int> >(o);

	o << "HAC::entity general classes:" << endl;
	__dump_class_size<module>(o);
	__dump_class_size<state_manager>(o);
	__dump_class_size<port_member_context>(o);
	__dump_class_size<port_collection_context>(o);

	o << "HAC::entity definition classes:" << endl;
	__dump_class_size<footprint>(o);
	__dump_class_size<footprint_base<process_tag> >(o);
	__dump_class_size<footprint_manager>(o);
	__dump_class_size<definition_base>(o);
	__dump_class_size<process_definition_base>(o);
	__dump_class_size<process_definition>(o);
	__dump_class_size<channel_definition_base>(o);
	__dump_class_size<user_def_chan>(o);
	__dump_class_size<datatype_definition_base>(o);
	__dump_class_size<user_def_datatype>(o);
	__dump_class_size<enum_datatype_def>(o);
	__dump_class_size<enum_member>(o);
	__dump_class_size<built_in_datatype_def>(o);
	// __dump_class_size<built_in_channel_def>(o);
	__dump_class_size<template_formals_manager>(o);
	__dump_class_size<port_formals_manager>(o);
	__dump_class_size<typedef_base>(o);
	__dump_class_size<process_definition_alias>(o);
	__dump_class_size<channel_definition_alias>(o);
	__dump_class_size<datatype_definition_alias>(o);
	__dump_class_size<built_in_param_def>(o);

	o << "HAC::entity expression classes:" << endl;
	__dump_class_size<data_expr>(o);
	__dump_class_size<param_expr>(o);
	__dump_class_size<bool_expr>(o);
	__dump_class_size<bool_logical_expr>(o);
	__dump_class_size<bool_negation_expr>(o);
	__dump_class_size<pbool_expr>(o);
	__dump_class_size<pbool_logical_expr>(o);
	__dump_class_size<pbool_const>(o);
	__dump_class_size<pbool_const_collection>(o);
	__dump_class_size<int_expr>(o);
	__dump_class_size<int_arith_expr>(o);
	__dump_class_size<int_relational_expr>(o);
	__dump_class_size<int_negation_expr>(o);
	__dump_class_size<pint_expr>(o);
	__dump_class_size<pint_unary_expr>(o);
	__dump_class_size<pint_arith_expr>(o);
	__dump_class_size<pint_relational_expr>(o);
	__dump_class_size<pint_const>(o);
	__dump_class_size<pint_const_collection>(o);
	__dump_class_size<real_expr>(o);
	__dump_class_size<preal_expr>(o);
	__dump_class_size<preal_arith_expr>(o);
	__dump_class_size<preal_unary_expr>(o);
	__dump_class_size<preal_relational_expr>(o);
	__dump_class_size<preal_const>(o);
	__dump_class_size<preal_const_collection>(o);
	__dump_class_size<meta_index_expr>(o);
	__dump_class_size<const_index>(o);
	__dump_class_size<const_param>(o);
	__dump_class_size<meta_range_expr>(o);
	__dump_class_size<const_range>(o);
	__dump_class_size<pint_range>(o);
	__dump_class_size<param_expr_list>(o);
	__dump_class_size<const_param_expr_list>(o);
	__dump_class_size<dynamic_param_expr_list>(o);
	__dump_class_size<meta_index_list>(o);
	__dump_class_size<const_index_list>(o);
	__dump_class_size<dynamic_meta_index_list>(o);
	__dump_class_size<meta_range_list>(o);
	__dump_class_size<const_range_list>(o);
	__dump_class_size<dynamic_meta_range_list>(o);
	__dump_class_size<nonmeta_index_expr_base>(o);
	__dump_class_size<nonmeta_range_expr_base>(o);

	o << "HAC::entity type classes:" << endl;
	__dump_class_size<type_reference_base>(o);
	__dump_class_size<fundamental_type_reference>(o);
	__dump_class_size<channel_type_reference_base>(o);
	__dump_class_size<channel_type_reference>(o);
	__dump_class_size<builtin_channel_type_reference>(o);
	__dump_class_size<canonical_generic_chan_type>(o);
	__dump_class_size<canonical_process_type>(o);
	__dump_class_size<data_type_reference>(o);
	__dump_class_size<process_type_reference>(o);
	__dump_class_size<template_actuals>(o);
	// __dump_class_size<resolved_template_actuals>(o);

	o << "HAC::entity instance classes:" << endl;
	__dump_class_size<instance_collection_base>(o);
	__dump_class_size<physical_instance_collection>(o);
	__dump_class_size<param_value_collection>(o);
	__dump_class_size<bool_instance_collection>(o);
	__dump_class_size<int_instance_collection>(o);
	__dump_class_size<channel_instance_collection>(o);
	__dump_class_size<substructure_alias_base<true> >(o);
	__dump_class_size<substructure_alias_base<false> >(o);
	__dump_class_size<subinstance_manager>(o);
	__dump_class_size<bool_instance_collection>(o);
	__dump_class_size<bool_instance>(o);
	__dump_class_size<instance_alias_info<bool_tag> >(o);
	__dump_class_size<instance_alias<bool_tag, 1> >(o);
	__dump_class_size<instance_alias<bool_tag, 4> >(o);
	__dump_class_size<bool_scalar>(o);
	__dump_class_size<bool_array_1D>(o);
	__dump_class_size<bool_array_4D>(o);
	__dump_class_size<process_instance_collection>(o);
	__dump_class_size<process_instance>(o);
	__dump_class_size<instance_alias_info<process_tag> >(o);
	__dump_class_size<instance_alias<process_tag, 1> >(o);
	__dump_class_size<instance_alias<process_tag, 4> >(o);
	__dump_class_size<process_scalar>(o);
	__dump_class_size<process_array_1D>(o);
	__dump_class_size<process_array_4D>(o);
	__dump_class_size<pint_instance_collection>(o);
	__dump_class_size<pint_instance>(o);
	__dump_class_size<pint_scalar>(o);
	__dump_class_size<pint_array_1D>(o);
	__dump_class_size<pint_array_4D>(o);
	__dump_class_size<pbool_instance_collection>(o);
	__dump_class_size<pbool_instance>(o);
	__dump_class_size<pbool_scalar>(o);
	__dump_class_size<pbool_array_1D>(o);
	__dump_class_size<pbool_array_4D>(o);

	o << "HAC::entity reference classes:" << endl;
	__dump_class_size<nonmeta_instance_reference_base>(o);
	__dump_class_size<meta_instance_reference_base>(o);
	__dump_class_size<simple_meta_indexed_reference_base>(o);
	__dump_class_size<simple_nonmeta_instance_reference_base>(o);
	// __dump_class_size<simple_param_meta_value_reference>(o);
	__dump_class_size<aggregate_meta_value_reference_base>(o);
	__dump_class_size<aggregate_meta_instance_reference_base>(o);
	__dump_class_size<bool_meta_instance_reference_base>(o);
	__dump_class_size<process_meta_instance_reference_base>(o);
	__dump_class_size<pbool_meta_value_reference_base>(o);
	__dump_class_size<pint_meta_value_reference_base>(o);
	__dump_class_size<simple_bool_nonmeta_instance_reference>(o);
	__dump_class_size<simple_process_nonmeta_instance_reference>(o);
	__dump_class_size<simple_bool_meta_instance_reference>(o);
	__dump_class_size<simple_process_meta_instance_reference>(o);
	__dump_class_size<aggregate_bool_meta_instance_reference>(o);
	__dump_class_size<aggregate_process_meta_instance_reference>(o);
	__dump_class_size<bool_member_meta_instance_reference>(o);
	__dump_class_size<process_member_meta_instance_reference>(o);

	return o;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

