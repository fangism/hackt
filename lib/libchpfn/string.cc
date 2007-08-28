/**
	\file "libchpfn/string.cc"
	$Id: string.cc,v 1.2 2007/08/28 04:53:50 fang Exp $
 */

#include <iostream>
#include <sstream>
#include "libchpfn/string.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dlfunction.h"
#include "util/memory/count_ptr.h"
#include "util/string.tcc"		// for string_to_num

namespace HAC {
namespace CHP {
using std::istringstream;
using std::ostringstream;
using std::cerr;
using std::endl;

//=============================================================================
/***
@texinfo fn/strcat.texi
@deffn Fucntion strcat args...
@deffnx Fucntion sprint args...
@deffnx Fucntion tostring args...
Stringify all @var{args} and concatenate into a single string (returned).  
This can be used to convert argument types to a string.  
Does not include terminating newline.  
@end deffn
@end texinfo
***/
string_value_type
strcat(const chp_function_argument_list_type& args) {
	ostringstream oss;
	args.dump_raw(oss);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename T>
static
T
strto_(const char* fn, const string_value_type& str) {
	T ret;
	NEVER_NULL(fn);
	if (util::strings::string_to_num(str, ret)) {
		cerr << "Error in conversion: "
			<< fn << "(\"" << str << "\")" << endl;
		THROW_EXIT;
	}
	return ret;
}

/***
@texinfo fn/strtoz.texi
@deffn Function strtoz str
Convert string @var{str} to an integer.  
Throws run-time exception if conversion fails.  
@end deffn
@end texinfo
***/
int_value_type
strtoz(const string_value_type& str) {
	return strto_<int_value_type>("strtoz", str);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/strtob.texi
@deffn Function strtob str
Convert string @var{str} (0 or 1) to a boolean.  
Throws run-time exception if conversion fails.  
@end deffn
@end texinfo
***/
bool_value_type
strtob(const string_value_type& str) {
	return strto_<bool_value_type>("strtob", str);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo fn/strtor.texi
@deffn Function strtor str
Convert string @var{str} to real (floating-point) value.
Throws run-time exception if conversion fails.  
@end deffn
@end texinfo
***/
real_value_type
strtor(const string_value_type& str) {
	return strto_<real_value_type>("strtor", str);
}

//=============================================================================
}	// end namespace CHP
}	// end namespace HAC

