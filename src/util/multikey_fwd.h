// "multikey_fwd.h"
// multidimensional key class, forward declarations

#ifndef	__MULTIKEY_H_FWD__
#define	__MULTIKEY_H_FWD__

// overrideable namespace
#ifndef	MULTIKEY_NAMESPACE
#define	MULTIKEY_NAMESPACE	multikey_namespace
#endif

#include <iosfwd>

namespace MULTIKEY_NAMESPACE {
using namespace std;

//=============================================================================
template <size_t D, class K = int, K init = 0>
class multikey;

//-----------------------------------------------------------------------------
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k); 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator > (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator == (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator >= (const multikey<D,K>& l, const multikey<D,K>& r);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const multikey<D,K>& l, const multikey<D,K>& r);

//-----------------------------------------------------------------------------

}	// end namespace MULTIKEY_NAMESPACE

//=============================================================================
#endif	//	__MULTIKEY_H_FWD__

