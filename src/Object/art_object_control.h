/**
	\file "Object/art_object_control.h"
	This file contains class definitions for control-flow scopes
	of the ART++ language.  
	$Id: art_object_control.h,v 1.10 2005/06/19 01:58:35 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_CONTROL_H__
#define	__OBJECT_ART_OBJECT_CONTROL_H__

#include "util/string_fwd.h"
#include "Object/art_object_instance_management_base.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class sequential_scope;
class scopespace;
using std::string;
using util::memory::never_ptr;

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
	never_ptr<const sequential_scope>		parent;
	// induction variable 
	// range expression
public:
	/** what about name of scope? none. */
	explicit
	loop_scope(const never_ptr<const sequential_scope> p);
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
	never_ptr<const sequential_scope>		parent;
public:
	conditional_scope(const string& n, const never_ptr<const scopespace>);
		// more args...     
	~conditional_scope();

	// unroll
};      // end class conditional_scope

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__OBJECT_ART_OBJECT_CONTROL_H__

