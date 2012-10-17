/**
	\file "friend_function_formal_bug.cc"
	Example of a major bug in gcc-3.3, and its workaround.  
	$Id: friend_function_formal_bug.cc,v 1.6 2011/02/04 02:23:44 fang Exp $
 */

#include <vector>
#include "util/macros.h"
#include "util/size_t.h"

#if 0
// forward declarations
namespace std {

	template <class _T1>
	inline
	void
	_Construct(_T1*);

	template <class _T1, class _T2>
	inline
	void
	_Construct(_T1*, const _T2&);

}
#else
#include "util/STL/construct_fwd.hh"
#endif

using std::_Construct;

template <class T>
class fake_pool {
private:
	std::vector<T>	vec;
public:
//	fake_pool() : vec() { }

	T*
	allocate(void) {
		vec.push_back(T());	// refer to <bits/stl_vector.h>
		return &vec.back();
	}

#if 1
	// some old gcc-3.3 rejects an unused named parameter with -Wall
	// even with -Wno-unused-parameter flag ... arg
	void
	deallocate(T* UNNAMED(p)) {
		// for now do nothing
	}
#endif
};	// end class fake_pool

class thing {
	typedef	thing		this_type;
private:
	thing()	{ }		// private constructor
public:
	~thing() { }

	friend class fake_pool<this_type>;
	friend void FRIEND_NAMESPACE_STD_CONSTRUCT _Construct<this_type>(this_type*);

//*************************** BUG HERE *********************************
#if 0
	// works in gcc-3.4, but dies with cryptic message in gcc-3.3
	friend void _Construct<this_type, this_type>(
		this_type*, const this_type&);
#else
	friend void FRIEND_NAMESPACE_STD_CONSTRUCT _Construct<this_type, this_type>(
		this_type* __p, const this_type& __value);
#endif
//*************************** END BUG **********************************

	static void*	operator new (size_t);
	static void	operator delete (void*);
private:
	static void*	operator new (size_t, void*);
	static void	operator delete (void*, void*);
public:
	typedef fake_pool<this_type>		pool_type;
private:
	static pool_type			pool;

};	// end class thing

thing::pool_type
thing::pool;

void*
thing::operator new (size_t) {
	return pool.allocate();
}

#if 1
void*
thing::operator new (size_t, void* p) {
	return p;
}

void
thing::operator delete (void* p) {
	thing* t = reinterpret_cast<thing*>(p);
	pool.deallocate(t);
}
#endif

int
main(int, char*[]) {
	return 0;
}



