// "ptrs_fwd.h"
// forward declarations for "ptrs.h"

#ifndef	__PTRS_FWD_H__
#define	__PTRS_FWD_H__

// overrideable namespace
#ifndef	PTRS_NAMESPACE
#define	PTRS_NAMESPACE			fang
#endif

namespace PTRS_NAMESPACE {

template <class>	class some_ptr;		// sometimes owner
template <class>	class some_const_ptr;	// sometimes owner
template <class>	class excl_ptr;		// exclusive owner and deleter
template <class>	class excl_const_ptr;	// exclusive owner and deleter
template <class>	class never_ptr;	// never owner
template <class>	class never_const_ptr;	// never owner

}	// end namespace PTRS_NAMESPACE

#endif	//	__PTRS_FWD_H__

