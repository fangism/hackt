/**
	\file "Object/lang/PRS_footprint.h"
	$Id: PRS_footprint_rule.h,v 1.2 2006/01/22 06:53:04 fang Exp $
 */

#ifndef	__OBJECT_LANG_PRS_FOOTPRINT_RULE_H__
#define	__OBJECT_LANG_PRS_FOOTPRINT_RULE_H__

#include <iosfwd>
#include "util/macros.h"
#include "Object/lang/cflat_visitee.h"

namespace HAC {
namespace entity {
namespace PRS {
using std::ostream;
using std::istream;

//=============================================================================
/**
	Compact and resolved representation of production rule.  
	Implementation is defined in "Object/lang/PRS_footprint.cc".
 */
struct footprint_rule : public cflat_visitee {
	/**
		index to root expression for this node.
		1-indexed.
	 */
	int				expr_index;
	/**
		index to output node (local to this definition).
		1-indexed.  
	 */
	int				output_index;
	/**
		Whether or not is pull-up or down.
		Could use the sign of output index...
	 */
	bool				dir;

	footprint_rule() { }

	footprint_rule(const int e, const int o, const bool d) :
		expr_index(e), output_index(o), dir(d) { }

	void
	write_object_base(ostream&) const;

	void
	load_object_base(istream&);

	void
	accept(cflat_visitor&) const;
};	// end struct footprint_rule

}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_LANG_PRS_FOOTPRINT_RULE_H__
