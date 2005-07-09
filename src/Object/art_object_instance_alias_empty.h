/**
	\file "Object/art_object_instance_alias_empty.h"
	Implementation of alias info that has no actual parameters.  
	$Id: art_object_instance_alias_empty.h,v 1.1.2.3 2005/07/09 23:13:16 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_EMPTY_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_EMPTY_H__

#define	DEBUG_ALIAS_EMPTY		1

#include <iosfwd>
#include "util/memory/pointer_classes_fwd.h"
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::good_bool;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Many types do not have relaxed actuals, so this empty class
	is appropriate for their collection instance aliases.  
	This implementation is important because it enables the empty
	base class optimization for one of the largest hogs of memory
	in the unrolled object.  
 */
class instance_alias_info_empty {
protected:
	typedef	count_ptr<const const_param_expr_list>	alias_actuals_type;

public:
	static const alias_actuals_type			null;
protected:
#if DEBUG_ALIAS_EMPTY
	bool
	attach_actuals(const alias_actuals_type&) const;
#else
	/**
		No-op.  
		Need to fake const-ness to make child-class happy
		\return true (success).
	 */
	bool
	attach_actuals(const alias_actuals_type&) const {
		// no-op!
		return true;
	}
#endif

public:
	/**
		Return NULL pointer to the actuals.  
		Defined in "Object/art_object_instance_alias_actuals.cc".
	 */
	const alias_actuals_type&
	get_relaxed_actuals(void) const { return null; }

	/**
		Nothing to print!
	 */
	ostream&
	dump_actuals(ostream& o) const {
		return o;
	}

	/**
		Pray that compiler is smart enough to inline
		this and optimize away if-true.  
	 */
	static
	good_bool
	compare_and_update_actuals(const alias_actuals_type&,
		const alias_actuals_type&) {
		return good_bool(true);
	}

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const {
	}

	void
	write_object_base(const persistent_object_manager&, ostream&) const {
	}

	void
	load_object_base(const persistent_object_manager&, istream&) {
	}

};	// end class instance_alias_info_empty

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ALIAS_EMPTY_H__

