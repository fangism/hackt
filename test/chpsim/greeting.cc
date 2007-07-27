// "greeting.cc"
// testing void function

#include <iostream>
#include <sim/chpsim/chpsim_dlfunction.h>

using std::cout;
using std::endl;
USING_CHPSIM_DLFUNCTION_PROLOGUE

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
hello(void) {
	cout << "Hello, world!" << endl;
}

CHP_DLFUNCTION_LOAD_DEFAULT("hello", hello)

