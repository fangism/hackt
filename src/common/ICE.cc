/**
	\file "common/ICE.cc"
	String and common symbols defined for internal compiler errors.
	$Id: ICE.cc,v 1.1.2.1 2005/08/08 22:57:16 fang Exp $
 */

#include <ostream>
#include "common/ICE.h"
#include "config.h"

namespace ART {

const char ICE_greeting[] = "Internal compiler error: in ";
const char ICE_footer[] =
	"Please submit a bug report including the version ("
	PACKAGE_STRING "), test case with steps to reproduce, "
	"and configuration, if appropriate, to " PACKAGE_BUGREPORT
	".\nHave a nice day.";

/**
	Just to make sure the ICE macros compile.  
 */
static
void
__ICE_macro_test(std::ostream& o) {
	ICE(o,
		o << "Good-bye!" << std::endl;
	);
}

}	// end namespace ART

