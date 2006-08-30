/**
	\file "sim/ISE.cc"
	String and common symbols defined for internal simulator errors.
	$Id: ISE.cc,v 1.3 2006/08/30 04:05:05 fang Exp $
 */

#include <ostream>
#include "sim/ISE.h"
#include "cvstag.h"	// generated

namespace HAC {
namespace SIM {
using std::ostream;
using std::endl;

//=============================================================================
static const char ISE_greeting[] = "Internal simulator error: in ";
static const char ISE_footer[] =
"*** Please submit a bug report including version \n"
"*** \"" PACKAGE_STRING ", " CVSTAG "\",\n"
"*** (preferably reduced) test case, steps to reproduce, and configuration,\n"
"*** if appropriate, to <" PACKAGE_BUGREPORT ">.\n"
"*** This program will now self-destruct.  Thank you, and have a nice day.";

//=============================================================================
void
__ISE_where(ostream& o, const char* fn, const char* file, const size_t line) {
	o << ISE_greeting << fn << " at " << file << ':' << line
		<< ": " << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
__ISE_exit(ostream& o) {
	o << ISE_footer << endl;
	THROW_EXIT;
}

//=============================================================================
static
void
__ISE_macro_test(ostream& o) __ATTRIBUTE_UNUSED__;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just to make sure the ISE macros compile.  
 */
static
void
__ISE_macro_test(ostream& o) {
	ISE(o,
		o << "Good-bye!" << endl;
	);
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

