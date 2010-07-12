/**
	\file "Object/lang/SPEC_fwd.h"
	Useful forward declarations for SPEC headers.  
	$Id: SPEC_fwd.h,v 1.7 2010/07/12 17:46:59 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_FWD_H__
#define	__HAC_OBJECT_LANG_SPEC_FWD_H__

#include "util/size_t.h"
#include "util/STL/vector_fwd.h"
#include "util/STL/set_fwd.h"
#include "Object/inst/instance_pool_fwd.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
class param_expr;
class const_param;
class unroll_context;
struct bool_tag;
class bool_literal;
template <class> class state_instance;

#if 0
// source types
typedef	std::default_vector<bool_literal>::type	directive_source_group_type;
typedef std::default_vector<directive_source_group_type>::type
						directive_source_nodes_type;
#endif
typedef	std::default_vector<util::memory::count_ptr<const param_expr> >::type	
						directive_source_params_type;
// unrolled types (resolved parameters)
typedef	std::default_set<size_t>::type		directive_node_group_type;
/**
	Vector of sets is needed for support for grouped nodes.  
 */
typedef	std::default_vector<directive_node_group_type>::type	
						directive_base_nodes_type;
typedef	std::default_vector<util::memory::count_ptr<const const_param> >::type	
						directive_base_params_type;

//=============================================================================
namespace PRS {
	class literal;
	class rule_dump_context;
}	// end namespace PRS

//=============================================================================
namespace SPEC {
class footprint;
class footprint_directive;

typedef	util::memory::count_ptr<PRS::literal>		literal_ptr_type;
typedef	state_instance<bool_tag>			bool_instance_type;
typedef	instance_pool<bool_instance_type>		node_pool_type;

class directive_abstract;
class bool_directive;
class directives_set;
class directive_conditional;
class directive_loop;

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_FWD_H__

