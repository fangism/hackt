/**
	\file "Object/unroll/loop_scope.h"
	This file contains class definitions for control-flow scopes
	of the ART++ language.  
	$Id: loop_scope.h,v 1.2.22.1 2005/10/26 22:12:37 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_LOOP_SCOPE_H__
#define	__OBJECT_UNROLL_LOOP_SCOPE_H__

#include "Object/unroll/instance_management_base.h"
#include "Object/unroll/sequential_scope.h"
#include "Object/unroll/meta_loop_base.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class sequential_scope;
using util::memory::never_ptr;

//=============================================================================
/**
	NOTE: these comments have not been touched for most of a year...
	TODO: update comments.  
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
		public sequential_scope, 
		protected meta_loop_base {
	typedef	loop_scope				this_type;
	typedef	instance_management_base		interface_type;
	typedef	sequential_scope			parent_type;
protected:
	// inherits a list of sequential instance_management items
#if 0
	/**
		Should have modifiable pointer to parent scope?
		Parent may be namespace?  NO, because it is a strictly
		sequential item.
		Parent may be definition or some other sequential scope.  
	 */
	never_ptr<const sequential_scope>		parent;
#endif
	// induction variable (inherited)
	// range expression (inherited)
public:
	loop_scope();

	loop_scope(const ind_var_ptr_type&, const range_ptr_type&);
#if 0
	/** what about name of scope? none. */
	explicit
	loop_scope(const never_ptr<const sequential_scope> p);
		// more args...  
#endif

	~loop_scope();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	good_bool
	unroll(const unroll_context&) const;

	CREATE_UNIQUE_PROTO;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};      // end class loop_scope  
     
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__OBJECT_UNROLL_LOOP_SCOPE_H__

