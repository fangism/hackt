/**
	\file "Object/inst/alias_empty.h"
	Implementation of alias info that has no actual parameters.  
	This file originated from "Object/art_object_instance_alias_empty.h"
		in a previous life.  
	$Id: alias_empty.h,v 1.3.2.6 2005/09/14 00:17:10 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_EMPTY_H__
#define	__OBJECT_INST_ALIAS_EMPTY_H__

#define	DEBUG_ALIAS_EMPTY		1

#include <iosfwd>
#include "util/memory/pointer_classes_fwd.h"
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class const_param_expr_list;
class footprint;
class footprint_frame;
class state_manager;
class port_member_context;
template <class> class instance_alias_info;
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
	typedef	instance_alias_info_empty		this_type;
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

protected:
	/// No-op
	template <class AliasType>
	good_bool
	__compare_and_propagate_actuals(const alias_actuals_type&, 
			const AliasType&) const {
		return good_bool(true);
	}

	template <class AliasType>
	static
	good_bool
	__symmetric_synchronize(const AliasType&, const AliasType&) {
		return good_bool(true);
	}

	/**
		Thus far, no meta types without alias actuals can have 
		substructure.  
	 */
	template <class AliasType>
	static
	good_bool
	__initialize_assign_footprint_frame(const AliasType&,
			const footprint_frame&, const state_manager&,
			const port_member_context&, const size_t) {
		// no-op.
		return good_bool(true);
	}

public:
	static
	good_bool
	compare_actuals(const alias_actuals_type&,
		const alias_actuals_type&) {
		return good_bool(true);
	}

	template <class AliasType>
	static
	good_bool
	create_dependent_types(const AliasType&);

	// called by footprint_frame::dump_footprint.
	template <class AliasType>
	static
	ostream&
	dump_complete_type(const AliasType&, ostream&, const footprint* const);

protected:
	/**
		Since type has no relaxed actuals, 
		just return the canonical type of the collection. 
	 */
	template <class InstColl>
	typename InstColl::instance_collection_parameter_type
	complete_type_actuals(const InstColl& _inst) const {
		return _inst.get_canonical_type();
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

#endif	// __OBJECT_INST_ALIAS_EMPTY_H__

