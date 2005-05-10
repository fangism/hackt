/**
	\file "util/multikey_assoc_fwd.h"
	Forward declarations for multidimensional associatice containers.  
	$Id: multikey_assoc_fwd.h,v 1.3 2005/05/10 04:51:27 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_FWD_H__
#define	__UTIL_MULTIKEY_ASSOC_FWD_H__


#define MULTIKEY_ASSOC_TEMPLATE_SIGNATURE				\
template <size_t D, class C>

#define SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE			\
template <class C>

namespace util {
//=============================================================================

template <size_t D, class C>
class multikey_assoc;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_ASSOC_FWD_H__

