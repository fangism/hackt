/**
	\file "util/visitee.h"
	$Id: visitee.h,v 1.2 2006/01/22 06:53:38 fang Exp $
 */

#ifndef	__UTIL_VISITEE_H__
#define	__UTIL_VISITEE_H__

namespace util {
//=============================================================================
/**
	Foundation visitable attribute for Visitor pattern.  
	\param Visitor the base visitor type to accept.  
	\param Return the return type of the accept() method.  
 */
template <class Visitor, typename Return = void>
struct visitee {
	typedef	Visitor		visitor_type;
	typedef	Return		return_type;

virtual	~visitee() { }

virtual	return_type
	accept(visitor_type&) = 0;

};	// end struct visitee

//-----------------------------------------------------------------------------
/**
	A visitable base class that only permits non-modifying visits.  
	Useful in immutable hierarchies.  
 */
template <class Visitor, typename Return = void>
struct visitee_const {
	typedef	Visitor		visitor_type;
	typedef	Return		return_type;

virtual	~visitee_const() { }

virtual	return_type
	accept(visitor_type&) const = 0;
};	// end struct visitee_const

//=============================================================================
}	// end namespace util

#endif	// __UTIL_VISITEE_H__

