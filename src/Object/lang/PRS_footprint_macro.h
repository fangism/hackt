/**
	\file "Object/lang/PRS_footprint_macro.h"
	$Id: PRS_footprint_macro.h,v 1.3.134.1 2009/09/02 22:09:26 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

#include "Object/lang/directive_base.h"
#include "Object/lang/cflat_visitee.h"
#include "Object/lang/generic_attribute.h"

namespace HAC {
namespace entity {
namespace PRS {
//=============================================================================
/**
	A named macro taking node indices as arguments.  
 */
class footprint_macro : public cflat_visitee, public directive_base {
public:
#if PRS_LITERAL_ATTRIBUTES
	resolved_attribute_list_type		attributes;
#endif

	footprint_macro() : directive_base() { }

	explicit
	footprint_macro(const string& s) : directive_base(s) { }

	// everything else inherited from directive_base

	void
	accept(cflat_visitor&) const;

#if PRS_LITERAL_ATTRIBUTES
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif

	void
	write_object(const persistent_object_manager& m, ostream& o) const {
		write_object_base(m, o);
	}

	void
	load_object(const persistent_object_manager& m, istream& i) {
		load_object_base(m, i);
	}

};	// end struct footprint_macro

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_MACRO_H__

