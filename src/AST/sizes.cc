/**
	\file "AST/sizes.cc"
	Just dumps the sizeof for most HAC::entity classes.
	$Id: sizes.cc,v 1.1 2006/08/23 20:57:14 fang Exp $
 */

#include <iostream>
// checking sizes of these structures as well
#include <vector>
#include <list>

#include "common/sizes.h"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "AST/sizes.h"

// include all AST/*.h header files to evaluate struct sizes.
#include "AST/parse_context.h"

namespace HAC {
namespace parser {
using std::ostream;
using std::cerr;
using std::endl;

//=============================================================================
/**
	Diagnostic for inspecting sizes of classes.  
	Has nothing to do with class persistence. 
 */
ostream&
dump_class_sizes(ostream& o) {
	o << "HAC::AST classes:" << endl;
	__dump_class_size<parser::context>(o);

	return o;
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

