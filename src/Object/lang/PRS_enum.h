/**
	\file "Object/lang/PRS.h"
	Enumerations for production rules.
	$Id: PRS_enum.h,v 1.3.114.1 2007/10/05 21:13:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_ENUM_H__
#define	__HAC_OBJECT_LANG_PRS_ENUM_H__

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================

enum {
	PRS_LITERAL_TYPE_ENUM = 0,
	PRS_NOT_EXPR_TYPE_ENUM = 1,
	PRS_AND_EXPR_TYPE_ENUM = 2,
	PRS_OR_EXPR_TYPE_ENUM = 3,
	PRS_NODE_TYPE_ENUM = 4		// introduced with PRS_INTERNAL_NODES
};

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_ENUM_H__

