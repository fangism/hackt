/**
	\file "sim/chpsim/DependenceCollector.h"
	$Id: DependenceCollector.h,v 1.3.2.2 2007/02/25 03:01:43 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_DEPENDENCECOLLECTOR_H__
#define	__HAC_SIM_CHPSIM_DEPENDENCECOLLECTOR_H__

#include <set>
#include "sim/common.h"
#include "Object/expr/expr_visitor.h"
#include "Object/global_entry_context.h"
#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {
	class footprint;
	class footprint_frame;
	class state_manager;
}
namespace SIM {
namespace CHPSIM {
using entity::footprint;
using entity::footprint_frame;
using entity::state_manager;
using entity::global_entry_context;
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
class StateConstructor;

using entity::int_range_expr;
using entity::nonmeta_index_list;
using entity::nonmeta_range_list;
using entity::const_param_expr_list;
using entity::dynamic_param_expr_list;
using entity::const_index;
using entity::pint_const;
using entity::pbool_const;
using entity::preal_const;
using entity::int_negation_expr;
using entity::bool_negation_expr;
// using entity::real_negation_expr;
using entity::int_arith_expr;
using entity::int_relational_expr;
// using entity::real_arith_expr;
// using entity::real_relational_expr;
using entity::bool_logical_expr;
using entity::int_arith_loop_expr;
using entity::bool_logical_loop_expr;
using entity::channel_probe;
// using entity::real_arith_loop_expr;
using entity::pint_unary_expr;
using entity::pbool_unary_expr;
using entity::preal_unary_expr;
using entity::pint_arith_expr;
using entity::pint_relational_expr;
using entity::preal_arith_expr;
using entity::preal_relational_expr;
using entity::pbool_logical_expr;
using entity::pint_arith_loop_expr;
using entity::pbool_logical_loop_expr;
using entity::preal_arith_loop_expr;
using entity::convert_pint_to_preal_expr;
using entity::pint_range;
using entity::const_range;
using entity::const_range_list;
using entity::dynamic_meta_range_list;
using entity::const_index_list;
using entity::dynamic_meta_index_list;
using entity::pint_const_collection;
using entity::pbool_const_collection;
using entity::preal_const_collection;
using entity::simple_channel_nonmeta_instance_reference;
using entity::simple_process_nonmeta_instance_reference;
using entity::simple_int_nonmeta_instance_reference;
using entity::simple_bool_nonmeta_instance_reference;
using entity::simple_enum_nonmeta_instance_reference;
using entity::simple_datastruct_nonmeta_instance_reference;
using entity::simple_pbool_nonmeta_instance_reference;
using entity::simple_pint_nonmeta_instance_reference;
using entity::simple_preal_nonmeta_instance_reference;
using entity::simple_pbool_nonmeta_value_reference;
using entity::simple_pint_nonmeta_value_reference;
using entity::simple_preal_nonmeta_value_reference;
using entity::simple_bool_meta_instance_reference;
using entity::simple_int_meta_instance_reference;
using entity::simple_channel_meta_instance_reference;
using entity::simple_process_meta_instance_reference;
using entity::simple_enum_meta_instance_reference;
using entity::simple_datastruct_meta_instance_reference;
using entity::aggregate_bool_meta_instance_reference;
using entity::aggregate_int_meta_instance_reference;
using entity::aggregate_channel_meta_instance_reference;
using entity::aggregate_process_meta_instance_reference;
using entity::aggregate_enum_meta_instance_reference;
using entity::aggregate_datastruct_meta_instance_reference;
using entity::bool_member_meta_instance_reference;
using entity::int_member_meta_instance_reference;
using entity::process_member_meta_instance_reference;
using entity::channel_member_meta_instance_reference;
using entity::enum_member_meta_instance_reference;
using entity::datastruct_member_meta_instance_reference;
using entity::simple_pint_meta_value_reference;
using entity::simple_pbool_meta_value_reference;
using entity::simple_preal_meta_value_reference;
using entity::aggregate_pint_meta_value_reference;
using entity::aggregate_pbool_meta_value_reference;
using entity::aggregate_preal_meta_value_reference;

//=============================================================================
typedef	std::set<node_index_type>	dependence_index_set_type;

/**
	Generic base class for sets of dependencies represented as indices.  
	Factored out so we can reference using meta-type tags.  
 */
template <class Tag>
struct dependence_collector_base {
	dependence_index_set_type		_set;

	dependence_collector_base();
	~dependence_collector_base();
};	// end struct dependence_collector_base

//=============================================================================
/**
	Keep synchronized with declarations in expr_visitor.  
 */
