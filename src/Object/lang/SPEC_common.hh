/**
	\file "Object/lang/SPEC_common.hh"
	Tool independent base-classes for macro definitions.  
	$Id: SPEC_common.hh,v 1.7 2010/07/14 18:12:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_COMMON_H__
#define	__HAC_OBJECT_LANG_SPEC_COMMON_H__

#include "Object/lang/PRS_fwd.hh"
#include "Object/lang/directive_definition.hh"
#include "Object/traits/type_tag_enum.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
class cflat_visitor;
class cflat_context_visitor;

namespace SPEC {
/**
	Parent namespace for tool-independent macro classes.  
	These base classes are not registered anywhere.  
 */
namespace directives {
using entity::meta_type_tag_enum;
using util::good_bool;
typedef directive_definition::node_args_type	node_args_type;
typedef directive_definition::param_args_type	param_args_type;

//=============================================================================
/**
	Macro for declaring tool-independent PRS-macro classes.  
	Only used in this header.  
 */
#define	DECLARE_SPEC_COMMON_STRUCT(class_name)				\
struct class_name {							\
	typedef class_name				this_type;	\
public:									\
	static const meta_type_tag_enum			type;		\
	static good_bool __check_num_params(const char*, const size_t);	\
	static good_bool __check_num_nodes(const char*, const size_t);	\
	static good_bool __check_param_args(const char*, 		\
		const param_args_type&);				\
	static good_bool __check_node_args(const char*,			\
		const node_args_type&);					\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_SPEC_COMMON_STRUCT(UnAliased_base)
DECLARE_SPEC_COMMON_STRUCT(Assert)

/**
	The main() function for this directive is tool-independent
	so we implement the common method here.  
 */
struct UnAliased : public UnAliased_base {
	static
	good_bool
	__main(cflat_context_visitor&, const node_args_type&);
};	// end struct UnAliased

DECLARE_SPEC_COMMON_STRUCT(LVS_exclhi)
DECLARE_SPEC_COMMON_STRUCT(LVS_excllo)
DECLARE_SPEC_COMMON_STRUCT(LVS_BDD_order)
DECLARE_SPEC_COMMON_STRUCT(LVS_unstaticized)
DECLARE_SPEC_COMMON_STRUCT(LVS_cross_coupled_inverters)
DECLARE_SPEC_COMMON_STRUCT(SIM_force_exclhi)
DECLARE_SPEC_COMMON_STRUCT(SIM_force_excllo)
DECLARE_SPEC_COMMON_STRUCT(layout_min_sep)
DECLARE_SPEC_COMMON_STRUCT(layout_min_sep_proc)

// intended to denote voltage domain crossing
DECLARE_SPEC_COMMON_STRUCT(supply_x)
DECLARE_SPEC_COMMON_STRUCT(RunModeStatic)

// intended for timing constraint checks
DECLARE_SPEC_COMMON_STRUCT(setup_pos)
DECLARE_SPEC_COMMON_STRUCT(setup_neg)
DECLARE_SPEC_COMMON_STRUCT(hold_pos)
DECLARE_SPEC_COMMON_STRUCT(hold_neg)
// DECLARE_SPEC_COMMON_STRUCT(width_hi)
// DECLARE_SPEC_COMMON_STRUCT(width_low)

#undef	DECLARE_SPEC_COMMON_STRUCT

//=============================================================================
}	// end namespace directives
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_COMMON_H__

