/**
	\file "throw_test.cc"
	Demonstration that throwing exception will result in proper clean
	up of objects on the stack.  
	$Id: throw_test.cc,v 1.2 2005/01/28 19:59:00 fang Exp $
 */

#include <exception>
#include <list>
#include "pooled_thing.h"
#include "memory/count_ptr.h"

using util::memory::count_ptr;

typedef	std::list<count_ptr<pooled_thing> >		list_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static
void
add_to_list(list_type& l, const int v) {
	const count_ptr<pooled_thing> temp(new pooled_thing(v));
	l.push_back(temp);
	if (v == 13)
		throw std::exception();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int argc, char* argv[]) {
try {
	list_type the_list;
	int i = 0;
	for ( ; i<20; i++) {
		add_to_list(the_list, i);
	}
}
catch (...) {
	exit(1);		// NOW we may exit safely
//	throw;			// uncaught exception will Abort
				// skipping static destruction
}
}

