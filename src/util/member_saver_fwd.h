/**
	\file "util/member_saver_fwd.h"
	$Id: member_saver_fwd.h,v 1.1.2.1 2006/04/09 04:34:04 fang Exp $
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

