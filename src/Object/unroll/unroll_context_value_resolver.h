/**
	\file "Object/unroll/unroll_context_value_resolver.h"
	Specialized operator definitions for value collection
	resolution.  
	NOTE: method definitions moved to .cc file to prevent premature
	instantiation before complete types are available.  
	$Id: unroll_context_value_resolver.h,v 1.8.10.2 2006/09/11 22:31:23 fang Exp $
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
/**
	Specialization of pint lookup.  
	Implementation defined in 
	"Object/unroll/unroll_context_value_resolver.cc".
 */
template <>
class unroll_context_value_resolver<pint_tag> {
public:
	typedef	class_traits<pint_tag>		traits_type;
	typedef	traits_type::value_collection_generic_type
						value_collection_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	traits_type::instance_placeholder_type
						value_placeholder_type;
#else
	typedef	value_collection_type		value_placeholder_type;
#endif
	typedef traits_type::value_array<0>::type
						value_scalar_type;
	typedef	traits_type::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	const_return_type
	operator () (const unroll_context& c, const value_placeholder_type& v,
			value_type& i) const;

#if !USE_INSTANCE_PLACEHOLDERS
	value_collection_type&
	operator () (const unroll_context& c, 
#if USE_INSTANCE_PLACEHOLDERS
			const
#endif
			value_placeholder_type& v) const;
#endif

};	// end class value_resolver

//-----------------------------------------------------------------------------
/**
	Specialization of pbool lookup.  
	Implementation defined in 
	"Object/unroll/unroll_context_value_resolver.cc".
 */
template <>
class unroll_context_value_resolver<pbool_tag> {
public:
	typedef	class_traits<pbool_tag>		traits_type;
	typedef	traits_type::value_collection_generic_type
						value_collection_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	traits_type::instance_placeholder_type
						value_placeholder_type;
#else
	typedef	value_collection_type		value_placeholder_type;
#endif
	typedef	traits_type::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	const_return_type
	operator () (const unroll_context& c, const value_placeholder_type& v,
			value_type& i) const;

#if !USE_INSTANCE_PLACEHOLDERS
	value_collection_type&
	operator () (const unroll_context& c,
#if USE_INSTANCE_PLACEHOLDERS
			const
#endif
			value_placeholder_type& v) const;
#endif

};	// end class value_resolver

//-----------------------------------------------------------------------------
/**
	Specialization of preal lookup.  
	Implementation defined in 
	"Object/unroll/unroll_context_value_resolver.cc".
 */
template <>
class unroll_context_value_resolver<preal_tag> {
public:
	typedef class_traits<preal_tag>		traits_type;
	typedef	traits_type::value_collection_generic_type
						value_collection_type;
#if USE_INSTANCE_PLACEHOLDERS
	typedef	traits_type::instance_placeholder_type
						value_placeholder_type;
#else
	typedef	value_collection_type		value_placeholder_type;
#endif
	typedef	traits_type::value_type
						value_type;
	typedef	pair<bool, const value_collection_type*>
						const_return_type;

	const_return_type
	operator () (const unroll_context& c, const value_placeholder_type& v,
			value_type& i) const;

#if !USE_INSTANCE_PLACEHOLDERS
	value_collection_type&
	operator () (const unroll_context& c,
#if USE_INSTANCE_PLACEHOLDERS
			const
#endif
			value_placeholder_type& v) const;
#endif

};	// end class value_resolver

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_UNROLL_CONTEXT_VALUE_RESOLVER_H__

