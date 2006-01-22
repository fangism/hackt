/**
	\file "Object/lang/PRS.h"
	Enumerations for production rules.
	$Id: PRS_enum.h,v 1.2 2006/01/22 06:53:03 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_ENUM_H__
#define	__OBJECT_LANG_PRS_ENUM_H__

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

#endif	// __OBJECT_LANG_PRS_ENUM_H__

