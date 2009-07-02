/**
	\file "Object/unroll/port_scope.h"
	This file contains class definitions for control-flow scopes
	of the HAC language.  
	$Id: port_scope.h,v 1.1 2009/07/02 23:22:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PORT_SCOPE_H__
#define	__HAC_OBJECT_UNROLL_PORT_SCOPE_H__

#include "Object/unroll/instance_management_base.h"
#include "Object/unroll/sequential_scope.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	The sole purpose of this class is to wrap port instantiations
	into a body, to distinguish them from non-port instantiations, 
	and create hooks for performing actions before and after
	port instantiations.
 */
class port_scope : public instance_management_base, 
		public sequential_scope {
	typedef	port_scope				this_type;
	typedef	instance_management_base		interface_type;
	typedef	sequential_scope			parent_type;
	typedef	sequential_scope			implementation_type;
protected:
	// inherits a list of sequential instance_management items
	// induction variable (inherited)
	// range expression (inherited)
public:
	port_scope();

	~port_scope();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

#if 0
private:
	good_bool
	expand(good_bool (parent_type::*)(const unroll_context&) const) const;
#endif
public:
	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};      // end class port_scope  
     
//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	//	__HAC_OBJECT_UNROLL_PORT_SCOPE_H__

