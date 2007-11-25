/**
	\file "Object/unroll/conditional_scope.h"
	This file contains class definitions for control-flow scopes
	of the HAC language.  
	$Id: conditional_scope.h,v 1.7.98.2 2007/11/25 10:02:53 fang Exp $
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
	TODO: this *could* be templated to make the parent type
		and base type parameters... might save duplicate code.
 */
class conditional_scope : public instance_management_base, 
		protected meta_conditional_base {
	typedef	conditional_scope			this_type;
	typedef	instance_management_base		interface_type;
	typedef	sequential_scope			parent_type;
	// inherits a list of sequential instance_management items
	// boolean meta-expression (inherited)
	typedef	std::vector<sequential_scope>		clause_list_type;
	clause_list_type				clauses;
public:
	conditional_scope();

	~conditional_scope();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	bool
	empty(void) const;

	void
	append_guarded_clause(const guard_ptr_type&);

	sequential_scope&
	get_last_clause(void) { return clauses.back(); }

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

