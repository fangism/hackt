/**
	\file "libchpfn/assert.cc"
	Run-time invariant checks.
	TODO: assert version with string for message.
	$Id: assert.cc,v 1.1.2.1 2007/08/12 19:44:04 fang Exp $
 */

#include <iostream>
#include <stdexcept>
#include "libchpfn/assert.h"
#include "Object/expr/types.h"
// #include "util/macros.h"

namespace HAC {
namespace CHP {
using std::cerr;
using std::endl;

/**
@texinfo fn/assert.texi
@deffn Function assert z
Run-time invariant check.  If @var{z} is false, throw an exception
and halt the simulator, who is expected to produce some diagnostic message.
@end deffn
@end texinfo
 */
void
assert(const bool_value_type z) {
        if (!z) {
                cerr << "Run-time assertion failed!" << endl;
                // THROW_EXIT;             // just generic std::exception
		throw std::exception();
        }
        // expect caller to catch and give a more useful message of context
}

}	// end namespace CHP
}	// end namespace HAC

