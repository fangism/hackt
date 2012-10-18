/**
	\file "Object/sizes-entity.cc"
	Just dumps the sizeof for most HAC::entity classes.
	This file came from "art_persistent_table.cc".
	$Id: sizes-entity.cc,v 1.3 2010/04/07 00:12:32 fang Exp $
 */

#include <iostream>

#include "common/sizes-common.hh"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "Object/sizes-entity.hh"

// include all Object/*.hh header files to evaluate struct sizes.
#include "Object/module.hh"
#include "Object/global_entry.hh"
#include "Object/def/footprint.hh"
#include "Object/def/enum_datatype_def.hh"
#include "Object/def/process_definition.hh"
#include "Object/def/process_definition_alias.hh"
#include "Object/def/user_def_chan.hh"
#include "Object/def/user_def_datatype.hh"
#include "Object/def/datatype_definition_alias.hh"
#include "Object/def/channel_definition_alias.hh"
#include "Object/def/built_in_datatype_def.hh"
#include "Object/def/channel_definition_base.hh"
#include "Object/def/param_definition.hh"

#include "Object/type/builtin_channel_type_reference.hh"
#include "Object/type/canonical_generic_chan_type.hh"
#include "Object/type/canonical_type.hh"
#include "Object/type/channel_type_reference.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/type/fundamental_type_reference.hh"
#include "Object/type/param_type_reference.hh"
#include "Object/type/process_type_reference.hh"
#include "Object/type/template_actuals.hh"

#include "Object/traits/preal_traits.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/bool_logical_expr.hh"
#include "Object/expr/bool_negation_expr.hh"
#include "Object/expr/int_arith_expr.hh"
#include "Object/expr/int_relational_expr.hh"
#include "Object/expr/int_negation_expr.hh"
// #include "Object/expr/real_arith_expr.hh"
// #include "Object/expr/real_relational_expr.hh"
// #include "Object/expr/real_negation_expr.hh"
#include "Object/expr/const_collection.hh"
#include "Object/expr/const_index.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/pint_range.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/dynamic_meta_index_list.hh"
#include "Object/expr/const_range_list.hh"
#include "Object/expr/dynamic_meta_range_list.hh"
#include "Object/expr/pbool_logical_expr.hh"
#include "Object/expr/pbool_unary_expr.hh"
#include "Object/expr/pint_arith_expr.hh"
#include "Object/expr/pint_unary_expr.hh"
#include "Object/expr/pint_relational_expr.hh"
#include "Object/expr/preal_arith_expr.hh"
#include "Object/expr/preal_unary_expr.hh"
#include "Object/expr/preal_relational_expr.hh"

#include "Object/inst/alias_actuals.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_printer.hh"
#include "Object/inst/alias_visitee.hh"
#include "Object/inst/alias_visitor.hh"
#include "Object/inst/bool_instance.hh"
#include "Object/inst/bool_instance_collection.hh"
#include "Object/inst/channel_instance.hh"
#include "Object/inst/channel_instance_collection.hh"
#include "Object/inst/collection_fwd.hh"
#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/enum_instance.hh"
#include "Object/inst/enum_instance_collection.hh"
#include "Object/inst/general_collection_type_manager.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_collection_base.hh"
#include "Object/inst/instance_array.hh"
#include "Object/inst/instance_scalar.hh"
#include "Object/inst/port_formal_array.hh"
#include "Object/inst/port_actual_collection.hh"
#include "Object/inst/instance_fwd.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/int_collection_type_manager.hh"
#include "Object/inst/int_instance.hh"
#include "Object/inst/int_instance_collection.hh"
#include "Object/inst/internal_aliases_policy.hh"
#include "Object/inst/null_collection_type_manager.hh"
#include "Object/inst/param_value_collection.hh"
#include "Object/inst/parameterless_collection_type_manager.hh"
#include "Object/inst/pbool_instance.hh"
#include "Object/inst/pbool_value_collection.hh"
#include "Object/inst/physical_instance_collection.hh"
#include "Object/inst/pint_instance.hh"
#include "Object/inst/pint_value_collection.hh"
#include "Object/inst/port_alias_tracker.hh"
#include "Object/inst/preal_instance.hh"
#include "Object/inst/preal_value_collection.hh"
#include "Object/inst/process_instance.hh"
#include "Object/inst/process_instance_collection.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/struct_instance.hh"
#include "Object/inst/struct_instance_collection.hh"
#include "Object/inst/subinstance_manager.hh"
#include "Object/inst/substructure_alias_base.hh"
#include "Object/inst/value_collection.hh"
#include "Object/inst/value_scalar.hh"
#include "Object/inst/value_array.hh"
#include "Object/inst/value_placeholder.hh"
#include "Object/inst/instance_placeholder.hh"
#include "Object/inst/datatype_instance_placeholder.hh"

#include "Object/ref/aggregate_meta_instance_reference.hh"
#include "Object/ref/aggregate_meta_value_reference.hh"
#include "Object/ref/data_nonmeta_instance_reference.hh"
#include "Object/ref/inst_ref_implementation.hh"
#include "Object/ref/member_meta_instance_reference.hh"
#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/meta_reference_union.hh"
#include "Object/ref/meta_value_reference.hh"
#include "Object/ref/nonmeta_instance_reference_subtypes.hh"
#include "Object/ref/references_fwd.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/simple_meta_value_reference.hh"
#include "Object/ref/simple_nonmeta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"

#include "util/multikey_map.hh"
#include "util/multikey.hh"
#include "util/packed_array.hh"

namespace HAC {
namespace entity {
using std::ostream;
using std::cerr;
using std::endl;
using util::memory::never_ptr;
using util::memory::some_ptr;
using util::memory::excl_ptr;
using util::memory::count_ptr;
using util::packed_array;
using util::packed_array_generic;

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
	// __dump_class_size<packed_array<1, size_t, char*> >(o);	// error
	// TODO: need specialization on coeffs_type here
	__dump_class_size<packed_array<4, size_t, char*> >(o);
	__dump_class_size<packed_array_generic<size_t, char*> >(o);

	o << "HAC::entity general classes:" << endl;
	__dump_class_size<module>(o);

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
	__dump_class_size<port_actual_collection<bool_tag> >(o);
	__dump_class_size<bool_instance>(o);
	__dump_class_size<instance_alias_info<bool_tag> >(o);
	__dump_class_size<bool_scalar>(o);
	__dump_class_size<bool_array_1D>(o);
	__dump_class_size<bool_array_4D>(o);
	__dump_class_size<bool_port_formal_array>(o);
	__dump_class_size<process_instance_collection>(o);
	__dump_class_size<port_actual_collection<process_tag> >(o);
	__dump_class_size<process_instance>(o);
	__dump_class_size<instance_alias_info<process_tag> >(o);
	__dump_class_size<process_scalar>(o);
	__dump_class_size<process_array_1D>(o);
	__dump_class_size<process_array_4D>(o);
	__dump_class_size<process_port_formal_array>(o);
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
	__dump_class_size<instance_placeholder_base>(o);
	__dump_class_size<physical_instance_placeholder>(o);
	__dump_class_size<param_value_placeholder>(o);
	__dump_class_size<bool_instance_placeholder>(o);
	__dump_class_size<int_instance_placeholder>(o);
	__dump_class_size<process_instance_placeholder>(o);
	__dump_class_size<pbool_value_placeholder>(o);
	__dump_class_size<pint_value_placeholder>(o);
	__dump_class_size<preal_value_placeholder>(o);

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

