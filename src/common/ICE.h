/**
	\file "common/ICE.h"
	Macros for internal compiler errors.  
	$Id: ICE.h,v 1.3.20.1 2005/12/11 00:45:59 fang Exp $
 */

#ifndef	__COMMON_ICE_H__
#define	__COMMON_ICE_H__

#include <iosfwd>
#include "util/macros.h"
#include "util/attributes.h"

/**
	Standard greeting for internal compiler errors.  
	\param ostr the output stream, like cerr.
 */
#define	ICE_GREET(ostr)							\
	HAC::__ICE_where(ostr, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	Sign off and abort.  
 */
#define	ICE_EXIT(ostr)							\
	HAC::__ICE_exit(ostr)

/**
	Standard internal compiler error macro.  
	USE THIS MACRO.
	\param ostr the output stream, like cerr.
	\param diagnostic something to execute to print more information, 
		is optional.  
 */
#define	ICE(ostr, diagnostic)						\
	ICE_GREET(ostr);						\
	{ diagnostic }							\
	ICE_EXIT(ostr);

#define	ICE_NEVER_CALL(ostr)						\
	ICE_GREET(ostr);						\
	ostr << ICE_never_call << endl;					\
	ICE_EXIT(ostr)

namespace HAC {

extern
void
__ICE_where(std::ostream&, const char* fn, const char* file, const size_t ln);

extern
void
__ICE_exit(std::ostream&) __ATTRIBUTE_NORETURN__ ;

extern
const char ICE_never_call[];

}	// end namespace HAC

#endif	// __COMMON_ICE_H__

