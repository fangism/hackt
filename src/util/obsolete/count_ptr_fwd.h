/**
	\file "count_ptr_fwd.h"
	Reference counting pointer class, forward declarations only. 
	See full decsriptions in "count_ptr.h".  
	$Id: count_ptr_fwd.h,v 1.1 2004/11/30 01:26:23 fang Exp $
 */

#ifndef	__COUNT_PTR_FWD_H__
#define	__COUNT_PTR_FWD_H__

#ifndef COUNT_PTR_NAMESPACE
#define COUNT_PTR_NAMESPACE		count_ptr_ns
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