struct DependenceSetCollector :
		public entity::nonmeta_expr_visitor, 
		public global_entry_context, 
		public dependence_collector_base<bool_tag>,
		public dependence_collector_base<int_tag>,
		public dependence_collector_base<enum_tag>,
		public dependence_collector_base<channel_tag> {
	typedef	entity::nonmeta_expr_visitor	parent_type;
public:
	DependenceSetCollector(const StateConstructor&);

	~DependenceSetCollector();

#if 0
	template <class Tag>
	const dependence_index_set_type&
	get_set(void) const {
		return dependence_collector_base<Tag>::_set;
	}
#endif

	void
	clear(void);

	using parent_type::visit;

#define	VISIT_PROTO(type)						\
	void visit(const type&)

	VISIT_PROTO(int_arith_expr);
	VISIT_PROTO(int_negation_expr);
	VISIT_PROTO(int_relational_expr);
	VISIT_PROTO(bool_negation_expr);
	VISIT_PROTO(bool_logical_expr);
//	VISIT_PROTO(real_negation_expr);
//	VISIT_PROTO(real_arith_expr);
//	VISIT_PROTO(real_relational_expr);
	VISIT_PROTO(int_arith_loop_expr);
	VISIT_PROTO(bool_logical_loop_expr);
	VISIT_PROTO(channel_probe);
//	VISIT_PROTO(real_arith_loop_expr);
	VISIT_PROTO(int_range_expr);
//	VISIT_PROTO(nonmeta_index_list);
//	VISIT_PROTO(nonmeta_range_list);
//	VISIT_PROTO(const_param_expr_list);
//	VISIT_PROTO(dynamic_param_expr_list);
	VISIT_PROTO(const_index);
	VISIT_PROTO(pint_const);
	VISIT_PROTO(pbool_const);
	VISIT_PROTO(preal_const);
	VISIT_PROTO(pint_unary_expr);
	VISIT_PROTO(pbool_unary_expr);
	VISIT_PROTO(preal_unary_expr);
	VISIT_PROTO(pint_arith_expr);
	VISIT_PROTO(pint_relational_expr);
	VISIT_PROTO(preal_arith_expr);
	VISIT_PROTO(preal_relational_expr);
	VISIT_PROTO(pbool_logical_expr);
	VISIT_PROTO(pint_arith_loop_expr);
	VISIT_PROTO(pbool_logical_loop_expr);
	VISIT_PROTO(preal_arith_loop_expr);
	VISIT_PROTO(convert_pint_to_preal_expr);
	VISIT_PROTO(pint_range);
	VISIT_PROTO(const_range);
//	VISIT_PROTO(const_range_list);
//	VISIT_PROTO(dynamic_meta_range_list);
//	VISIT_PROTO(const_index_list);
//	VISIT_PROTO(dynamic_meta_index_list);

	VISIT_PROTO(pint_const_collection);
	VISIT_PROTO(pbool_const_collection);
	VISIT_PROTO(preal_const_collection);

	VISIT_PROTO(simple_channel_nonmeta_instance_reference);
	VISIT_PROTO(simple_process_nonmeta_instance_reference);
//	simple_datatype_nonmeta_instance_reference;

	VISIT_PROTO(simple_int_nonmeta_instance_reference);
	VISIT_PROTO(simple_bool_nonmeta_instance_reference);
	VISIT_PROTO(simple_enum_nonmeta_instance_reference);
	VISIT_PROTO(simple_datastruct_nonmeta_instance_reference);
	VISIT_PROTO(simple_pbool_nonmeta_instance_reference);
	VISIT_PROTO(simple_pint_nonmeta_instance_reference);
	VISIT_PROTO(simple_preal_nonmeta_instance_reference);

	VISIT_PROTO(simple_channel_meta_instance_reference);
	VISIT_PROTO(simple_process_meta_instance_reference);
	VISIT_PROTO(simple_enum_meta_instance_reference);
	VISIT_PROTO(simple_datastruct_meta_instance_reference);
	VISIT_PROTO(simple_bool_meta_instance_reference);
	VISIT_PROTO(simple_int_meta_instance_reference);


	VISIT_PROTO(aggregate_channel_meta_instance_reference);
	VISIT_PROTO(aggregate_process_meta_instance_reference);
	VISIT_PROTO(aggregate_enum_meta_instance_reference);
	VISIT_PROTO(aggregate_datastruct_meta_instance_reference);
	VISIT_PROTO(aggregate_bool_meta_instance_reference);
	VISIT_PROTO(aggregate_int_meta_instance_reference);

	VISIT_PROTO(process_member_meta_instance_reference);
	VISIT_PROTO(channel_member_meta_instance_reference);
	VISIT_PROTO(enum_member_meta_instance_reference);
	VISIT_PROTO(datastruct_member_meta_instance_reference);
	VISIT_PROTO(bool_member_meta_instance_reference);
	VISIT_PROTO(int_member_meta_instance_reference);

	VISIT_PROTO(simple_pint_meta_value_reference);
	VISIT_PROTO(simple_pbool_meta_value_reference);
	VISIT_PROTO(simple_preal_meta_value_reference);

	VISIT_PROTO(aggregate_pint_meta_value_reference);
	VISIT_PROTO(aggregate_pbool_meta_value_reference);
	VISIT_PROTO(aggregate_preal_meta_value_reference);

#undef	VISIT_PROTO
};	// end class DependenceSetCollector

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_DEPENDENCECOLLECTOR_H__


