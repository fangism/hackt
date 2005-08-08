/**
	\file "common/ICE.h"
	Macros for internal compiler errors.  
	$Id: ICE.h,v 1.2 2005/08/08 23:08:31 fang Exp $
 */

#ifndef	__COMMON_ICE_H__
#define	__COMMON_ICE_H__

#include "util/macros.h"

/**
	Internal auxiliary macro for stringifcation, don't use this directly.  
 */
#define	ICE_WHERE(ostr, fn, file, line)					\
	ostr << ART::ICE_greeting <<					\
		#fn " at " #file ":" #line ": " << std::endl

/**
	Standard greeting for internal compiler errors.  
	\param ostr the output stream, like cerr.
 */
#define	ICE_GREET(ostr)							\
	ICE_WHERE(ostr, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	Print bug report banner.
	Throws an exception to exit.  
	\param ostr the output stream, like cerr.
 */
#define	ICE_EXIT(ostr)							\
	ostr << ART::ICE_footer << std::endl;				\
	THROW_EXIT;

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

extern const char ICE_greeting[];
extern const char ICE_footer[];

}	// end namespace ART

#endif	// __COMMON_ICE_H__

