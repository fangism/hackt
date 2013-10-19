/**
	\file "Object/lang/RTE_footprint_assignment.hh"
	$Id: RTE_footprint_assignment.hh,v 1.9 2010/09/29 00:13:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_RTE_FOOTPRINT_ASSIGNMENT_HH__
#define	__HAC_OBJECT_LANG_RTE_FOOTPRINT_ASSIGNMENT_HH__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/cflat_visitee.hh"
#include "util/macros.h"
#include "util/persistent_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class const_param_expr_list;
struct global_entry_context;
namespace PRS {
class cflat_visitor;
}
namespace RTE {
using PRS::cflat_visitor;
using std::string;
using std::ostream;
using std::istream;
using std::vector;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Compact and resolved representation of production rule.  
	Implementation is defined in "Object/lang/RTE_footprint.cc".
 */
struct footprint_assignment : public PRS::cflat_visitee {
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

	footprint_assignment() { }

	footprint_assignment(const int e, const int o) :
		expr_index(e), output_index(o) { }

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	write_object(const persistent_object_manager& m, ostream& o) const {
		write_object_base(m, o);
	}

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	load_object(const persistent_object_manager& m, istream& i) {
		load_object_base(m, i);
	}

	void
	accept(cflat_visitor&) const;

};	// end struct footprint_assignment

//=============================================================================
}	// end namespace RTE
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_RTE_FOOTPRINT_ASSIGNMENT_HH__
