/**
	\file "Object/unroll/loop_scope.hh"
	This file contains class definitions for control-flow scopes
	of the HAC language.  
	$Id: loop_scope.hh,v 1.9 2008/11/23 17:54:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_LOOP_SCOPE_H__
#define	__HAC_OBJECT_UNROLL_LOOP_SCOPE_H__

#include "Object/unroll/instance_management_base.hh"
#include "Object/unroll/sequential_scope.hh"
#include "Object/unroll/meta_loop_base.hh"

namespace HAC {
namespace entity {
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
	typedef	sequential_scope			implementation_type;
	friend struct meta_loop<this_type>;
	typedef	meta_loop<this_type>			meta_loop_type;
protected:
	// inherits a list of sequential instance_management items
	// induction variable (inherited)
	// range expression (inherited)
public:
	loop_scope();

	loop_scope(const ind_var_ptr_type&, const range_ptr_type&);

	~loop_scope();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

private:
	/**
		TODO: re-use loop-expansion code in the form
		of pointer-member function calls.
		TODO: bundle arguments into single argument type.  
	 */
	good_bool
	expand(good_bool (parent_type::*)(const unroll_context&) const) const;
public:
	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};      // end class loop_scope  
     
//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	//	__HAC_OBJECT_UNROLL_LOOP_SCOPE_H__

