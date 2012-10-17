/**
	\file "Object/lang/PRS_footprint.hh"
	$Id: PRS_footprint_rule.hh,v 1.9 2010/09/29 00:13:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_FOOTPRINT_RULE_H__
#define	__HAC_OBJECT_LANG_PRS_FOOTPRINT_RULE_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/cflat_visitee.hh"
#include "Object/lang/generic_attribute.hh"
#include "util/macros.h"
#include "util/persistent_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class const_param_expr_list;
struct global_entry_context;
namespace PRS {
class cflat_visitor;
using std::string;
using std::ostream;
using std::istream;
using std::vector;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Unroll-resolved attributes.  
	Generated from PRS::attribute.
	Probably should use a pointer yet to keep persistence easy.  
 */
typedef	resolved_attribute		footprint_rule_attribute;

//=============================================================================
/**
	Compact and resolved representation of production rule.  
	Implementation is defined in "Object/lang/PRS_footprint.cc".
 */
struct footprint_rule : public cflat_visitee {
	typedef	resolved_attribute_list_type	attributes_list_type;
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
	/**
		List of resolved attributes.
	 */
	attributes_list_type		attributes;

	footprint_rule() { }

	footprint_rule(const int e, const int o, const bool d) :
		expr_index(e), output_index(o), dir(d) { }

	void
	push_back(const footprint_rule_attribute&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

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

};	// end struct footprint_rule

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_FOOTPRINT_RULE_H__
