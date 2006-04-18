/**
	\file "Object/lang/SPEC_registry.h"
	$Id: SPEC_registry.h,v 1.4 2006/04/18 18:42:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__
#define	__HAC_OBJECT_LANG_SPEC_REGISTRY_H__

#include "util/qmap.h"
#include "Object/lang/directive_definition.h"

namespace HAC {
namespace entity {
class state_manager;
namespace PRS {
class cflat_prs_printer;
}
namespace SPEC {
using PRS::cflat_prs_printer;
using std::string;
using util::good_bool;
//=============================================================================
/**
	This is an actual registered entry of a spec definition.  
 */
class spec_definition_entry : public directive_definition {
public:
	spec_definition_entry() : directive_definition() { }

	spec_definition_entry(const string& k, const main_ptr_type m, 
		const check_num_args_ptr_type np = NULL,
		const check_num_args_ptr_type nn = NULL,
		const check_param_args_ptr_type p = NULL,
		const check_node_args_ptr_type n = NULL) :
		directive_definition(k, m, np, nn, p, n) { }

	// everything else inherited from directive_definition

};	// end class spec_definition_entry

//=============================================================================
typedef	util::default_qmap<string, spec_definition_entry>::type
							spec_registry_type;

extern const spec_registry_type				spec_registry;

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_REGISTRY_H__

