/**
	\file "Object/lang/PRS_macro_common.h"
	Tool independent base-classes for macro definitions.  
	$Id: PRS_macro_common.h,v 1.1.2.1 2006/04/20 03:34:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_MACRO_COMMON_H__
#define	__HAC_OBJECT_LANG_PRS_MACRO_COMMON_H__

#include "Object/lang/PRS_fwd.h"
#include "Object/lang/directive_definition.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
namespace PRS {
/**
	Parent namespace for tool-independent macro classes.  
	These base classes are not registered anywhere.  
 */
namespace macros {
using util::good_bool;
typedef directive_definition::node_args_type	node_args_type;
typedef directive_definition::param_args_type	param_args_type;

//=============================================================================
/**
	Macro for declaring tool-independent PRS-macro classes.  
 */
#define	DECLARE_PRS_MACRO_COMMON_STRUCT(class_name)			\
struct class_name {							\
	typedef class_name				this_type;	\
public:									\
	static good_bool __check_num_params(const char*, const size_t);	\
	static good_bool __check_num_nodes(const char*, const size_t);	\
	static good_bool __check_param_args(const char*, 		\
		const param_args_type&);				\
	static good_bool __check_node_args(const char*,			\
		const node_args_type&);					\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_PRS_MACRO_COMMON_STRUCT(Echo)
DECLARE_PRS_MACRO_COMMON_STRUCT(PassN)
DECLARE_PRS_MACRO_COMMON_STRUCT(PassP)

//=============================================================================
}	// end namespace macros
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_MACRO_COMMON_H__

