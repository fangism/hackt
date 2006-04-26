/**
	\file "Object/unroll/unroll_context_value_resolver.cc"
	Rationale: separate definition to control eager instantiation.  
	$Id: unroll_context_value_resolver.cc,v 1.1.2.1 2006/04/26 19:50:49 fang Exp $
 */

#include "Object/unroll/unroll_context_value_resolver.h"
#if 0
#include <utility>		// for std::pair
#include "Object/unroll/unroll_context.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/def/footprint.h"
#endif

namespace HAC {
namespace entity {

//=============================================================================
// class unroll_context_value_resolver<pint_tag> method definitions
/**
	\return true if lookup was resolved a loop variable.  
 */
unroll_context_value_resolver<pint_tag>::const_return_type
unroll_context_value_resolver<pint_tag>::operator ()
		(const unroll_context& c, const value_collection_type& v,
		value_type& i) const {
	// hack: intercept loop variable lookups
	const value_scalar_type* const
		vsp(IS_A(const value_scalar_type*, &v));
	if (vsp && vsp->is_loop_variable()) {
		// icc: this causes premature instantiation of 
		// value_array<pint_tag, 0>
		const count_ptr<const pint_const>
			pc(c.lookup_loop_var(*vsp));
		if (pc) {
			i = pc->static_constant_value();
			// stupid LWG defect...
			// return std::make_pair(true, v);
			// stupid reference-to-reference...
			// return return_type(true, v);
			return const_return_type(true, &v);
		}
	}
	// end hack
	const footprint* const f(c.get_target_footprint());
	const value_collection_type&
		_vals(f ? IS_A(const value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return const_return_type(false, &_vals);
}

unroll_context_value_resolver<pint_tag>::value_collection_type&
unroll_context_value_resolver<pint_tag>::operator ()
	(const unroll_context& c, value_collection_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return _vals;
}

//-----------------------------------------------------------------------------
// class unroll_context_value_resolver<pbool_tag> method definitions

unroll_context_value_resolver<pbool_tag>::const_return_type
unroll_context_value_resolver<pbool_tag>::operator ()
		(const unroll_context& c, const value_collection_type& v,
		value_type& i) const {
	const footprint* const f(c.get_target_footprint());
	const value_collection_type&
		_vals(f ? IS_A(const value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return const_return_type(false, &_vals);
}

unroll_context_value_resolver<pbool_tag>::value_collection_type&
unroll_context_value_resolver<pbool_tag>::operator ()
	(const unroll_context& c, value_collection_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return _vals;
}

//-----------------------------------------------------------------------------

	/**
		No need to check if variable is for flow control, 
			because those are not declared and referenced.  
		\param i is never used.  
	 */
unroll_context_value_resolver<preal_tag>::const_return_type
unroll_context_value_resolver<preal_tag>::operator ()
		(const unroll_context& c, const value_collection_type& v,
		value_type& i) const {
	const footprint* const f(c.get_target_footprint());
	const value_collection_type&
		_vals(f ? IS_A(const value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return const_return_type(false, &_vals);
}

unroll_context_value_resolver<preal_tag>::value_collection_type&
unroll_context_value_resolver<preal_tag>::operator ()
	(const unroll_context& c, value_collection_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&,
				*(*f)[v.get_name()])
			: v);
	return _vals;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

