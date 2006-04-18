/**
	\file "util/multikey_set_fwd.h"
	Forward declarations for multikey set.
	$Id: multikey_set_fwd.h,v 1.6 2006/04/18 18:42:44 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_SET_FWD_H__
#define	__UTIL_MULTIKEY_SET_FWD_H__

#include "util/STL/set_fwd.h"
#include "util/size_t.h"

/**
	\param S a set type, where S::value_type == T
 */
#define	MULTIKEY_SET_TEMPLATE_SIGNATURE					\
template <size_t D, class T, class S>

#define	MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE				\
template <size_t D, class K, class T>

namespace util {

MULTIKEY_SET_TEMPLATE_SIGNATURE
class multikey_set;

MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
class multikey_set_element;

MULTIKEY_SET_ELEMENT_TEMPLATE_SIGNATURE
class multikey_set_element_derived;

/**
	Template typedef for the default set type.  
 */
template <size_t D, class T>
struct default_multikey_set {
	typedef	multikey_set<D, T, typename std::default_set<T>::type>
							type;

	template <size_t D2, class T2>
	struct rebind : public default_multikey_set<D2, T2> { };
};	// end struct default_multikey_set

}	// end namespace util

#endif	// __UTIL_MULTIKEY_SET_FWD_H__

