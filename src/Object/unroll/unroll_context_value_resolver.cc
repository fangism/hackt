/**
	\file "Object/unroll/unroll_context_value_resolver.cc"
	Rationale: separate definition to control eager instantiation.  
	$Id: unroll_context_value_resolver.cc,v 1.9 2006/10/19 03:30:27 fang Exp $
 */

#include "Object/unroll/unroll_context_value_resolver.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/inst/value_placeholder.h"

namespace HAC {
namespace entity {

//=============================================================================
// class unroll_context_value_resolver<pint_tag> method definitions

/**
	TODO: these policy/implementation classes may be obsolete
		after unifying the lookup mechanism.
	TODO: use a consistent scoped lookup function (up footprints)
		to avoid this hackery, now that we have proper placeholders.  
	2006-09-01
	TODO: consider when lookup is scoped and not a simple name!
		Trace the placehlder's roots for proper name binding.  

	Looks up a value for reading and resolving.  
	\return true if lookup was resolved a loop variable.  
 */
unroll_context_value_resolver<pint_tag>::const_return_type
unroll_context_value_resolver<pint_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	// lookup is same for all meta variable types (consistency!)
	// including loop induction variables
	const count_ptr<const value_collection_type>
		val_p(c.lookup_rvalue_collection(v).
			is_a<const value_collection_type>());
	const value_collection_type* _vals(&*val_p);
	return const_return_type(false, _vals);
}

//-----------------------------------------------------------------------------
// class unroll_context_value_resolver<pbool_tag> method definitions

unroll_context_value_resolver<pbool_tag>::const_return_type
unroll_context_value_resolver<pbool_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	// this will lookup globals already...
	const count_ptr<const value_collection_type>
		val_p(c.lookup_rvalue_collection(v).
			is_a<const value_collection_type>());
	const value_collection_type* _vals(&*val_p);
	return const_return_type(false, _vals);
}

//-----------------------------------------------------------------------------
/**
	No need to check if variable is for flow control, 
		because those are not declared and referenced.  
	\param i is never used.  
 */
unroll_context_value_resolver<preal_tag>::const_return_type
unroll_context_value_resolver<preal_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	// this will lookup globals already...
	const count_ptr<const value_collection_type>
		val_p(c.lookup_rvalue_collection(v).
			is_a<const value_collection_type>());
	const value_collection_type* _vals(&*val_p);
	return const_return_type(false, _vals);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

