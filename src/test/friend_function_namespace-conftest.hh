/**
	\file "friend_function_namespace-conftest.h"
 * This tests for the presence of an internal function in some STL libraries.
 * Moral: don't rely on this.
 */

#include "config.h"

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
using std::_Construct;

class thing {
	typedef	thing		this_type;
#if FRIEND_FUNCTION_HOME_NAMESPACE
#define	FRIEND_NAMESPACE_CONSTRUCT		std::
#else
#define	FRIEND_NAMESPACE_CONSTRUCT
#endif
	friend void FRIEND_NAMESPACE_CONSTRUCT
		_Construct<this_type>(this_type*);
	friend void FRIEND_NAMESPACE_CONSTRUCT
		_Construct<this_type, this_type>(
		this_type* __p, const this_type& __value);

};	// end class thing

