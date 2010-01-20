/**
	\file "Object/inst/alias_actuals.h"
	Implementation of alias info that has actual parameters.  
	This file originated from "Object/art_object_instance_alias_actuals.h"
		in a previous life.  
	$Id: alias_actuals.h,v 1.14.2.3 2010/01/20 02:18:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_ACTUALS_H__
#define	__HAC_OBJECT_INST_ALIAS_ACTUALS_H__

#define	DEBUG_ALIAS_ACTUALS		1

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/devel_switches.h"	// for MEMORY_MAPPED_GLOBAL_ALLOCATION
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class const_param_expr_list;
class footprint;
class footprint_frame;
class port_member_context;
class state_manager;
template <class> class instance_alias_info;
class unroll_context;
using std::istream;
using std::ostream;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Actually contains actuals!  These actuals are relaxed parameter
	values that allow members of the same collection to 
	individualize to a limited extent.  
 */
class instance_alias_info_actuals {
	typedef	instance_alias_info_actuals		this_type;
protected:
	typedef	count_ptr<const const_param_expr_list>	alias_actuals_type;

protected:
	/**
		Making this mutable for convenience.  
	 */
	alias_actuals_type			actuals;

protected:
	instance_alias_info_actuals() : actuals() { }
	~instance_alias_info_actuals() { }

	/**
		\return false, error if actuals was already set.
	 */
#if DEBUG_ALIAS_ACTUALS
	bool
	attach_actuals(const alias_actuals_type& a);
#else
	bool
	attach_actuals(const alias_actuals_type& a) {
		if (actuals)
			return false;
		else {
			NEVER_NULL(a);
			actuals = a;
			return true;
		}
	}
#endif

public:
	const alias_actuals_type&
	get_relaxed_actuals(void) const {
		return actuals;
	}

	ostream&
	dump_actuals(ostream& o) const;

protected:
	/**
		\return true if actuals are new.
	 */
	bool
	copy_actuals(const this_type& t) { 
		// INVARIANT(!actuals);		// ?
		const bool ret = t.actuals && !actuals;
		actuals = t.actuals;
		return ret;
	}

	template <class AliasType>
	static
	good_bool
	__initialize_assign_footprint_frame(const AliasType&, footprint_frame&
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, state_manager&, 
		const port_member_context&
		, const size_t
#endif
		);

	template <class AliasType>
	static
	good_bool
	synchronize_actuals(AliasType&, AliasType&, const unroll_context&);

	template <class AliasType>
	static
	void
	finalize_actuals_and_substructure_aliases(AliasType&, 
			const unroll_context&);

	template <class AliasType>
	static
	void
	__finalize_find(AliasType&, const unroll_context&);

	template <class AliasType>
	static
	bool
	__has_complete_type(const AliasType&);

	template <class AliasType>
	static
	void
	__assert_complete_type(const AliasType&);

public:
	static
	good_bool
	compare_actuals(const alias_actuals_type&, 
		const alias_actuals_type&);

	template <class AliasType>
	static
	good_bool
	create_dependent_types(const AliasType&, const footprint&);

// protected:
	template <class InstColl>
	typename InstColl::instance_collection_parameter_type
	complete_type_actuals(const InstColl& _inst) const;

	// called by footprint_frame::dump_footprint.
	template <class AliasType>
	static
	ostream&
	dump_complete_type(const AliasType&, ostream&, const footprint* const);

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class instance_alias_info_empty_actuals

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_ACTUALS_H__

