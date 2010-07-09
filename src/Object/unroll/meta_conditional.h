/**
	\file "Object/unroll/meta_conditional.h"
	Helper functions for repetitive conditional constructs.  
	All functions are static members of a wrapper class
	for convenient friendship.  
	$Id: meta_conditional.h,v 1.4 2010/07/09 02:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_CONDITIONAL_H__
#define	__HAC_OBJECT_UNROLL_META_CONDITIONAL_H__

#include <iosfwd>
#include "Object/unroll/meta_conditional_base.h"
#include "Object/lang/SPEC_fwd.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using std::ostream;
using std::istream;
using util::persistent_object_manager;
using SPEC::node_pool_type;
using util::good_bool;

/**
	Helper class for repetitive functions.  Be-friend me.  
 */
template <class T>
struct meta_conditional {
	typedef	typename T::clause_list_type		clause_list_type;
	typedef	clause_list_type			sequence_type;
	typedef typename clause_list_type::const_iterator	clause_iterator;
	typedef	meta_conditional_base::const_iterator	guard_iterator;
	typedef typename clause_list_type::value_type	element_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ALL clauses are empty.  
 */
static
bool
empty(const T&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class C>
static
ostream&
dump(const T&, ostream&, const C&,
	ostream& (T::clause_list_type::value_type::*dumper)(ostream&, const C&) const
		= &T::clause_list_type::value_type::dump);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
good_bool
unroll(const T&, const unroll_context&, const char*);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
append_guarded_clause(T&, const meta_conditional_base::guard_ptr_type&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
collect_transient_info(const T&, persistent_object_manager&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
write_object(const T&, 
	const persistent_object_manager&, ostream&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
load_object(T& mc, 
	const persistent_object_manager&, istream&);

};	// end class meta_conditional

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_CONDITIONAL_H__

