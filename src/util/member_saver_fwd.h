/**
	\file "util/member_saver_fwd.h"
	$Id: member_saver_fwd.h,v 1.2 2006/04/11 07:54:48 fang Exp $
 */

#ifndef	__UTIL_MEMBER_SAVER_FWD_H__
#define	__UTIL_MEMBER_SAVER_FWD_H__

namespace util {
//=============================================================================
template <class T, class MT, MT T::*Member>
class member_saver;

//=============================================================================
}	// end namepace util

#endif	// __UTIL_MEMBER_SAVER_FWD_H__

