/**
	\file "Object/lang/PRS_footprint_macro.h"
	$Id: PRS_footprint_macro.h,v 1.3 2006/02/10 21:50:40 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

#include "Object/lang/directive_base.h"
#include "Object/lang/cflat_visitee.h"

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================
/**
	A named macro taking node indices as arguments.  
 */
class footprint_macro : public cflat_visitee, public directive_base {
public:
	footprint_macro() : directive_base() { }

	explicit
	footprint_macro(const string& s) : directive_base(s) { }

	// everything else inherited from directive_base

	void
	accept(cflat_visitor&) const;

};	// end struct footprint_macro

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

