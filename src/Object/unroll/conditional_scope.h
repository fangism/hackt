/**
	\file "Object/unroll/conditional_scope.h"
	This file contains class definitions for control-flow scopes
	of the HAC language.  
	$Id: conditional_scope.h,v 1.7 2006/03/15 04:38:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_H__
#define	__HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_H__

#include "Object/unroll/instance_management_base.h"
#include "Object/unroll/sequential_scope.h"
#include "Object/unroll/meta_conditional_base.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Scope of a conditional body.
	Boolean guard expression is evaluated at unroll-time (and later)
	and conditionally expanded.  
 */
class conditional_scope : public instance_management_base, 
		public sequential_scope, 
		protected meta_conditional_base {
	typedef	conditional_scope			this_type;
	typedef	instance_management_base		interface_type;
	typedef	sequential_scope			parent_type;
	// inherits a list of sequential instance_management items
	// boolean meta-expression (inherited)
public:
	conditional_scope();

	explicit
	conditional_scope(const guard_ptr_type&);

	~conditional_scope();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);


};      // end class conditional_scope

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	//	__HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_H__

