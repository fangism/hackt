/**
	\file "art_object_control.h"
	This file contains class definitions for control-flow scopes
	of the ART++ language.  
	$Id: art_object_control.h,v 1.4 2004/11/02 07:51:47 fang Exp $
 */

#ifndef	__ART_OBJECT_CONTROL_H__
#define	__ART_OBJECT_CONTROL_H__

#include "art_object_base.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Scope of a loop body.
	Notes: Instances in loop bodies will register sparse collections
	in the parent definition scope, but track indices in this scope.
	Q: how should this be kept in the enclosing scope?
		in hash_map or in some ordered list?
	Q: should contents (instantiations) be kept ordered?
	Q: derive from dynamic_scope?
	Q: should not really be a scope, doesn't contain
		it's own used_id_map, should use parent's.
		All lookups and registrations go to enclosing
		definition or namespace scope.
	S: may contain instantiations and connections... references?
	Q: However interface should be like scopespace?
	Q: Should be able to make similar queries?
	S: May have to further refine classification of scopes...
		named_scope, true_scope, psuedo_scope...
 */
class loop_scope : public instance_management_base, 
		public sequential_scope {
protected:
	// inherits a list of sequential instance_management items
	/**
		Should have modifiable pointer to parent scope?
		Parent may be namespace?  NO, because it is a strictly
		sequential item.
		Parent may be definition or some other sequential scope.  
	 */
	never_const_ptr<sequential_scope>		parent;
	// induction variable 
	// range expression
public:
	/** what about name of scope? none. */
	loop_scope(never_const_ptr<sequential_scope> p);
		// more args...  
	~loop_scope();

	// unroll ... may need context of expression values
};      // end class loop_scope  
     
//=============================================================================
/**
	Scope of a conditional body.
	Should this be some list?    
 */
class conditional_scope : public instance_management_base, 
		public sequential_scope {
protected:
	// condition expression
	never_const_ptr<sequential_scope>		parent;
public:
	conditional_scope(const string& n, never_const_ptr<scopespace>);
		// more args...     
	~conditional_scope();

	// unroll
};      // end class conditional_scope

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__ART_OBJECT_CONTROL_H__

