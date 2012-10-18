/**
	\file "util/test/gxx-pr29164-operator-delete.cc"
	Bug test case, jacked up to test a workaround.  
	$Id: one_array_only.cc,v 1.1 2007/03/12 07:38:18 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif	// NDEBUG

#include "one_array_only.hh"
#include <iostream>
#include <cassert>

using std::cout;

//=============================================================================
// global initialization
bool
one_array_only::allocated = false;

one_array_only
one_array_only::pool[256];

//=============================================================================
#if 0
void*
one_array_only::operator new (std::size_t size) throw() {
	cout << "class new operator\n";
	return NULL;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void*
one_array_only::operator new[] (std::size_t size) throw() {
	cout << "class new[] operator\n";
#if 0
	// does not trigger bug
	if (allocated) {
		return NULL;
	} else if (size > 256) {
		cout << "cannot allocate more than [256].\n";
		return NULL;
	} else {
		allocated = true;
		return pool;
	}
#else
#if 0
	return pool;	// does not trigger bug
#else
	return NULL;	// triggers bug: no operator delete[] called
#endif
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
void
one_array_only::operator delete (void *p) throw() {
	cout << "class delete operator\n";
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
one_array_only::operator delete[] (void *p) throw() {
	cout << "class delete[] operator\n";
#if 1
	if (p) {
		assert(p == pool);
		allocated = false;
	}
#endif
}

//=============================================================================

