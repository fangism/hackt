/**
	\file "exit_test.cc"
	Demonstration that calling exit(1) will result in non-fatal
	memory leaks, as reported by the static list_vector_pool.  
	$Id: exit_test.cc,v 1.4 2005/09/04 21:15:10 fang Exp $
 */

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
		exit(1);
		// exit will immediately jump to the end of main(), 
		// skipping clean-up on the stack, resulting in
		// a pseudo-memory leak, diagnosed by the memory pool.  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int, char*[]) {
	list_type the_list;
	int i = 0;
	for ( ; i<20; i++) {
		add_to_list(the_list, i);
	}
}

