// "count_ptr_fwd.h"
// reference counting pointer class, forward declarations only
// see full decsriptions in "count_ptr.h"

#ifndef	__COUNT_PTR_FWD_H__
#define	__COUNT_PTR_FWD_H__

#ifndef COUNT_PTR_NAMESPACE
#define COUNT_PTR_NAMESPACE		fang
#endif

namespace COUNT_PTR_NAMESPACE {
//=============================================================================
// forward declarations
class abstract_base_count_ptr;
template <class>	class count_ptr;
template <class>	class count_const_ptr;

//=============================================================================
}	// end namespace COUNT_PTR_NAMESPACE

#endif	//	__COUNT_PTR_FWD_H__

