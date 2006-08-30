/**
	\file "sim/ISE.h"
	Macros for internal simulator errors.  
	$Id: ISE.h,v 1.3 2006/08/30 04:05:06 fang Exp $
 */

#ifndef	__SIM_ISE_H__
#define	__SIM_ISE_H__

#include <iosfwd>
#include "util/macros.h"
#include "util/attributes.h"

/**
	Standard greeting for internal compiler errors.  
	\param ostr the output stream, like cerr.
 */
#define	ISE_GREET(ostr)							\
	HAC::SIM::__ISE_where(ostr, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	Sign off and abort.  
 */
#define	ISE_EXIT(ostr)							\
	HAC::SIM::__ISE_exit(ostr)

/**
	Standard internal compiler error macro.  
	USE THIS MACRO.
	\param ostr the output stream, like cerr.
	\param diagnostic something to execute to print more information, 
		is optional.  
 */
#define	ISE(ostr, diagnostic)						\
	ISE_GREET(ostr);						\
	{ diagnostic }							\
	ISE_EXIT(ostr);

/**
        More verbose assertion failures.  
 */
#ifdef  DISABLE_INVARIANT
#define ISE_INVARIANT(pred)
#else
#define ISE_INVARIANT(pred)						\
	if (UNLIKELY(!(pred))) {					\
		ISE_GREET(std::cerr);					\
		std::cerr << "assert failed: `" # pred "\'"		\
			<< std::endl;					\
		ISE_EXIT(std::cerr);					\
	}
#endif

namespace HAC {
namespace SIM {

extern
void
__ISE_where(std::ostream&, const char* fn, const char* file, const size_t ln);

extern
void
__ISE_exit(std::ostream&) __ATTRIBUTE_NORETURN__ ;

}	// end namespace SIM
}	// end namespace HAC

#endif	// __SIM_ISE_H__

