/**
	\file "Object/inst/alias_empty.hh"
	Implementation of alias info that has no actual parameters.  
	This file originated from "Object/art_object_instance_alias_empty.h"
		in a previous life.  
	$Id: alias_empty.hh,v 1.17 2010/04/07 00:12:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_EMPTY_HH__
#define	__HAC_OBJECT_INST_ALIAS_EMPTY_HH__

#define	DEBUG_ALIAS_EMPTY		1

#include <iosfwd>
#include "Object/unroll/target_context.hh"
#include "util/memory/pointer_classes_fwd.hh"
#include "util/persistent_fwd.hh"
#include "util/boolean_types.hh"

namespace HAC {
namespace entity {
class const_param_expr_list;
class footprint;
class footprint_frame;
class state_manager;
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
	bool
	matched_actuals(const this_type&) const { return true; }

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
	/**
		no actuals to forward!
		\return false to signal no change
	 */
	bool
	copy_actuals(const this_type&) const { return false; }

	/**
		Thus far, no meta types without alias actuals can have 
		substructure.  
		NOT TRUE ANYMORE! channels and datatypes now cannot
		have relaxed types.  
	 */
	template <class AliasType>
	static
	good_bool
	__initialize_assign_footprint_frame(const AliasType&,
			footprint_frame&);

	static
	good_bool
	synchronize_actuals(const this_type&, const this_type&,
			const target_context&) {
		return good_bool(true);
	}

	static
	void
	finalize_actuals_and_substructure_aliases(const this_type&, 
			const target_context&) {
		// do nothing
	}

	static
	void
	__finalize_find(const this_type&) { }

	static
	bool
	__has_complete_type(const this_type&) { return true; }

	static
	void
	__assert_complete_type(const this_type&) { }

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
	create_dependent_types(const AliasType&, const footprint&);

	// called by footprint_frame::dump_footprint.
	template <class AliasType>
	static
	ostream&
	dump_complete_type(const AliasType&, ostream&, const footprint* const);

	/**
		Since type has no relaxed actuals, 
		just return the canonical type of the collection. 
	 */
	template <class InstColl>
	typename InstColl::instance_collection_parameter_type
	complete_type_actuals(const InstColl& _inst) const {
		return _inst.get_canonical_collection().__get_raw_type();
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_EMPTY_HH__

