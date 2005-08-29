/**
	\file "Object/inst/alias_actuals.h"
	Implementation of alias info that has actual parameters.  
	This file originated from "Object/art_object_instance_alias_actuals.h"
		in a previous life.  
	$Id: alias_actuals.h,v 1.2.8.6 2005/08/29 21:32:03 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_ACTUALS_H__
#define	__OBJECT_INST_ALIAS_ACTUALS_H__

#define	DEBUG_ALIAS_ACTUALS		1

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class const_param_expr_list;
template <class> class instance_alias_info;
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
	mutable alias_actuals_type			actuals;

protected:
	instance_alias_info_actuals() : actuals() { }
	~instance_alias_info_actuals() { }

	/**
		\return false, error if actuals was already set.
	 */
#if DEBUG_ALIAS_ACTUALS
	bool
	attach_actuals(const alias_actuals_type& a) const;
#else
	bool
	attach_actuals(const alias_actuals_type& a) const {
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
	template <class AliasType>
	good_bool
	__compare_and_propagate_actuals(const alias_actuals_type&,
		AliasType&);

#if 0
private:
	static
	good_bool
	symmetric_compare_and_update_actuals(alias_actuals_type& l, 
		alias_actuals_type& r);
#endif

protected:
	template <class AliasType>
	static
	good_bool
	__symmetric_synchronize(AliasType& l, AliasType& r);

public:
	static
	good_bool
	compare_actuals(const alias_actuals_type&, 
		const alias_actuals_type&);

	template <class AliasType>
	static
	good_bool
	create_dependent_types(const AliasType&);

// protected:
	template <class InstColl>
	typename InstColl::instance_collection_parameter_type
	complete_type_actuals(const InstColl& _inst) const;

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
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_H__

