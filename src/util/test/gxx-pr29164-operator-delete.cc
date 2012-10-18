/**
	\file "util/test/gxx-pr29164-operator-delete.cc"
	Bug test case, jacked up to test a workaround.  
	Suspicion: may not be a bug, call to class operator delete []
	is elided when pointer is NULL, not miscompiled.  
	Incidentally, several compilers are reported to behave similarly.  
	This test case will detect if behavior ever changes.  
	$Id: gxx-pr29164-operator-delete.cc,v 1.3 2007/04/15 05:52:35 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif	// NDEBUG

#define	USE_EXCL_ARRAY_PTR		1

#include <iostream>
#if USE_EXCL_ARRAY_PTR
#include "util/memory/excl_array_ptr.hh"
#else
#include "util/memory/deallocation_policy_fwd.hh"
#endif
#include "one_array_only.hh"

using std::cout;

//=============================================================================
#if USE_EXCL_ARRAY_PTR
using util::memory::excl_array_ptr;
#endif

int
main(int, char*[]) {
{
	one_array_only* p = new one_array_only[12];
//	delete [] p;		// triggers bug
#if UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY
	// automatic
	util::memory::operator_delete_array(p);	// also works around bug
#else
	// not automatic
	one_array_only::operator delete [](p);	// works around bug
#endif
}
#if USE_EXCL_ARRAY_PTR
	const excl_array_ptr<one_array_only>::type
		array_ptr(new one_array_only[64]);
	// here not expected to call because excl_ptr suppresses
#endif
	return 0;
}


