/**
	\file "util/member_saver.h"
	$Id: member_saver.h,v 1.1.2.1 2006/04/07 22:54:38 fang Exp $
 */

#ifndef	__UTIL_MEMBER_SAVER_H__
#define	__UTIL_MEMBER_SAVER_H__

#include "util/attributes.h"

namespace util {
//=============================================================================
/**
	Helper struct to save away a member value for the duration
	of a scope and restore the former value upon end-of-scope.  
	Useful for auto-balancing in a stack-like manner.  
 */
template <class T, class MT, MT T::*Member>
class member_saver {
public:
	typedef	T		object_type;
	typedef	MT		member_type;
private:
	object_type&		object;
	const member_type	saved_member;
public:
	explicit
	member_saver(T& t) : object(t), saved_member(t.*Member) { }

	explicit
	member_saver(T& t, const member_type& m) :
		object(t), saved_member(object.*Member) {
		object.*Member = m;
	}

	~member_saver() { object.*Member = saved_member; }

private:
	// undefined copy-constructing
	explicit
	member_saver(const member_saver&);

} __ATTRIBUTE_UNUSED__ ;	// end class member_saver

//=============================================================================
}	// end namepace util

#endif	// __UTIL_MEMBER_SAVER_H__

