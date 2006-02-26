/**
	\file "throw_test.cc"
	Demonstration that throwing exception will result in proper clean
	up of objects on the stack.  
	$Id: throw_test.cc,v 1.5 2006/02/26 02:28:06 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <exception>
#include <list>
#include "pooled_thing.h"
#include "util/memory/count_ptr.tcc"

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
main(int, char*[]) {
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

