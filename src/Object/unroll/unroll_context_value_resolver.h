/**
	\file "Object/unroll/unroll_context_value_resolver.h"
	Specialized operator definitions for value collection
	resolution.  
	$Id: unroll_context_value_resolver.h,v 1.2.12.1 2005/12/11 00:45:59 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__
#define	__OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__

#include <utility>		// for std::pair
#include "Object/unroll/unroll_context.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/def/footprint.h"

namespace HAC {
namespace entity {
using std::pair;

//=============================================================================
template <>
class unroll_context_value_resolver<pint_tag> {
public:
	typedef	class_traits<pint_tag>::value_collection_generic_type
						value_collection_type;
	typedef class_traits<pint_tag>::value_array<0>::type
						value_scalar_type;
	typedef	class_traits<pint_tag>::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	/**
		\return true if lookup was resolved a loop variable.  
	 */
	const_return_type
	operator () (const unroll_context& c, const value_collection_type& v,
			value_type& i) const {
		// hack: intercept loop variable lookups
		const value_scalar_type* const
			vsp(IS_A(const value_scalar_type*, &v));
		if (vsp && vsp->is_loop_variable()) {
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

	value_collection_type&
	operator () (const unroll_context& c, value_collection_type& v) const {
		// no specialization, can't assign to loop vars.  
		const footprint* const f(c.get_target_footprint());
		value_collection_type&
			_vals(f ? IS_A(value_collection_type&,
					*(*f)[v.get_name()])
				: v);
		return _vals;
	}

};	// end class value_resolver

//-----------------------------------------------------------------------------
template <>
class unroll_context_value_resolver<pbool_tag> {
public:
	typedef	class_traits<pbool_tag>::value_collection_generic_type
						value_collection_type;
	typedef	class_traits<pbool_tag>::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	/**
		\param i is never used.  
	 */
	const_return_type
	operator () (const unroll_context& c, const value_collection_type& v,
			value_type& i) const {
#if 0
		// check for flow control variables?  probably not necessary.
#endif
		const footprint* const f(c.get_target_footprint());
		const value_collection_type&
			_vals(f ? IS_A(const value_collection_type&,
					*(*f)[v.get_name()])
				: v);
		return const_return_type(false, &_vals);
	}

	value_collection_type&
	operator () (const unroll_context& c, value_collection_type& v) const {
		// no specialization, can't assign to loop vars.  
		const footprint* const f(c.get_target_footprint());
		value_collection_type&
			_vals(f ? IS_A(value_collection_type&,
					*(*f)[v.get_name()])
				: v);
		return _vals;
	}

};	// end class value_resolver

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__

