/**
	\file "common/ICE.h"
	Macros for internal compiler errors.  
	$Id: ICE.h,v 1.5 2006/08/25 05:47:46 fang Exp $
 */

#ifndef	__COMMON_ICE_H__
#define	__COMMON_ICE_H__

#include <iosfwd>
#include "util/macros.h"
#include "util/attributes.h"
#include "util/likely.h"

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

/**
	When unreachable code is reached!
 */
#define	ICE_NEVER_CALL(ostr)						\
	ICE_GREET(ostr);						\
	ostr << ICE_never_call << endl;					\
	ICE_EXIT(ostr)

/**
	More verbose assertion failures.  
 */
#ifdef	DISABLE_INVARIANT
#define	ICE_INVARIANT(pred)
#else
#define	ICE_INVARIANT(pred)						\
	if (UNLIKELY(!pred)) {						\
		ICE_GREET(ostr);					\
		std::cerr << "assert failed: `" # pred "\'" 		\
			<< std::endl;					\
		ICE_EXIT(std::cerr)					\
	}
#endif


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

