/**
	\file "util/member_saver.h"
	Useful class for temporarily saving away a modifying a member
	value for the duration of a scope.  
	See also "util/value_saver.h".
	$Id: member_saver.h,v 1.3 2006/04/23 07:37:28 fang Exp $
 */

#ifndef	__UTIL_MEMBER_SAVER_H__
#define	__UTIL_MEMBER_SAVER_H__

#include "util/member_saver_fwd.h"
#include "util/value_saver.h"

namespace util {
//=============================================================================
/**
	Helper struct to save away a member value for the duration
	of a scope and restore the former value upon end-of-scope.  
	Useful for auto-balancing in a stack-like manner.  
	Bonus: self-balancing behavior is exception safe.  
 */
template <class T, class MT, MT T::*Member>
class member_saver : public value_saver<MT> {
public:
	typedef	value_saver<MT>	parent_type;
	typedef	T		object_type;
	typedef	MT		member_type;
public:
	explicit
	member_saver(T& t) :
		parent_type(t.*Member) { }

	explicit
	member_saver(T& t, const member_type& m) : parent_type(t.*Member, m) { }

	~member_saver() { }

private:
	// undefined copy-constructing
	explicit
	member_saver(const member_saver&);

} __ATTRIBUTE_UNUSED__ ;	// end class member_saver

//=============================================================================
}	// end namepace util

#endif	// __UTIL_MEMBER_SAVER_H__

