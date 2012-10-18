/**
	\file "util/test/one_array_only.hh"
	Bug test case, jacked up to test a workaround.  
	$Id: one_array_only.hh,v 1.1 2007/03/12 07:38:18 fang Exp $
 */

#include "util/size_t.h"		// for size_t

/**
	Class to test whether or not correct operator delete [] is called. 
 */
class one_array_only {
private:
	static bool					allocated;
	static one_array_only				pool[256];
public:
#if 0
	static
	void*
	operator new (std::size_t size) throw() {
		cout << "class new operator\n";
		return NULL;
	}
#endif

	static
	void*
	operator new[] (std::size_t) throw();

#if 0
	static
	void
	operator delete (void *p) throw() {
		cout << "class delete operator\n";
	}
#endif

	static
	void
	operator delete[] (void*) throw();

};	// end class one_array_only

