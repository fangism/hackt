/**
	\file "sim/ISE.h"
	Macros for internal simulator errors.  
	$Id: ISE.h,v 1.1.2.1 2006/07/13 02:39:40 fang Exp $
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
	HAC::__ISE_where(ostr, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
	Sign off and abort.  
 */
#define	ISE_EXIT(ostr)							\
	HAC::__ISE_exit(ostr)

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

