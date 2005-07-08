/**
	\file "Object/art_object_instance_alias_actuals.h"
	Implementation of alias info that has actual parameters.  
	$Id: art_object_instance_alias_actuals.h,v 1.1.2.1 2005/07/08 18:15:27 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_ACTUALS_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_ALIAS_ACTUALS_H__

#include <iosfwd>
#include "util/memory/count_ptr.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_fwd.h"

namespace ART {
namespace entity {
class const_param_expr_list;
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

public:
	ostream&
	dump_actuals(ostream& o) const;

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

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ALIAS_ACTUALS_H__

