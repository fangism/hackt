/**
	\file "multikey_fwd.h"
	Forward declarations or multidimensional key class.  
	$Id: multikey_fwd.h,v 1.10.2.1 2005/02/09 04:14:16 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_H_FWD__
#define	__UTIL_MULTIKEY_H_FWD__

// overrideable namespace
#ifndef	MULTIKEY_NAMESPACE
#define	MULTIKEY_NAMESPACE	util
#endif

#include <iosfwd>

namespace MULTIKEY_NAMESPACE {
using std::ostream;

//=============================================================================

template <size_t D, class K = int>
class multikey;

template <class K>
class multikey_generic;

template <size_t D, class K>
class multikey_generator;

template <class K>
class multikey_generator_generic;

//-----------------------------------------------------------------------------
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k); 

template <class K>
ostream&
operator << (ostream& o, const multikey_generic<K>& k); 

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
#endif	// __UTIL_MULTIKEY_H_FWD__

