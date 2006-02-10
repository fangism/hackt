/**
	\file "Object/lang/PRS_macro_registry.h"
	$Id: PRS_macro_registry.h,v 1.3.2.2 2006/02/10 21:11:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__
#define	__HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

#include "util/qmap.h"
#include "Object/lang/directive_definition.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::string;
//=============================================================================
/**
	This is an actual registered entry of a macro definition.  
 */
class macro_definition_entry : public directive_definition {
public:
	macro_definition_entry() : directive_definition() { }

	macro_definition_entry(const string& k, const main_ptr_type m, 
		const check_num_args_ptr_type np, 
		const check_num_args_ptr_type nn, 
		const check_param_args_ptr_type p,
		const check_node_args_ptr_type c) :
		directive_definition(k, m, np, nn, p, c) { }

	// everything else inherited from directive_definition

};	// end class macro_definition_entry

//=============================================================================
typedef	util::qmap<string, macro_definition_entry>	macro_registry_type;

extern const macro_registry_type			macro_registry;

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// and namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_MACRO_REGISTRY_H__

