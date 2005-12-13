/**
	\file "common/ICE.cc"
	String and common symbols defined for internal compiler errors.
	$Id: ICE.cc,v 1.4 2005/12/13 04:15:44 fang Exp $
 */

#include <ostream>
#include "common/ICE.h"
#include "config.h"
#include "util/attributes.h"

namespace HAC {
using std::ostream;
using std::endl;

//=============================================================================
const char ICE_never_call[] = ".. is never supposed to be called.";

//-----------------------------------------------------------------------------
static const char ICE_greeting[] = "Internal compiler error: in ";
static const char ICE_footer[] =
"*** Please submit a bug report including version (" PACKAGE_STRING "),\n"
"*** test case with steps to reproduce, and configuration, if appropriate,\n"
"*** to <" PACKAGE_BUGREPORT ">.  Thank you, and have a nice day.";

//=============================================================================
void
__ICE_where(ostream& o, const char* fn, const char* file, const size_t line) {
	o << ICE_greeting << fn << " at " << file << ':' << line
		<< ": " << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
__ICE_exit(ostream& o) {
	o << ICE_footer << endl;
	THROW_EXIT;
}

//=============================================================================
static
void
__ICE_macro_test(ostream& o) __ATTRIBUTE_UNUSED__;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just to make sure the ICE macros compile.  
 */
static
void
__ICE_macro_test(ostream& o) {
	ICE(o,
		o << "Good-bye!" << endl;
	);
}

//=============================================================================
}	// end namespace HAC

