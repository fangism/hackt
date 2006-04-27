/**
	\file "Object/unroll/unroll_context_value_resolver.h"
	Specialized operator definitions for value collection
	resolution.  
	NOTE: method definitions moved to .cc file to prevent premature
	instantiation before complete types are available.  
	$Id: unroll_context_value_resolver.h,v 1.7 2006/04/27 05:51:51 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__
#define	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__

#include <utility>		// for std::pair
#include "Object/unroll/unroll_context.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_collection.h"
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
			value_type& i) const;

	value_collection_type&
	operator () (const unroll_context& c, value_collection_type& v) const;

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
		No need to check if variable is for flow control, 
			because those are not declared and referenced.  
		\param i is never used.  
	 */
	const_return_type
	operator () (const unroll_context& c, const value_collection_type& v,
			value_type& i) const;

	value_collection_type&
	operator () (const unroll_context& c, value_collection_type& v) const;
};	// end class value_resolver

//-----------------------------------------------------------------------------
template <>
class unroll_context_value_resolver<preal_tag> {
public:
	typedef	class_traits<preal_tag>::value_collection_generic_type
						value_collection_type;
	typedef	class_traits<preal_tag>::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	/**
		No need to check if variable is for flow control, 
			because those are not declared and referenced.  
		\param i is never used.  
	 */
	const_return_type
	operator () (const unroll_context& c, const value_collection_type& v,
			value_type& i) const;

	value_collection_type&
	operator () (const unroll_context& c, value_collection_type& v) const;

};	// end class value_resolver

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__

