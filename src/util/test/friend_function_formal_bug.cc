/**
	\file "friend_function_formal_bug.cc"
	Example of a major bug in gcc-3.3, and its workaround.  
	$Id: friend_function_formal_bug.cc,v 1.1.6.1 2005/01/27 23:36:17 fang Exp $
 */

#include <vector>

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
	void
	deallocate(T* p) {
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
	friend void _Construct<this_type>(this_type*);

//*************************** BUG HERE *********************************
#if 0
	// works in gcc-3.4, but dies with cryptic message in gcc-3.3
	friend void _Construct<this_type, this_type>(
		this_type*, const this_type&);
#else
	friend void _Construct<this_type, this_type>(
		this_type* __p, const this_type& __value);
#endif
//*************************** END BUG **********************************

	static void*	operator new (size_t);
	static void	operator delete (void*);
private:
	static void*	operator new (size_t, void*&);
public:
	typedef fake_pool<this_type>		pool_type;
private:
	static pool_type			pool;

};	// end class thing

thing::pool_type
thing::pool;

void*
thing::operator new (size_t s) {
	return pool.allocate();
}

#if 1
void*
thing::operator new (size_t s, void*& p) {
	return p;
}

void
thing::operator delete (void* p) {
	thing* t = reinterpret_cast<thing*>(p);
	pool.deallocate(t);
}
#endif

int
main(int argc, char* argv[]) {
	return 0;
}



