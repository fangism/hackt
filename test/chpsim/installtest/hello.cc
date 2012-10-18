#include <iostream>
#include <sim/chpsim/chpsim_dlfunction.hh>

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
int_value_type
plus(const int_value_type a, const int_value_type b) {
	return a + b;
}

CHP_DLFUNCTION_LOAD_DEFAULT("plus", plus)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


