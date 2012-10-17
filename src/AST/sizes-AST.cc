/**
	\file "AST/sizes-AST.cc"
	Just dumps the sizeof for most HAC::entity classes.
	$Id: sizes-AST.cc,v 1.1 2007/02/27 02:27:52 fang Exp $
 */

#include <iostream>
// checking sizes of these structures as well
#include <vector>
#include <list>

#include "common/sizes-common.hh"
#include "util/what.tcc"	// use default typeinfo-based mangled names
#include "AST/sizes-AST.hh"

// include all AST/*.hh header files to evaluate struct sizes.
#include "AST/parse_context.hh"

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

