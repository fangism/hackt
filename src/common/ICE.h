/**
	\file "common/ICE.h"
	Macros for internal compiler errors.  
	$Id: ICE.h,v 1.2.2.1 2005/08/18 05:33:34 fang Exp $
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
	ART::__ICE_where(ostr, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	Sign off and abort.  
 */
#define	ICE_EXIT(ostr)							\
	ART::__ICE_exit(ostr)

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

namespace ART {

extern
void
__ICE_where(std::ostream&, const char* fn, const char* file, const size_t ln);

extern
void
__ICE_exit(std::ostream&) __ATTRIBUTE_NORETURN__ ;

}	// end namespace ART

#endif	// __COMMON_ICE_H__

