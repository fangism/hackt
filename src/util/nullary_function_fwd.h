/**
	\file "util/nullary_function_fwd.h"
	Generic generator functor base.
	$Id: nullary_function_fwd.h,v 1.3 2005/05/10 04:51:28 fang Exp $
 */

#ifndef	__UTIL_NULLARY_FUNCTION_FWD_H__
#define	__UTIL_NULLARY_FUNCTION_FWD_H__

namespace util {
//=============================================================================

struct nullary_function_virtual_parent_tag;
struct nullary_function_nonvirtual_parent_tag;

template <class>
struct nullary_function;

template <class>
struct nullary_function_virtual;

template <class, class>
struct nullary_function_parent_select;

// forward-declare specializations
template <class R>
struct nullary_function_parent_select<R, nullary_function_virtual_parent_tag>;
template <class R>
struct nullary_function_parent_select<R, nullary_function_nonvirtual_parent_tag>;

template <class, class Tag = nullary_function_nonvirtual_parent_tag>
class pointer_to_nullary_function;

template <class, class Tag = nullary_function_nonvirtual_parent_tag>
class binder_null;

#if 0
template <class Op, class T>
inline
binder_null<Op>
bind_null(const Op&, const T&);
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_NULLARY_FUNCTION_FWD_H__

