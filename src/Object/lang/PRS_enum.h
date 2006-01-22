/**
	\file "Object/lang/PRS.h"
	Enumerations for production rules.
	$Id: PRS_enum.h,v 1.3 2006/01/22 18:20:17 fang Exp $
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
	PRS_OR_EXPR_TYPE_ENUM = 3
};

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_ENUM_H__

